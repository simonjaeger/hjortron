#include "filesystem/fs.h"
#include "filesystem/fat12.h"
#include "display/display.h"
#include "memory/malloc.h"
#include "drivers/ata.h"
#include "debug.h"
#include "assert.h"

#define DRIVERS_LENGTH 256

static fs_driver *drivers[DRIVERS_LENGTH];

fs_file *fs_open(string path)
{
    /* Check mount format. */
    if (path[0] != '/' || path[2] != '/')
    {
        error("%s", "invalid path");
        return NULL;
    }
    path++;

    for (size_t i = 0; i < DRIVERS_LENGTH; i++)
    {
        if (drivers[i] == NULL)
        {
            break;
        }

        /* Check for correct driver. */
        if (drivers[i]->mnt != path[0])
        {
            continue;
        }

        path += 2;

        /* Open file and assign driver. */
        fs_file *file = drivers[i]->open(path);
        file->driver = drivers[i];
        return file;
    }

    error("%s", "cannot find driver");
    return NULL;
}

fs_dir *fs_opendir(string path)
{
    /* Check mount format. */
    if (path[0] != '/' || path[2] != '/')
    {
        error("%s", "invalid path");
        return NULL;
    }
    path++;

    for (size_t i = 0; i < DRIVERS_LENGTH; i++)
    {
        if (drivers[i] == NULL)
        {
            break;
        }

        /* Check for correct driver. */
        if (drivers[i]->mnt != path[0])
        {
            continue;
        }

        path += 2;

        /* Open directory and assign driver. */
        fs_dir *dir = drivers[i]->opendir(path);
        dir->driver = drivers[i];
        return dir;
    }

    error("%s", "cannot find driver");
    return NULL;
}

void fs_close(fs_file *file)
{
    assert(file);
    assert(file->driver);
    assert(file->driver->mnt != DRIVER_MOUNT_UNASSIGNED);
    file->driver->close(file);
}

void fs_closedir(fs_dir *dir)
{
    assert(dir);
    assert(dir->driver);
    assert(dir->driver->mnt != DRIVER_MOUNT_UNASSIGNED);
    dir->driver->closedir(dir);
}

void fs_read(fs_file *file, uint32_t *buffer, uint32_t len)
{
    assert(file);
    assert(file->driver);
    assert(file->driver->mnt != DRIVER_MOUNT_UNASSIGNED);
    file->driver->read(file, buffer, len);
}

void fs_readdir(fs_dir *dir, fs_dirent **entries, uint32_t *len)
{
    assert(dir);
    assert(dir->driver);
    assert(dir->driver->mnt != DRIVER_MOUNT_UNASSIGNED);
    dir->driver->readdir(dir, entries, len);
}

void fs_write(fs_file *file, uint32_t *buffer, uint32_t len)
{
    assert(file);
    assert(file->driver);
    assert(file->driver->mnt != DRIVER_MOUNT_UNASSIGNED);
    file->driver->write(file, buffer, len);
}

void fs_seek(fs_file *file, uint32_t offset)
{
    assert(file);
    assert(file->driver);
    assert(file->driver->mnt != DRIVER_MOUNT_UNASSIGNED);
    file->driver->seek(file, offset);
}

void fs_mount(fs_driver *driver, char mnt)
{
    for (size_t i = 0; i < DRIVERS_LENGTH; i++)
    {
        if (drivers[i] != NULL)
        {
            continue;
        }

        drivers[i] = driver;
        drivers[i]->mnt = mnt;
        info("mount driver, mnt=%c", mnt);
        return;
    }
    error("%s", "cannot mount driver");
}