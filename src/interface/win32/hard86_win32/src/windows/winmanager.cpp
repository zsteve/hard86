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

#include "winmanager.h"
namespace nsHard86Win32{

WinManager::WinManager(const wchar_t* defFontName, int defFontHeight){
	m_defFont=CreateFont(defFontHeight,
		NULL,
		0, 0, FW_DONTCARE, 0, 0, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
		FF_DONTCARE, defFontName);
	if(!m_defFont){
		MessageBox(NULL, L"Error : WinManager::WinManager() - m_defFont==NULL", L"Error", MB_ICONERROR);
	}
}

WinManager::~WinManager(){
	for(map<int, Window*>::iterator it=m_wndMap.begin();
		it!=m_wndMap.end();
		++it){
		delete (*it).second;
	}
	DeleteFont(m_defFont);
}

bool WinManager::SetDefFont(int id){
	if(!Exists(id)) return false;
	m_wndMap[id]->SendMessage(WM_SETFONT, (WPARAM)m_defFont, true);
}

bool WinManager::AddWindow(int id, Window* win){
	if(m_wndMap.count(id)>0) return false;	// cannot replace existing
	m_wndMap[id]=win;
}

bool WinManager::RemoveWindow(int id){
	if(m_wndMap.count(id)) return false;	// non existent
	m_wndMap.erase(id);
	return true;
}

bool WinManager::SetFont(int id, HFONT font){
	if(m_wndMap.count(id)) return false;
	m_wndMap[id]->SendMessage(WM_SETFONT, (WPARAM)font, FALSE);
	return true;
}

}

