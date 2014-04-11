#include "file.h"
#include <Windows.h>
#include <conio.h>
#include <iostream>

using namespace std;

using namespace nsObjWin32::nsFiles;

int main(){

	File myFile(L"C:\\test\\test.txt");
	wcout << "File : " << myFile.GetPath() << " was loaded" << endl;
	wcout << "File size : " << myFile.Size() << endl;
	myFile.Open();
	myFile.Copy(L"C:\\test\\test2.txt");
	myFile.Write(L"This is UNICODE text");
	wcout << myFile.GetPointer() << endl;
	myFile.SetPointer(0);
	wcout << myFile.GetPointer() << endl;
	myFile.SetPointer(1024);
	myFile.SetEOF();
	_getch();
	return 0;
}