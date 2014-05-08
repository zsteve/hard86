/*  Hard86 - An 8086 Emulator with support for virtual hardware
	
    Copyright (C) 2014 Stephen Zhang

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.	
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
