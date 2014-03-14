/**
 * @file Emulator engine interface
 * x86 emulator engine for hard86
 * Stephen Zhang, 2014
 */

#ifndef EMULATOR_ENGINE_INTERFACE_H
#define EMULATOR_ENGINE_INTERFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <wchar.h>

#include "emulator_engine.h"
#include "../../../global/defines.h"
#include "../../../global/typedefs.h"

/* system multithreading */
#include "../../../system/multithreading/event/c/event.h"
#include "../../../system/multithreading/mutex/c/mutex.h"
#include "../../../system/multithreading/thread/c/thread.h"

int system_init(MUTEX sys_mutex_);
int system_destroy();
int system_execute();

#endif