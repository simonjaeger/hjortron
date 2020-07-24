#include "fs/fs.h"
#include "fs/fat12.h"
#include "display/display.h"
#include "memory/malloc.h"
#include "device/ata.h"
#include "debug.h"

#define DRIVERS_LENGTH 256

static fs_driver *drivers[DRIVERS_LENGTH];

void fs_mount(fs_driver *driver, char mnt)
{
    for (size_t i = 0; i < DRIVERS_LENGTH; i++)
    {
        // Check for available driver slot.
        if (drivers[i] != NULL)
        {
            continue;
        }

        drivers[i] = driver;
        drivers[i]->mnt = mnt;
        debug("mount driver, mnt=%c", mnt);
        return;
    }
    debug("%s", "cannot mount driver");
}

fs_file *fs_open(string path)
{
    // Check for proper mount format.
    if (path[0] != '/' || path[2] != '/')
    {
        debug("%s", "invalid path");
        return NULL;
    }
    path++;

    for (size_t i = 0; i < DRIVERS_LENGTH; i++)
    {
        // Check for end of driver array.
        if (drivers[i] == NULL)
        {
            break;
        }

        // Check for correct driver.
        if (drivers[i]->mnt != path[0])
        {
            continue;
        }

        path += 2;

        // Open file and assign driver.
        fs_file *file = drivers[i]->open(path);
        file->driver = drivers[i];
        return file;
    }

    debug("%s", "cannot find driver");
    return NULL;
}

static fat12_extended_bios_parameter_block *bios_parameter_block = NULL;
static size_t lba_fat1;
static size_t lba_root;
static size_t lba_data;

void fs_init(const boot_info *boot_info)
{
    bios_parameter_block = (fat12_extended_bios_parameter_block *)(uint32_t)boot_info->bpb;

    char oem_identifier[9];
    strset(oem_identifier, '\0', 9);
    strcpy(bios_parameter_block->oem_identifier, oem_identifier, 8);

    char volume_label[12];
    strset(volume_label, '\0', 12);
    strcpy(bios_parameter_block->volume_label, volume_label, 11);

    char system_identifier[9];
    strset(system_identifier, '\0', 9);
    strcpy(bios_parameter_block->system_identifier, system_identifier, 8);

    printf("FAT12 Information:\n");
    printf("OEM Identifier: %s\n", oem_identifier);
    printf("System Identifier: %s\n", system_identifier);
    printf("Volume Identifier: %d\n", bios_parameter_block->volume_identifier);
    printf("Volume Label: %s\n", volume_label);
    printf("Total sectors: %d\n", bios_parameter_block->sectors);
    printf("Root entries: %d\n", bios_parameter_block->directory_entries);
    printf("Sectors per FAT: %d\n", bios_parameter_block->sectors_per_fat);
    printf("Sectors per cluster: %d\n", bios_parameter_block->sectors_per_cluster);
    printf("Sectors per track: %d\n", bios_parameter_block->sectors_per_track);
    printf("Bytes per sector: %d\n", bios_parameter_block->bytes_per_sector);
    printf("Reserved sectors: %d\n", bios_parameter_block->reserved_sectors);

    lba_fat1 = bios_parameter_block->reserved_sectors;
    lba_root = lba_fat1 + bios_parameter_block->sectors_per_fat * bios_parameter_block->fats;
    lba_data = lba_root + bios_parameter_block->directory_entries * FAT12_DIRECTORY_ENTRY_SIZE / bios_parameter_block->bytes_per_sector;

    debug("initialized, fat1=%lx, root=%lx, data=%lx", ((uint64_t)lba_fat1), ((uint64_t)lba_root), ((uint64_t)lba_data));
}

void fs_print(fat12_directory_entry *entries, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        fat12_directory_entry entry = entries[i];
        if (entry.attributes == 0x10 || entry.attributes == 0x20)
        {
            char filename[FAT12_FILENAME_LENGTH + 1];
            strset(filename, '\0', FAT12_FILENAME_LENGTH + 1);
            strcpy(entry.filename, filename, FAT12_FILENAME_LENGTH);
            string type = entry.attributes == 0x10 ? "FOLDER" : "FILE  ";
            printf("%s %s %d\n", filename, type, entry.size);
        }
    }
}

bool fs_match(string filename1, string filename2)
{
    for (size_t i = 0; i < FAT12_FILENAME_LENGTH; i++)
    {
        if (!filename1[i] || filename1[i] != filename2[i])
        {
            return false;
        }
    }
    return true;
}

fat12_directory_entry *fs_find(fat12_directory_entry *entries, size_t count, const string filename, uint8_t attributes)
{
    for (size_t i = 0; i < count; i++)
    {
        if (entries[i].attributes == attributes && fs_match(entries[i].filename, filename))
        {
            return &entries[i];
        }
    }
    return NULL;
}

