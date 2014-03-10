/**
 * @file C event functions
 * C event functions for hard86
 * Stephen Zhang, 2014
 */

#ifndef EVENT_C_H
#define EVENT_C_H

#include <windows.h>
#include "../../../../global/defines.h"
#include "../../../../global/typedefs.h"

typedef void* EVENT;

EVENT event_create(int initial_state, int manual_reset);
int event_set(EVENT event);
int event_reset(EVENT event);
int event_waitfor(EVENT event);
int event_try_waitfor(EVENT event);
int event_close(EVENT event);

#endif
