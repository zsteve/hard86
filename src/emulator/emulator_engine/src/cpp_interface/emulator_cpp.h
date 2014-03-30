/**
* @file C++ Interface for Emulator
* Stephen Zhang, 2014
*/

#ifndef EMULATOR_CPP_H
#define EMULATOR_CPP_H

#include "../../../../global/typedefs.h"
#include "../../../../global/defines.h"
#include "../../../../system/datastruct/clist/clist.h"
#include "../../../../system/multithreading/mutex/c/cmutex.h"
#include "../../../../system/multithreading/mutex/cpp/mutex.h"

namespace nsEmulator{

	/**
	 * Singleton Emulator class
	 * interface to the C component
	 */
	class Emulator{
	private:
		Emulator(Mutex sysMutex,
			DBGCALLBACK bpHitFunc,
			DBGCALLBACK preExFunc,
			DBGCALLBACK posExFunc,
			uint8* sysMem,
			uint32 sysMemSize);

		static Emulator* m_instance;
	public:
		static Emulator* GetInstance(Mutex sysMutex,
									DBGCALLBACK bpHitFunc,
									DBGCALLBACK preExFunc,
									DBGCALLBACK posExFunc,
									uint8* sysMem,
									uint32 sysMemSize);

		static Emulator* GetInstance();

		void Reset();

		// Instance functions

		uint16 ReadMem16(uint32 addr){ return read_mem_16(addr); }
		uint8 ReadMem8(uint32 addr){ return read_mem_8(addr); }

		void WriteMem16(uint16 val, uint32 addr) { write_mem_16(val, addr); }
		void WriteMem8(uint8 val, uint32 addr) { write_mem_8(val, addr); }

		void WriteIOPort(uint8 val, uint16 port){ write_io_port(val, port); }
		uint8 ReadIOPort(uint16 port){ return read_io_port(port); }

		void WriteReg(int reg, uint16 val){ write_reg(reg, val); }
		uint16 ReadReg(int reg){ return read_reg(reg); }

		void StackPush(uint16 val){ stack_push(val); }
		uint16 StackPop(){ return stack_pop(); }

		int Execute(){ return system_execute(); }
		void Reset();
	protected:
		uint8* m_sysMem;
		uint32 m_sysMemSize;

		Mutex m_sysMutex;
		DBGCALLBACK m_bpHitFunc, m_preExFunc, m_posExFunc;
	private:
	};

}

#endif