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

#ifndef HARD86_WIN32_GLOBAL_H
#define HARD86_WIN32_GLOBAL_H

#include <string>

#include "../../../../global/defines.h"
#include "../../../../global/typedefs.h"

namespace nsHard86Win32{

	enum AppWindows{ WND_MAINFRAME };

	void WriteDebug(const wchar_t* message, const wchar_t* func, const wchar_t* file, int line, bool fatal=false);

#define OUT_DEBUG(msg) WriteDebug(L"Error : "L##msg, __FUNCTIONW__, __FILEW__, __LINE__)
#define OUT_DEBUG_FATAL(msg) WriteDebug(L"Error : "L##msg, __FUNCTIONW__, __FILEW__, __LINE__, true)

	/**
	* Convert RGB to COLORREF
	*/
#define RGB2COLORREF(rgb) RGB((rgb & 0xff0000)>>16, (rgb & 0xff00)>>8, (rgb & 0xff))

	/**
	 * Convert wstring to string
	 */
	std::string wstrtostr(const std::wstring& w);

	/**
	 * Convert string to wstring 
	 */
	std::wstring strtowstr(const std::string& s);

	/**
	 * Special window message H86_UPDATE_SYS_DATA
	 * Informs window to update emulated system data
	 * WPARAM - system mutex handle
	 * LPARAM - system state struct pointer
	 */
#define H86_UPDATE_SYS_DATA WM_USER+1

}

#endif