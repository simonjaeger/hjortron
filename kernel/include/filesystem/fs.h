#ifndef FILESYSTEM_FS_H
#define FILESYSTEM_FS_H

#include <stdint.h>
#include "string.h"

#define FILE_NAME_LENGTH 256
#define DIRECTORY_NAME_LENGTH 256
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

typedef struct fs_dir
{
    char name[DIRECTORY_NAME_LENGTH];
    uint32_t ref;
    uint32_t created;
    uint32_t modified;
    uint32_t accessed;
    struct fs_driver *driver;
} fs_dir;

typedef struct fs_driver
{
    char mnt;
    fs_file *(*open)(string path);
    fs_dir *(*opendir)(string path);
    void (*close)(fs_file *file);
    void (*closedir)(fs_dir *dir);
    void (*read)(fs_file *file, uint32_t *buffer, uint32_t len);
    void (*write)(fs_file *file, uint32_t *buffer, uint32_t len);
    void (*seek)(fs_file *file, uint32_t offset);
} fs_driver;

fs_file *fs_open(string path);
fs_dir *fs_opendir(string path);
void fs_close(fs_file *file);
void fs_closedir(fs_dir *dir);
void fs_read(fs_file *file, uint32_t *buffer, uint32_t len);
void fs_write(fs_file *file, uint32_t *buffer, uint32_t len);
void fs_seek(fs_file *file, uint32_t offset);

void fs_mount(fs_driver *driver, char mnt);

#endif // FILESYSTEM_FS_H