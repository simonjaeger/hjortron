#ifndef CPU_BOOT_H_
#define CPU_BOOT_H_

#include <stdint.h>
#include "string.h"
#include "cpu/gdt.h"

#define E820_ENTRY_FREE 0x1
#define E820_ENTRY_RESERVED 0x2

typedef struct memory_map_entry
{
    uint64_t base;
    uint64_t len;
    uint32_t type;
    uint32_t acpi;
} __attribute__((packed)) memory_map_entry;

typedef struct memory_map
{
    uint32_t len;
    memory_map_entry *entries;
} __attribute__((packed)) memory_map;

typedef struct cpuid
{
    char vendor[13];
    uint32_t features1;
    uint32_t features2;
} __attribute__((packed)) cpuid;


typedef struct boot_info
{
    gdt *gdt;
    uint16_t bpb;
    uint8_t drive;
    cpuid cpuid;
    memory_map memory_map;
} __attribute__((packed)) boot_info;

void disk_info(const boot_info *boot_info);
void cpuid_info(const cpuid *cpuid);
void mmap_info(const memory_map *memory_map);

#endif // CPU_BOOT_H_