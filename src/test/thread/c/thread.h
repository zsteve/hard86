#ifndef THREAD_H
#define THREAD_H

#include <windows.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef unsigned long uint32;

#define FALSE 0
#define TRUE 1

typedef struct{
    HANDLE hThread;
    uint32 threadID;
}THREAD;

int Thread_Create(LPTHREAD_START_ROUTINE threadProc,
                    void* threadParam,
                    THREAD* threadStruct);

int Thread_Start(THREAD* threadStruct);

int Thread_Pause(THREAD* threadStruct);

int Thread_Close(THREAD* threadStruct);

#ifdef __cplusplus
}
#endif

#endif
