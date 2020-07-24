#ifndef FS_FS_H
#define FS_FS_H

#include "boot.h"
#include "types.h"
#include "string.h"

#define FILE_NAME_LENGTH 256
#define DRIVER_MOUNT_UNASSIGNED 0

struct fs_driver;

typedef struct fs_file
{
    char name[FILE_NAME_LENGTH];
    uint32_t len;
    uint32_t offset;
    uint32_t ref;
    uint32_t created;
    uint32_t modified;
    uint32_t accessed;
    struct fs_driver *driver;
} fs_file;

typedef struct fs_driver
{
    char mnt;
    fs_file *(*open)(string path);
    void (*close)(fs_file *file);
    void (*read)(fs_file *file, uint32_t *buffer, uint32_t len);
    void (*write)(fs_file *file, uint32_t *buffer, uint32_t len);
    void (*seek)(fs_file *file, uint32_t offset);
} fs_driver;

void fs_mount(fs_driver *driver, char mnt);

fs_file *fs_open(string path);

void fs_init(const boot_info *boot_info);
void fs_list(const string path);

void fs_ls_test();

#endif // FS_FS_H