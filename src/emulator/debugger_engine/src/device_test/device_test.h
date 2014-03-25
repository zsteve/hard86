/**
* @file virtual device test
* Stephen Zhang, 2014
*/

#ifndef DEVICE_TEST_H
#define DEVICE_TEST_H

#include "../../../emulator_engine/src/emulator_engine_interface.h"
#include "../../../../system/multithreading/mutex/c/cmutex.h"

namespace nsDeviceTest{
	int VirtualDevice_Initialize(void* param1, void* param2);
	int VirtualDevice_Terminate();
	int VirtualDevice_AcceptEmulationMutex(MUTEX sysMutex, sys_state_ptr sysState);
}

#endif