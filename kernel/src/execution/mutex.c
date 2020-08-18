#include "execution/mutex.h"

void mutex_init(mutex_t *mutex)
{
    *mutex = 0;
}

void mutex_lock(mutex_t *mutex)
{
    while (!__sync_bool_compare_and_swap(mutex, 0, 1))
    {
        asm("pause");
    }
}

void mutex_unlock(mutex_t *mutex)
{
    *mutex = 0;
}