/**
 * @file C Thread functions
 * C thread functions for hard86
 * Stephen Zhang, 2014
 */

#include <Windows.h>
#include "thread.h"

#include "../../../../global/defines.h"
#include "../../../../global/typedefs.h"

int thread_create(LPTHREAD_START_ROUTINE proc,
                    void* param,
                    THREAD* thread)
{
    thread->handle=CreateThread(NULL,
							   0,
							   proc,
							   param,
							   CREATE_SUSPENDED,
							   &thread->id);
    if(thread->handle){
        return TRUE;
    }
    return FALSE;
}

int thread_start(THREAD* thread)
{
    return (ResumeThread(thread->handle)==-1) ? FALSE : TRUE;
}

int thread_pause(THREAD* thread)
{
    return (SuspendThread(thread->handle)==-1) ? FALSE : TRUE;
}

int thread_kill(THREAD* thread)
{
	/*	fail-safe killing of thread
		this is also dangerous, as any handles held
		by the thread specified will not be released.
		In the case of mutexes, this could result in
		a crash
	*/
	return TerminateThread(thread->handle, 0);
}

int thread_close(THREAD* thread)
{
	WaitForSingleObject(thread->handle, -1);
    return CloseHandle(thread->handle);
}
