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

#include <stdexcept>

#include "emulator_cpp.h"

#include "../emulator_engine.h"
#include "../emulator_engine_interface.h"

namespace nsEmulator{

	Emulator* Emulator::m_instance=NULL;

	Emulator* Emulator::GetInstance(){
		if(!m_instance){
			throw std::runtime_error("Emulator::GetInstance() - NULL Instance");
		}
		return m_instance;
	}

	Emulator* Emulator::GetInstance(Mutex sysMutex,
		DBGCALLBACK bpHitFunc,
		DBGCALLBACK preExFunc,
		DBGCALLBACK posExFunc,
		uint8* sysMem,
		uint32 sysMemSize,
		uint8* sysBios,
		uint16 sysBiosSize){

		if(!m_instance){
			m_instance=new Emulator(sysMutex, bpHitFunc, preExFunc, posExFunc, sysMem, sysMemSize, sysBios, sysBiosSize);
		}
		return m_instance;
	}

	void Emulator::DisposeInstance(){
		if(m_instance)
			delete m_instance;
		m_instance=NULL;
	}

	Emulator::Emulator(Mutex sysMutex,
		DBGCALLBACK bpHitFunc,
		DBGCALLBACK preExFunc,
		DBGCALLBACK posExFunc,
		uint8* sysMem,
		uint32 sysMemSize,
		uint8* sysBios,
		uint16 sysBiosSize){

		m_sysMutex=sysMutex;
		m_bpHitFunc=bpHitFunc;
		m_preExFunc=preExFunc;
		m_posExFunc=posExFunc;
		m_sysMem=sysMem;
		m_sysMemSize=sysMemSize;

		m_sysMem=new uint8[sysMemSize];
		memcpy(m_sysMem, sysMem, sysMemSize);
		m_sysMemSize=sysMemSize;

		m_sysBios=new uint8[sysBiosSize];
		memcpy(m_sysBios, sysBios, sysBiosSize);
		m_sysBiosSize=sysBiosSize;

		SetStepThroughExternInt(false);

		system_init((MUTEX*)m_sysMutex.GetHandle(), m_bpHitFunc, m_preExFunc, m_posExFunc);
		system_load_bios(sysBios, sysBiosSize);
		system_load_mem(m_sysMem, m_sysMemSize);
	}

	uint16 Emulator::ReadMem16(uint32 addr){ return read_mem_16(addr); }

	uint8 Emulator::ReadMem8(uint32 addr){ return read_mem_8(addr); }

	void Emulator::WriteMem16(uint16 val, uint32 addr) { write_mem_16(val, addr); }

	void Emulator::WriteMem8(uint8 val, uint32 addr) { write_mem_8(val, addr); }

	void Emulator::WriteIOPort(uint8 val, uint16 port){ write_io_port(val, port); }

	uint8 Emulator::ReadIOPort(uint16 port){ return read_io_port(port); }

	void Emulator::WriteReg(int reg, uint16 val){ write_reg(reg, val); }

	uint16 Emulator::ReadReg(int reg){ return read_reg(reg); }

	void Emulator::StackPush(uint16 val){ stack_push(val); }

	uint16 Emulator::StackPop(){ return stack_pop(); }

	void Emulator::MakeExternInt(uint8 inum){ extern_int(inum); }
	void Emulator::SetStepThroughExternInt(bool v){ set_step_through_extern_int((int)v); }

	void Emulator::SetSysMutex(Mutex& sysMutex){ set_sys_mutex(sysMutex.GetHandle()); }

	int Emulator::Execute(){ return system_execute(); }

	void Emulator::Stop(){
		system_halt();
	}

	void Emulator::Reset(){
		system_destroy();
		system_init((MUTEX*)m_sysMutex.GetHandle(), m_bpHitFunc, m_preExFunc, m_posExFunc);
		system_load_bios(m_sysBios, m_sysBiosSize);
		system_load_mem(m_sysMem, m_sysMemSize);
	}


}
