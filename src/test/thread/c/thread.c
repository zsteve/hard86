#include <Windows.h>
#include "thread.h"

int Thread_Create(LPTHREAD_START_ROUTINE threadProc,
                    void* threadParam,
                    THREAD* threadStruct)
{
    threadStruct->hThread=CreateThread(NULL,
                                       0,
                                       threadProc,
                                       threadParam,
                                       CREATE_SUSPENDED,
                                       &threadStruct->threadID);
    if(threadStruct->hThread){
        return TRUE;
    }
    return FALSE;
}

int Thread_Start(THREAD* threadStruct)
{
    return (ResumeThread(threadStruct->hThread)==-1) ? FALSE : TRUE;
}

int Thread_Pause(THREAD* threadStruct)
{
    return (SuspendThread(threadStruct->hThread)==-1) ? FALSE : TRUE;
}

int Thread_Close(THREAD* threadStruct)
{
    return CloseHandle(threadStruct->hThread);
}
