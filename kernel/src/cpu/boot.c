#include "cpu/boot.h"
#include "debug.h"

void disk_info(const boot_info *boot_info)
{
    string type;
    if (boot_info->drive > 0xE0)
    {
        type = "hdd";
    }
    else if (boot_info->drive == 0xE0)
    {
        type = "cd-rom";
    }
    else if (boot_info->drive > 0x7F)
    {
        type = "hdd";
    }
    else
    {
        type = "fdd";
    }
    info("drive=%x, type=%s", boot_info->drive, type);
}

void cpuid_info(const cpuid *cpuid)
{
    info("vendor=%s, features=%x", cpuid->vendor, cpuid->features1);
}

void mmap_info(const memory_map *memory_map)
{
    for (size_t i = 0; i < memory_map->len; i++)
    {
        memory_map_entry *entry = &memory_map->entries[i];
        string type = "unknown";
        if (entry->type == E820_ENTRY_FREE)
        {
            type = "free";
        }
        else if (entry->type == E820_ENTRY_RESERVED)
        {
            type = "reserved";
        }
        info("base=%lx, length=%lx, type=%s", entry->base, entry->len, type);
    }
}