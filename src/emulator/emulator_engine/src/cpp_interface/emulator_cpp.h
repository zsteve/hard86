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

#ifndef EMULATOR_CPP_H
#define EMULATOR_CPP_H

#include "../emulator_engine_interface.h"

#include "../../../../global/typedefs.h"
#include "../../../../global/defines.h"
#include "../../../../system/datastruct/clist/clist.h"
#include "../../../../system/multithreading/mutex/c/cmutex.h"
#include "../../../../system/multithreading/mutex/cpp/mutex.h"

#define DLLEXPORT __declspec(dllexport)

namespace nsEmulator{

	enum DLLEXPORT Regs{
		ax, cx, dx, bx, sp, bp, si, di, es, cs, ss, ds, ip, flags,
		al, cl, dl, bl, ah, ch, dh, bh
	};

	/**
	 * Singleton Emulator class
	 * interface to the C component
	 */
	class DLLEXPORT Emulator{
	private:
		Emulator(Mutex sysMutex,
			DBGCALLBACK bpHitFunc,
			DBGCALLBACK preExFunc,
			DBGCALLBACK posExFunc,
			uint8* sysMem,
			uint32 sysMemSize,
			uint8* sysBios,
			uint16 sysBiosSize);

		static Emulator* m_instance;
	public:

		static const int MemSize=1048576;

		static Emulator* GetInstance(Mutex sysMutex,
									DBGCALLBACK bpHitFunc,
									DBGCALLBACK preExFunc,
									DBGCALLBACK posExFunc,
									uint8* sysMem,
									uint32 sysMemSize,
									uint8* sysBios,
									uint16 sysBiosSize);

		static Emulator* GetInstance();

		static bool HasInstance(void){ return (bool)m_instance; }

		static void DisposeInstance();

		// Instance functions

		uint16 ReadMem16(uint32 addr);
		uint8 ReadMem8(uint32 addr);

		void WriteMem16(uint16 val, uint32 addr);
		void WriteMem8(uint8 val, uint32 addr);

		void WriteIOPort(uint8 val, uint16 port);
		uint8 ReadIOPort(uint16 port);

		void WriteReg(int reg, uint16 val);
		uint16 ReadReg(int reg);

		void StackPush(uint16 val);
		uint16 StackPop();

		void MakeExternInt(uint8 inum);
		void SetStepThroughExternInt(bool v);

		sys_state_ptr SystemState(){ return get_system_state(); }

		void SetSysMutex(Mutex& sysMutex);

		int Execute();
		void Stop();
		void Reset();

	protected:
		uint8* m_sysMem;
		uint32 m_sysMemSize;

		uint8* m_sysBios;
		uint32 m_sysBiosSize;

		Mutex m_sysMutex;
		DBGCALLBACK m_bpHitFunc, m_preExFunc, m_posExFunc;
	private:
	};

#undef DLLEXPORT

}

#endif