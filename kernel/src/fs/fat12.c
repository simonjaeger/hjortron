#include "fs/fat12.h"
#include "device/ata.h"
#include "memory/malloc.h"
#include "debug.h"

// TODO: Move to driver specific metadata.
static fat12_extended_bios_parameter_block *bpb;
static size_t lba_fat1;
static size_t lba_root;
static size_t lba_data;

bool fat12_strcmp(string str1, string str2, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        if (!str1[i] || str1[i] != str2[i])
        {
            return false;
        }
    }
    return true;
}

uint16_t fat12_read_fat(uint16_t cluster)
{
    // Compute offsets.
    size_t byte_offset = cluster + cluster / 2;
    size_t lba_offset = byte_offset / bpb->bytes_per_sector;
    size_t sector_offset = byte_offset % bpb->bytes_per_sector;

    // Read FAT.
    uint8_t *buffer = (uint8_t *)malloc(bpb->bytes_per_sector);
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

void fat12_read_cluster_chain(uint32_t **buffer, size_t *len, uint16_t cluster)
{
    size_t clusters[256];
    *len = 0;
    for (size_t i = 0; i < 256; i++)
    {
        // Check if cluster is within range of valid cluster values.
        if (cluster < 0x2 || cluster > 0xFEF)
        {
            break;
        }

        clusters[(*len)++] = cluster;
        cluster = fat12_read_fat(cluster);
    }

    // Allocate buffer for entire chain.
    *buffer = (uint32_t *)malloc(bpb->bytes_per_sector * bpb->sectors_per_cluster * *len);
    for (size_t i = 0; i < *len; i++)
    {
        // Compute data offset (LBA).
        uint32_t lba_offset = clusters[i];
        lba_offset -= 2;
        lba_offset *= bpb->sectors_per_cluster;

        // Read cluster.
        ata_read((uint16_t *)(buffer[bpb->bytes_per_sector * bpb->sectors_per_cluster * i]), ATA_BUS_PRIMARY, lba_data + lba_offset, bpb->sectors_per_cluster);
    }
}

void fat12_read_directory(fat12_directory_entry **entries, size_t *len, uint16_t cluster)
{
    if (!cluster)
    {
        uint32_t *buffer = (uint32_t *)malloc(bpb->directory_entries * FAT12_DIRECTORY_ENTRY_SIZE);
        ata_read((uint16_t *)buffer, ATA_BUS_PRIMARY, lba_root, bpb->directory_entries * FAT12_DIRECTORY_ENTRY_SIZE / bpb->bytes_per_sector);
        *entries = (fat12_directory_entry *)buffer;
        *len = bpb->directory_entries;
        return;
    }

    // Read entrie chain of clusters for directory.
    size_t chain_len = 0;
    fat12_read_cluster_chain((uint32_t **)entries, &chain_len, cluster);
    *len = chain_len * bpb->sectors_per_cluster * bpb->bytes_per_sector / FAT12_DIRECTORY_ENTRY_SIZE;
}

fs_file *fat12_open(string path)
{
    debug("open path=%s", path);

    // Read root directory.
    fat12_directory_entry *entries;
    size_t entries_len = bpb->directory_entries;
    fat12_read_directory(&entries, &entries_len, 0);

    char buffer[FAT12_FILENAME_LENGTH];
    strset(buffer, '\0', FAT12_FILENAME_LENGTH);
    size_t i = 0;
    size_t j = 0;

    while (true)
    {
        if (path[j] == '\0')
        {
            if (strlen(buffer) == 0)
            {
                debug("%s", "empty buffer");
                return NULL;
            }

            // Find file within directory.
            fat12_directory_entry *entry = NULL;
            for (size_t i = 0; i < entries_len; i++)
            {
                if (entries[i].attributes == FAT12_ATTRIBUTE_ARCHIVE &&
                    fat12_strcmp(entries[i].filename, buffer, FAT12_FILENAME_LENGTH))
                {
                    entry = &entries[i];
                    break;
                }
            }

            // Check if the file was found.
            if (entry == NULL)
            {
                debug("%s", "cannot find file");
                return NULL;
            }

            // Create file.
            fs_file *file = (fs_file *)malloc(sizeof(fs_file));
            strset(file->name, '\0', FILE_NAME_LENGTH);
            strcpy(entry->filename, file->name, FAT12_FILENAME_LENGTH);

            file->inode = (entry->cluster_high << 16) | entry->cluster_low;
            file->length = entry->size;

            // Deallocate previous buffer.
            free(entries);
            return file;
        }
        else if (path[j] == '/')
        {
            // Find directory within directory.
            fat12_directory_entry *entry = NULL;
            for (size_t i = 0; i < entries_len; i++)
            {
                if (entries[i].attributes == FAT12_ATTRIBUTE_DIRECTORY &&
                    fat12_strcmp(entries[i].filename, buffer, FAT12_FILENAME_LENGTH))
                {
                    entry = &entries[i];
                    break;
                }
            }

            // Check if the directory was found.
            if (entry == NULL)
            {
                debug("%s", "cannot find directory");
                return NULL;
            }

            // Compute cluster and deallocate previous buffer.
            uint16_t cluster = (entry->cluster_high << 16) | entry->cluster_low;
            free(entries);

            // Read directory.
            fat12_read_directory(&entries, &entries_len, cluster);

            // Clear buffer.
            strset(buffer, '\0', FAT12_FILENAME_LENGTH);
            i = 0;
            j++;
            continue;
        }
        else if (i == FAT12_FILENAME_LENGTH)
        {
            debug("%s", "buffer overflow");
            return NULL;
        }

        buffer[i++] = path[j++];
    }
    return NULL;
}

fs_driver *fat12_init(const fat12_extended_bios_parameter_block *bios_parameter_block, char mnt[7])
{
    // Compute offsets within drive.
    bpb = (fat12_extended_bios_parameter_block *)(uint32_t)bios_parameter_block;
    lba_fat1 = bpb->reserved_sectors;
    lba_root = lba_fat1 + bpb->sectors_per_fat * bpb->fats;
    lba_data = lba_root + bpb->directory_entries * FAT12_DIRECTORY_ENTRY_SIZE / bpb->bytes_per_sector;

    // Create driver.
    fs_driver *driver = (fs_driver *)malloc(sizeof(fs_driver));
    strset(driver->mnt, '\0', 8);
    strcpy(mnt, driver->mnt, 7);
    driver->open = fat12_open;

    debug("initialized, fat1=%lx, root=%lx, data=%lx", ((uint64_t)lba_fat1), ((uint64_t)lba_root), ((uint64_t)lba_data));
    return driver;
}

void fat12_destroy(fs_driver *driver)
{
    free(driver);
}