#include <iostream>

#include "debugger.h"

#include "../../emulator_engine/src/emulator_engine_interface.h"
#include "../../emulator_engine/src/dasm/dasm.h"
#include "../../emulator_engine/src/dasm/dasm_process.h"
#include "../../../system/datastruct/clist/clist.h"
#include "../../../global/defines.h"
#include "../../../global/typedefs.h"
#include "../../../system/multithreading/mutex/cpp/mutex.h"

#include "device_test/device_test.h"

using namespace std;

using namespace nsDebugger;
using namespace nsVDev;
using namespace nsDasmProcess;

extern "C"{
	extern sys_state_type sys_state;
}

long fsize(FILE *stream)
{
	long curpos, length;
	curpos=ftell(stream);
	fseek(stream, 0L, SEEK_END);
	length=ftell(stream);
	fseek(stream, curpos, SEEK_SET);
	return length;
}

int main(){
	Mutex sysMutex;
	VDevList* vdevList=VDevList::GetInstance();
	VDev testDev(nsDeviceTest::VirtualDevice_Initialize,
		nsDeviceTest::VirtualDevice_Terminate,
		nsDeviceTest::VirtualDevice_AcceptEmulationMutex,
		(void*)22, (void*)24);
	vdevList->Add(testDev);
	Debugger::Init(vdevList);
	Debugger* dbg=Debugger::GetInstance();
	FILE* in_file=fopen("test/test.com", "r");
	int size=fsize(in_file);
	uint8* data=(uint8*)malloc(size);
	fread(data, 1, size, in_file);
	fclose(in_file);

	sysMutex.Unlock();

	system_init(sysMutex.GetHandle(), &Debugger::BreakPointHit, &Debugger::PreInstructionExecute, &Debugger::PostInstructionExecute);
	system_load_mem(data, size);

	clist c_dasm_list;
	DasmList::ProcessDisassembly(c_dasm_list=dasm_disassemble(100, sys_state.ip, sys_state.cs));
	clist_destroy(&c_dasm_list);

	dbg->AddBreakpoint(GET_ADDR(0x100, 0x700));
	dbg->ActivateBreakpoint(GET_ADDR(0x100, 0x700));

	BreakpointList::iterator it=dbg->BreakpointBegin();
	cout << hex << it->first << endl;
	it++;

	system_execute();

	system_destroy();

	free(data);

	return 0;
}