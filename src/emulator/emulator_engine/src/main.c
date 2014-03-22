/**
 * @file emulator engine unit test
 * Stephen Zhang, 2014
 */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "../../../global/typedefs.h"
#include "../../../system/multithreading/mutex/c/mutex.h"
#include "emulator_engine_interface.h"
#include "dasm/dasm.h"

long fsize(FILE *stream)
{
   long curpos, length;
   curpos = ftell(stream);
   fseek(stream, 0L, SEEK_END);
   length = ftell(stream);
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
	system_init(mutex);
	system_load_mem(data, size);

	disassemble(100, 0);
	system_execute();

	system_destroy();

	free(data);

	_getch();
	return 0;
}