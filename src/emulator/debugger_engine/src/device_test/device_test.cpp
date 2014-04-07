/**
* @file virtual device test
* Stephen Zhang, 2014
*/

#include <iostream>
#include <conio.h>

#include "../../../emulator_engine/src/emulator_engine_interface.h"
#include "../../../../system/multithreading/mutex/c/cmutex.h"

using namespace std;

namespace nsDeviceTest{

	int VirtualDevice_Initialize(void* param1, void* param2){
		cout << "VirtualDeviceTest initialized" << endl;
		cout << "Param 1 : " << (int)param1 << " Param 2 : " << (int)param2 << endl;
		return 0;
	}

	int VirtualDevice_Terminate(){
		return 0;
	}

	int VirtualDevice_AcceptEmulationMutex(MUTEX sysMutex, sys_state_ptr sysState){
		mutex_lock(sysMutex);
		cout << "[VDEVTEST]-------------------------------------------------------" << endl;
		cout << "VirtualDeviceTest now owns sysMutex" << endl;
		cout << "Test : LOBYTE of AX " << hex << (int)(sysState->ax[0]) << endl;
		cout << "[VDEVTEST]-------------------------------------------------------" << endl;
		mutex_unlock(sysMutex);
		return 0;
	}

}