#include <stdbool.h>
#include "filesystem/fat12.h"
#include "device/ata.h"
#include "memory/malloc.h"
#include "debug.h"

// TODO: Move to driver specific metadata.
// TODO: Support ATA_BUS_SECONDARY.

static fat12_extended_bios_parameter_block *bpb;
static size_t lba_fat1;
static size_t lba_root;
static size_t lba_data;
static size_t ata_bus;

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
bool fat12_valid_cluster(uint16_t cluster)
{
    // Check if cluster is within range of valid cluster values.
    return cluster >= 0x2 && cluster <= 0xFEF;
}

uint32_t fat12_cluster_lba(uint16_t cluster)
{
    // Compute LBA.
    uint32_t lba_offset = cluster;
    lba_offset -= 2;
    lba_offset *= bpb->sectors_per_cluster;
    return lba_data + lba_offset;
}

uint16_t fat12_read_fat(uint16_t cluster)
{
    // Compute offsets.
    size_t byte_offset = cluster + cluster / 2;
    size_t lba_offset = byte_offset / bpb->bytes_per_sector;
    size_t sector_offset = byte_offset % bpb->bytes_per_sector;

    // Read sector.
    uint8_t *buffer = (uint8_t *)malloc(bpb->bytes_per_sector);
    ata_read((uint16_t *)buffer, ata_bus, lba_fat1 + lba_offset, 1);

    // TODO: Cache buffer.

    // Get next cluster and deallocate buffer.
    uint16_t next_cluster = ((uint16_t *)(&buffer[sector_offset]))[0];

    free(buffer);

    // Remove 4 bits from the value that belong to another cluster.
    if (cluster & 1)
    {
        return next_cluster >> 4;
    }
    return next_cluster & 0xFFF;
}

void fat12_read_cluster_chain(uint8_t **buffer, size_t *len, uint16_t cluster)
{
    size_t clusters[256];
    *len = 0;
    for (size_t i = 0; i < 256; i++)
    {
        if (!fat12_valid_cluster(cluster))
        {
            break;
        }

        // Get next cluster.
        clusters[(*len)++] = cluster;
        cluster = fat12_read_fat(cluster);
    }

    // Allocate buffer for chain.
    *buffer = (uint8_t *)malloc(bpb->bytes_per_sector * bpb->sectors_per_cluster * *len);

    // Read clusters.
    for (size_t i = 0; i < *len; i++)
    {
        uint32_t lba = fat12_cluster_lba(clusters[i]);
        ata_read((uint16_t *)(buffer[bpb->bytes_per_sector * bpb->sectors_per_cluster * i]), ata_bus, lba, bpb->sectors_per_cluster);
    }
}

void fat12_read_directory(fat12_directory_entry **entries, size_t *len, uint16_t cluster)
{
    // Read root directory, consecutive sectors.
    if (!cluster)
    {
        uint32_t *buffer = (uint32_t *)malloc(bpb->directory_entries * FAT12_DIRECTORY_ENTRY_SIZE);
        ata_read((uint16_t *)buffer, ata_bus, lba_root, bpb->directory_entries * FAT12_DIRECTORY_ENTRY_SIZE / bpb->bytes_per_sector);
        *entries = (fat12_directory_entry *)buffer;
        *len = bpb->directory_entries;
        return;
    }

    // Read chain of clusters for directory.
    size_t chain_len = 0;
    fat12_read_cluster_chain((uint8_t **)entries, &chain_len, cluster);
    *len = chain_len * bpb->sectors_per_cluster * bpb->bytes_per_sector / FAT12_DIRECTORY_ENTRY_SIZE;
}

fs_file *fat12_open(string path)
{
    info("open, path=%s", path);

    // Read root directory.
    fat12_directory_entry *entries;
    size_t entries_len = bpb->directory_entries;
    fat12_read_directory(&entries, &entries_len, 0);

    char buffer[FAT12_FILENAME_LENGTH + 1];
    strset(buffer, ' ', FAT12_FILENAME_LENGTH + 1);
    size_t i = 0;
    size_t j = 0;

    while (true)
    {
        if (path[j] == '\0')
        {
            if (strlen(buffer) == 0)
            {
                error("%s", "empty buffer");
                return NULL;
            }

            // Find extension.
            char ext[3] = {0, 0, 0};
            for (size_t k = 1; k < FAT12_FILENAME_LENGTH; k++)
            {
                if (ext[0])
                {
                    if (k >= 8)
                    {
                        buffer[k] = ext[k - 8];
                        continue;
                    }

                    buffer[k] = ' ';
                    continue;
                }

                if (buffer[k] == '.')
                {
                    ext[0] = buffer[k + 1];
                    ext[1] = buffer[k + 2];
                    ext[2] = buffer[k + 3];
                    k--;
                }
            }

            if (!ext[0])
            {
                error("%s", "invalid path");
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
                error("%s", "cannot find file");
                return NULL;
            }

            // Create file.
            fs_file *file = (fs_file *)malloc(sizeof(fs_file));
            strset(file->name, '\0', FILE_NAME_LENGTH);
            strcpy(entry->filename, file->name, FAT12_FILENAME_LENGTH);

            file->ref = (entry->cluster_high << 16) | entry->cluster_low;
            file->len = entry->size;
            file->offset = 0;

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
                error("%s", "cannot find directory");
                return NULL;
            }

            // Compute cluster and deallocate previous buffer.
            uint16_t cluster = (entry->cluster_high << 16) | entry->cluster_low;
            free(entries);

            // Read directory.
            fat12_read_directory(&entries, &entries_len, cluster);

            // Clear buffer.
            strset(buffer, ' ', FAT12_FILENAME_LENGTH + 1);
            i = 0;
            j++;
            continue;
        }
        else if (i == FAT12_FILENAME_LENGTH)
        {
            error("%s", "buffer overflow");
            return NULL;
        }

        buffer[i++] = path[j++];
    }
    return NULL;
}

