/**
 * @file C mutex functions
 * C mutex functions for hard86
 * Stephen Zhang, 2014
 */

#ifndef mutex_C_H
#define mutex_C_H

#ifdef __cplusplus
extern "C"{
#endif

#include "../../../../global/defines.h"
#include "../../../../global/typedefs.h"

typedef void* MUTEX;

MUTEX mutex_create();

int mutex_lock(MUTEX mutex);

int mutex_trylock(MUTEX mutex);

int mutex_unlock(MUTEX mutex);

int mutex_close(MUTEX mutex);

#ifdef __cplusplus
}
#endif

#endif
