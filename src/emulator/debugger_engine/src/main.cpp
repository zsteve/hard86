#include <iostream>

#include "debugger.h"

#include "../../emulator_engine/src/cpp_interface/emulator_cpp.h"
#include "../../emulator_engine/src/dasm/dasm_cpp.h"
#include "../../symbols_loader/src/sym_loader.h"
#include "../../../global/defines.h"
#include "../../../global/typedefs.h"
#include "../../../system/multithreading/mutex/cpp/mutex.h"

#include "device_test/device_test.h"

using namespace std;

using namespace nsDebugger;
using namespace nsVDev;
using namespace nsDasm;
using namespace nsEmulator;
using namespace nsSymLoader;

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

	// load FAS symbols file
	uint8* fas_data;
	int fas_size;
	{
		FILE* fas_file=fopen("test/test.fas", "r");
		fas_size=fsize(fas_file);
		fas_data=(uint8*)malloc(fas_size);
		fread(fas_data, 1, fas_size, fas_file);
		fclose(fas_file);
	}

	SymbolData symbolData(fas_data, fas_size);

	sysMutex.Unlock();

	Emulator& emulator=*Emulator::GetInstance(sysMutex,
		Debugger::BreakPointHit,
		Debugger::PreInstructionExecute,
		Debugger::PostInstructionExecute,
		data,
		size);


	Disassembler dasm;
	DasmList l=dasm.Disassemble(100, emulator.ReadReg(Regs::ip), emulator.ReadReg(Regs::cs), symbolData);
	for(DasmList::iterator it=l.begin();
		it!=l.end();
		++it){
		cout << "DASM :: " << hex << it->GetAddr() << " : " << it->GetLine() << endl;
	}

	emulator.Execute();

	emulator.Reset();

	free(data);

	return 0;
}