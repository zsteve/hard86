#include <iostream>
#include <cstdlib>

#include "../src/sym_loader.h"
#include <conio.h>

using namespace std;

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
	FILE* f=fopen("intest.fas", "r");
	int size=fsize(f);
	uint8* data=(uint8*)malloc(size);
	fread(data, 1, size, f);
	fclose(f);

	SymbolData sym(data, size);

	for(int i=0; i<sym.size(); i++){
		cout << sym[i].Name() << " Size : " << sym[i].Size() << endl;
	}

	_getch();
	return 0;
}