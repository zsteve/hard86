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

	std::wstring StringToWString(const std::string& s){
		std::wstring retv;
		retv.resize(s.size());
		for(int i=0; i<s.size(); i++) retv[i]=(wchar_t)s[i];
		return retv;
	}

	std::string WStringToString(const std::wstring& w){
		char* s=new char[w.size()];
		for(int i=0; i<w.size(); i++){
			s[i]=w[i] & 255;
		}
		std::string retv(s);
		delete[] s;
		return s;
	}

}