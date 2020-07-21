#ifndef FS_FS_H
#define FS_FS_H

#include "boot.h"

void fs_init(const boot_info* boot_info);
void fs_ls_test();

#endif // FS_FS_H