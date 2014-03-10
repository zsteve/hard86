/**
 * @file C Thread functions
 * C thread functions for hard86
 * Stephen Zhang, 2014
 */

#ifndef THREAD_H
#define THREAD_H

#include <windows.h>

#include "../../../../global/defines.h"
#include "../../../../global/typedefs.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct{
    HANDLE handle;
    uint32 id;
}THREAD;

int thread_create(LPTHREAD_START_ROUTINE proc,
                    void* param,
                    THREAD* thread);

int thread_start(THREAD* thread);

int thread_pause(THREAD* thread);

int thread_kill(THREAD* thread);

int thread_close(THREAD* thread);

#ifdef __cplusplus
}
#endif

#endif