uint16_t fs_next_cluster(uint16_t cluster)
{
    // Compute offsets.
    size_t byte_offset = cluster + cluster / 2;
    size_t lba_offset = byte_offset / bios_parameter_block->bytes_per_sector;
    size_t sector_offset = byte_offset % bios_parameter_block->bytes_per_sector;

    // Read FAT.
    uint8_t *buffer = (uint8_t *)malloc(bios_parameter_block->bytes_per_sector);
    ata_read((uint16_t *)buffer, ATA_BUS_PRIMARY, lba_fat1 + lba_offset, 1);

    // TODO: Cache buffer.

    // Get next cluster and deallocate buffer.
    uint16_t next_cluster = *((uint16_t *)&buffer[sector_offset]);
    free(buffer);

    // Remove 4 bits from the value that belong to another cluster.
    if (cluster & 1)
    {
        return next_cluster >> 4;
    }
    return next_cluster & 0xFFF;
}

size_t fs_clusters(uint16_t cluster)
{
    // Check if cluster is within range of valid cluster values.
    size_t len = 0;
    while (cluster >= 0x2 && cluster <= 0xFEF)
    {
        cluster = fs_next_cluster(cluster);
        len++;
    }
    return len;
}

void fs_read_cluster(uint16_t *buffer, uint16_t cluster)
{
    // Compute data offset (LBA).
    uint32_t lba_offset = cluster;
    lba_offset -= 2;
    lba_offset *= bios_parameter_block->sectors_per_cluster;

    // Read cluster.
    ata_read(buffer, ATA_BUS_PRIMARY, lba_data + lba_offset, bios_parameter_block->sectors_per_cluster);
}

void *fs_read_file(const fat12_directory_entry *entry)
{
    // Compute cluster and clusters.
    uint16_t cluster = (entry->cluster_high << 16) | entry->cluster_low;
    size_t clusters = fs_clusters(cluster);
    size_t cluster_size = bios_parameter_block->bytes_per_sector * bios_parameter_block->sectors_per_cluster;

    // Allocate buffer for entire file.
    uint8_t *buffer = (uint8_t *)malloc(cluster_size * clusters);
    for (size_t i = 0; i < clusters; i++)
    {
        if (i > 0)
        {
            cluster = fs_next_cluster(cluster);
        }
        debug("%x", cluster);
        fs_read_cluster((uint16_t *)(&buffer[cluster_size * i]), cluster);
    }
    return buffer;
}

void fs_list(const string path)
{
    printf("\n%s\n", path);

    // Compute sectors and create buffer for root directory.
    size_t directory_size = bios_parameter_block->directory_entries * FAT12_DIRECTORY_ENTRY_SIZE / bios_parameter_block->bytes_per_sector;
    uint32_t *buffer = (uint32_t *)malloc(directory_size * bios_parameter_block->bytes_per_sector);

    // Read entire root directory.
    ata_read((uint16_t *)buffer, ATA_BUS_PRIMARY, lba_root, directory_size);

    fat12_directory_entry *entries = (fat12_directory_entry *)buffer;
    // fs_print(entries, bios_parameter_block->directory_entries);

    fat12_directory_entry *entry = fs_find(entries, bios_parameter_block->directory_entries, "DATA1      ", FAT12_ATTRIBUTE_DIRECTORY);
    // fat12_directory_entry *entry = fs_find(entries, bios_parameter_block->directory_entries, "KERNEL  BIN", FAT12_ATTRIBUTE_ARCHIVE);
    if (entry)
    {
        entries = (fat12_directory_entry *)fs_read_file(entry);
        entry = fs_find(entries, bios_parameter_block->directory_entries, "DATA2      ", FAT12_ATTRIBUTE_DIRECTORY);

        // fat12_directory_entry *entry = fs_find(entries, bios_parameter_block->directory_entries, "KERNEL  BIN", FAT12_ATTRIBUTE_ARCHIVE);
        if (entry)
        {
            entries = (fat12_directory_entry *)fs_read_file(entry);
            fs_print(entries, directory_size);

            entry = fs_find(entries, bios_parameter_block->directory_entries, "LOREM   TXT", FAT12_ATTRIBUTE_ARCHIVE);
            if (entry)
            {
                void *file = fs_read_file(entry);
                debug("%s", file);
                free(file);
            }
            else
            {
                printf("%s", "3 not found");
            }
        }
        else
        {
            printf("%s", "2 not found");
        }

        // void *file = fs_read_file(entry);
        // free(file);
    }
    else
    {
        printf("%s", "1 not found");
    }

    free(buffer);
}