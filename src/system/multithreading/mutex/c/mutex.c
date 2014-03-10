/**
 * @file C mutex functions
 * C mutex functions for hard86
 * Stephen Zhang, 2014
 */

#include <windows.h>
#include "mutex.h"

#include "../../../../global/defines.h"
#include "../../../../global/typedefs.h"ddd

MUTEX mutex_create()
{
    return CreateMutex(NULL, FALSE, NULL);
}

int mutex_lock(MUTEX mutex)
{
    int retv=WaitForSingleObject(mutex, INFINITE);
    if(retv==WAIT_FAILED) return FALSE;
    return TRUE;
}

/**
 * @return true if mutex is locked, false otherwise
 */
int mutex_trylock(MUTEX mutex)
{
    int retv=WaitForSingleObject(mutex, 0);
    if(retv==WAIT_TIMEOUT) return TRUE;
    return FALSE;
}

int mutex_unlock(MUTEX mutex)
{
    int retv=ReleaseMutex(mutex);
    return retv ? TRUE : FALSE;
}

int mutex_close(MUTEX mutex)
{
    return CloseHandle(mutex);
}
