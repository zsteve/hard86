/**
* @file C++ Interface for Emulator
* Stephen Zhang, 2014
*/

#include "emulator_cpp.h"

#include "../emulator_engine.h"
#include "../emulator_engine_interface.h"

namespace nsEmulatorComponent{
namespace nsEmulator{

	Emulator* Emulator::m_instance=NULL;

	Emulator* Emulator::GetInstance(){
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

		system_init((MUTEX*)m_sysMutex.GetHandle(), m_bpHitFunc, m_preExFunc, m_posExFunc);
		system_load_bios(sysBios, sysBiosSize);
		system_load_mem(m_sysMem, m_sysMemSize);
	}

	void Emulator::Reset(){
		system_destroy();
		system_init((MUTEX*)m_sysMutex.GetHandle(), m_bpHitFunc, m_preExFunc, m_posExFunc);
		system_load_mem(m_sysMem, m_sysMemSize);
	}
}

}
