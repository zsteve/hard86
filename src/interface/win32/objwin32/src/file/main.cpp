#include "file.h"
#include <Windows.h>
#include <conio.h>
#include <iostream>

using namespace std;

using namespace nsObjWin32::nsFiles;

int main(){

	File myFile(L"C:\\test\\test");
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

	Directory myDir(L"G:\\test\\asdf\\asdf");
	list<File> fl(0);
	myDir.List(fl);
	for(list<File>::iterator it=fl.begin();
		it!=fl.end();
		++it){
		wcout << it->GetPath() << endl;
	}

	wcout << FileObject::GetRelativePath(L"C:\\test\\a.txt", L"C:\\test\\") << endl;
	wcout << FileObject::GetRelativePath(L"C:\\test\\a.txt", L"C:\\test") << endl;
	wcout << FileObject::GetAbsolutePath(L"../asdf", L"G:\\asdf\\asdf\\") << endl;
	_getch();
	return 0;
}