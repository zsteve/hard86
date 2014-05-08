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

#include "../../debugger_engine/src/debugger.h"
#include "../../debugger_engine/src/device_test/device_test.h"

#include "../../emulator_engine/src/cpp_interface/emulator_cpp.h"
#include "../../emulator_engine/src/dasm/dasm_cpp.h"
#include "../../symbols_loader/src/sym_loader.h"
#include "../../../global/defines.h"
#include "../../../global/typedefs.h"
#include "../../../system/multithreading/mutex/cpp/mutex.h"

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
	Debugger::Init(vdevList, Event());
	Debugger* dbg=Debugger::GetInstance();
	FILE* in_file=fopen("test/test.com", "r");
	int size=fsize(in_file);
	uint8* data=(uint8*)malloc(size);
	fread(data, 1, size, in_file);
	fclose(in_file);

	// load FAS symbols file
	/*uint8* fas_data;
	int fas_size;
	{
		FILE* fas_file=fopen("test/test.fas", "r");
		fas_size=fsize(fas_file);
		fas_data=(uint8*)malloc(fas_size);
		fread(fas_data, fas_size, 1, fas_file);
		fclose(fas_file);
	}

	SymbolData symbolData(fas_data, fas_size);*/
	SymbolData symbolData;

	sysMutex.Unlock();

	// load BIOS
	uint8* bios_data;
	int bios_size;
	{
		FILE* bios_file=fopen("bios/h86bios.bin", "r");
		bios_size=fsize(bios_file);
		bios_data=(uint8*)malloc(bios_size);
		fread(bios_data, bios_size, 1, bios_file);
		fclose(bios_file);
	}

	Emulator& emulator=*Emulator::GetInstance(sysMutex,
		Debugger::BreakPointHit,
		Debugger::PreInstructionExecute,
		Debugger::PostInstructionExecute,
		data,
		size,
		bios_data,
		bios_size);


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