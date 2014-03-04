#ifndef MUTEX_C_H
#define MUTEX_C_H

#ifdef __cplusplus
extern "C"{
#endif

#define TRUE 1
#define FALSE 0

typedef void* MUTEX;

MUTEX Mutex_Create();

int Mutex_Lock(MUTEX mutex);

int Mutex_TryLock(MUTEX mutex);

int Mutex_Unlock(MUTEX mutex);

int Mutex_Close(MUTEX mutex);

#ifdef __cplusplus
}
#endif

#endif
