#include "filesystem/fs.h"
#include "filesystem/fat12.h"
#include "display/display.h"
#include "memory/malloc.h"
#include "drivers/ata.h"
#include "debug.h"

#define DRIVERS_LENGTH 256

static fs_driver *drivers[DRIVERS_LENGTH];

fs_file *fs_open(string path)
{
    // Check mount format.
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

    error("%s", "cannot find driver");
    return NULL;
}

void fs_close(fs_file *file)
{
    if (file == NULL)
    {
        error("%s", "invalid file");
        return;
    }

    if (file->driver == NULL || file->driver->mnt == DRIVER_MOUNT_UNASSIGNED)
    {
        error("%s", "cannot find driver");
        return;
    }

    file->driver->close(file);
}

void fs_read(fs_file *file, uint32_t *buffer, uint32_t len)
{
    if (file == NULL)
    {
        error("%s", "invalid file");
        return;
    }

    if (file->driver == NULL || file->driver->mnt == DRIVER_MOUNT_UNASSIGNED)
    {
        error("%s", "cannot find driver");
        return;
    }

    file->driver->read(file, buffer, len);
}

void fs_write(fs_file *file, uint32_t *buffer, uint32_t len)
{
    if (file == NULL)
    {
        error("%s", "invalid file");
        return;
    }

    if (file->driver == NULL || file->driver->mnt == DRIVER_MOUNT_UNASSIGNED)
    {
        error("%s", "cannot find driver");
        return;
    }

    file->driver->write(file, buffer, len);
}

void fs_seek(fs_file *file, uint32_t offset)
{
    if (file == NULL)
    {
        error("%s", "invalid file");
        return;
    }

    if (file->driver == NULL || file->driver->mnt == DRIVER_MOUNT_UNASSIGNED)
    {
        error("%s", "cannot find driver");
        return;
    }

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