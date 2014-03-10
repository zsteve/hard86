/**
 * @file C event functions
 * C event functions for hard86
 * Stephen Zhang, 2014
 */

#include <windows.h>
#include "event.h"

#include "../../../../global/defines.h"
#include "../../../../global/typedefs.h"

EVENT event_create(int initial_state, int manual_reset){
	return (EVENT)CreateEvent(NULL, (BOOL)manual_reset, (BOOL)initial_state, NULL);
}

int event_set(EVENT event){
	return SetEvent((HANDLE)event);
}

int event_reset(EVENT event){
	return ResetEvent((HANDLE)event);
}

int event_waitfor(EVENT event){
	return WaitForSingleObject((HANDLE)event, -1);
}

int event_try_waitfor(EVENT event){
	return (WaitForSingleObject((HANDLE)event, 0)==WAIT_TIMEOUT) ? 0 : 1;
}

int event_close(EVENT event){
	return CloseHandle((HANDLE)event);
}
