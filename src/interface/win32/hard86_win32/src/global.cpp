#include "global.h"

#include <Windows.h>
#include <iostream>
#include <fstream>

using namespace std;

namespace nsHard86Win32{

	/**
	 * Write to debug
	 */
	void WriteDebug(const wchar_t* message, const wchar_t* func, const wchar_t* file, int line){
		wfstream f("debug.log", ios_base::app);
		f << message << L" at " << func << L", File: " << file << L", Line: " << line << endl;
	}

}