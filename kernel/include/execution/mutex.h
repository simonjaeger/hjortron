#ifndef EXECUTION_MUTEX_H_
#define EXECUTION_MUTEX_H_

typedef volatile int mutex_t;

void mutex_init(mutex_t *mutex);
void mutex_lock(mutex_t *mutex);
void mutex_unlock(mutex_t *mutex);

#endif // EXECUTION_MUTEX_H_