#include <windows.h>
#include "mutex.h"

MUTEX Mutex_Create()
{
    return CreateMutex(NULL, FALSE, NULL);
}

int Mutex_Lock(MUTEX mutex)
{
    int retv=WaitForSingleObject(mutex, INFINITE);
    if(retv==WAIT_FAILED) return FALSE;
    return TRUE;
}

int Mutex_TryLock(MUTEX mutex)
{
    int retv=WaitForSingleObject(mutex, 0);
    if(retv==WAIT_OBJECT_0) return TRUE;
    return FALSE;
}

int Mutex_Unlock(MUTEX mutex)
{
    int retv=ReleaseMutex(mutex);
    return retv ? TRUE : FALSE;
}

int Mutex_Close(MUTEX mutex)
{
    return CloseHandle(mutex);
}