void fat12_close(fs_file *file)
{
    info("close, file=%s", file->name);
    free(file);
}

void fat12_read(fs_file *file, uint32_t *buffer, uint32_t len)
{
    info("read, file=%s, ref=%x, buffer=%x, length=%x", file->name, file->ref, ((uint32_t)buffer), len);

    if (file->offset + len > file->len)
    {
        len = file->len - file->offset;
    }

    if (len == 0)
    {
        return;
    }

    // Compute clusters and offsets.
    size_t cluster_begin = file->offset / bpb->bytes_per_sector / bpb->sectors_per_cluster;
    size_t offset_begin = file->offset % (bpb->bytes_per_sector * bpb->sectors_per_cluster);
    size_t cluster_end = (file->offset + len) / bpb->bytes_per_sector / bpb->sectors_per_cluster;
    size_t offset_end = (file->offset + len) % (bpb->bytes_per_sector * bpb->sectors_per_cluster);

    // Allocate buffer to copy slices from.
    uint8_t *src_buffer = (uint8_t *)malloc(bpb->bytes_per_sector * bpb->sectors_per_cluster);
    uint8_t *dest_buffer = (uint8_t *)buffer;

    // Iterate the cluster chain from the start.
    size_t cluster = file->ref;
    size_t dest_offset = 0;
    size_t src_offset = 0;
    size_t src_len = 0;

    for (size_t i = 0; i <= cluster_end; i++)
    {
        if (!fat12_valid_cluster(cluster))
        {
            error("%s", "invalid cluster");
            return;
        }

        if (i >= cluster_begin && i <= cluster_end)
        {
            // Read cluster.
            uint32_t lba = fat12_cluster_lba(cluster);
            ata_read((uint16_t *)src_buffer, ata_bus, lba, bpb->sectors_per_cluster);

            // Determine slice of buffer to copy to destination buffer.
            if (i == cluster_begin && i == cluster_end)
            {
                src_offset = offset_begin;
                src_len = offset_end - offset_begin;
            }
            else if (i == cluster_begin)
            {
                src_offset = offset_begin;
                src_len = bpb->bytes_per_sector * bpb->sectors_per_cluster - offset_begin;
            }
            else if (i == cluster_end)
            {
                src_offset = 0;
                src_len = offset_end;
            }

            // Copy slice and increment buffer and file offset.
            memcpy((void *)&dest_buffer[dest_offset], (void *)&src_buffer[src_offset], src_len);
            dest_offset += src_len;
            file->offset += src_len;
        }

        // Get next cluster, if any.
        if (i != cluster_end)
        {
            cluster = fat12_read_fat(cluster);
        }
    }

    free(src_buffer);
}

void fat12_write(fs_file *file, uint32_t *buffer, uint32_t len)
{
    info("write, file=%s, ref=%x, buffer=%x, length=%x", file->name, file->ref, ((uint32_t)buffer), len);
    error("%s", "not implemented");
}

void fat12_seek(fs_file *file, uint32_t offset)
{
    info("seek, file=%s, ref=%x, offset=%x", file->name, file->ref, offset);
    file->offset = (file->offset + offset > file->len) ? file->len : offset;
}

fs_driver *fat12_init(const fat12_extended_bios_parameter_block *bios_parameter_block)
{
    // Compute offsets within drive.
    bpb = (fat12_extended_bios_parameter_block *)(uint32_t)bios_parameter_block;
    lba_fat1 = bpb->reserved_sectors;
    lba_root = lba_fat1 + bpb->sectors_per_fat * bpb->fats;
    lba_data = lba_root + bpb->directory_entries * FAT12_DIRECTORY_ENTRY_SIZE / bpb->bytes_per_sector;

    // TODO: Accept parameter for ATA bus.
    ata_bus = ATA_BUS_PRIMARY;

    // Create driver.
    fs_driver *driver = (fs_driver *)malloc(sizeof(fs_driver));
    driver->mnt = DRIVER_MOUNT_UNASSIGNED;
    driver->open = fat12_open;
    driver->close = fat12_close;
    driver->read = fat12_read;
    driver->write = fat12_write;
    driver->seek = fat12_seek;

    info("initialized, fat1=%lx, root=%lx, data=%lx", ((uint64_t)lba_fat1), ((uint64_t)lba_root), ((uint64_t)lba_data));
    return driver;
}

void fat12_destroy(fs_driver *driver)
{
    free(driver);
}