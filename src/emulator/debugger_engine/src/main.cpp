#include <iostream>

#include "debugger.h"

#include "../../emulator_engine/src/emulator_engine_interface.h"
#include "../../emulator_engine/src/dasm/dasm.h"
#include "../../../system/datastruct/clist/clist.h"
#include "../../../global/defines.h"
#include "../../../global/typedefs.h"
#include "../../../system/multithreading/mutex/c/mutex.h"

using namespace std;

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
	MUTEX mutex;
	FILE* in_file=fopen("test/test.com", "r");
	int size=fsize(in_file);
	uint8* data=(uint8*)malloc(size);
	fread(data, 1, size, in_file);
	fclose(in_file);

	mutex=mutex_create();
	mutex_unlock(mutex);	/* make sure it's unlocked */
	system_init(mutex, &BreakPointHit, &PreInstructionExecute, &PostInstructionExecute);
	system_load_mem(data, size);

	system_execute();

	system_destroy();

	free(data);

	return 0;
}