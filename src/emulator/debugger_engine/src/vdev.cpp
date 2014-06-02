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

#include <algorithm>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <wchar.h>

#include <conio.h>

#include "vdev.h"

#include "debugger.h"
#include "../../emulator_engine/src/emulator_engine_interface.h"
#include "../../../system/multithreading/mutex/c/cmutex.h"

#ifdef _WIN32
#include "../../../interface/win32/hard86_win32/src/global.h"
#endif

using namespace std;

namespace nsVDev{

	// VDev
#ifdef _WIN32
	VDev::VDev(InitFunc initFunc, TermFunc termFunc, AcceptMutexFunc acceptFunc, wstring& fileName, HMODULE hModule, void* param1, void* param2)
#else
	VDev::VDev(InitFunc initFunc, TermFunc termFunc, AcceptMutexFunc acceptFunc, wstring& fileName, void* param1, void* param2)
#endif
	
	{
		m_initFunc=initFunc;
		m_termFunc=termFunc;
		m_acceptMutexFunc=acceptFunc;

		m_params.first=param1;
		m_params.second=param2;

		m_fileName=fileName;

#ifdef _WIN32
		m_hModule=hModule;
#endif
	}

	VDev::VDev(const VDev& src){
		this->m_initFunc=src.m_initFunc;
		this->m_termFunc=src.m_termFunc;
		this->m_acceptMutexFunc=src.m_acceptMutexFunc;
		this->m_params=src.m_params;
		this->m_fileName=src.m_fileName;

#ifdef _WIN32
		m_hModule=src.m_hModule;
#endif
	}

	VDev::~VDev(){
#ifdef _WIN32
		m_hModule=NULL;
#endif
	}

	int VDev::Initialize(void* param1, void* param2){
		return m_initFunc(param1, param2);
	}

	int VDev::Initialize(){
		return m_initFunc(m_params.first, m_params.second);
	}

	int VDev::AcceptEmulationMutex(MUTEX emuMutex, sys_state_ptr sysState){
		return m_acceptMutexFunc(emuMutex, sysState);
	}

	int VDev::Terminate()
	{
#ifdef _WIN32
		int retv=m_termFunc();
		return retv;
#else
		return m_termFunc();
#endif
	}

	bool VDev::Free(){
#ifdef _WIN32
		return (bool)FreeLibrary(m_hModule);
#else
		return false;
#endif
	}

	// VDevList

#ifdef _WIN32
	using namespace nsHard86Win32;

	inline wstring& TidySlashes(wstring& str){
		for(wstring::iterator it=str.begin();
			it!=str.end();
			++it){
			if((*it)==L'/'){
				(*it)=L'\\';
			}
		}
		return str;
	}

	/**
	 * Load DLL as VDev
	 * @return true for success, false for failure
	 */
	bool VDevList::LoadVDevDLL(const string& path){
		HMODULE hMod=LoadLibrary(TidySlashes(strtowstr(path)).c_str());
		if(hMod){
			VDev::InitFunc initFunc=(VDev::InitFunc)GetProcAddress(hMod, "VirtualDevice_Initialize");
			VDev::TermFunc termFunc=(VDev::TermFunc)GetProcAddress(hMod, "VirtualDevice_Terminate");
			VDev::AcceptMutexFunc acceptMutexFunc=(VDev::AcceptMutexFunc)GetProcAddress(hMod, "VirtualDevice_AcceptEmulationMutex");

			if(initFunc && termFunc && acceptMutexFunc){
				this->Add(VDev(initFunc, termFunc, acceptMutexFunc, strtowstr(path), hMod));
				return true;
			}
			else{
				return false;
			}
		}
		else{
			return false;
		}
	}

#endif

	VDevList* VDevList::m_instance=NULL;

	VDevList::VDevList() : m_vdevList(0){
		m_lastID=0;
	}

	VDevList::~VDevList(){
		m_vdevList.clear();
	}

	VDevList* VDevList::GetInstance(){
		if(!m_instance){
			m_instance=new VDevList();
		}
		return m_instance;
	}

	int VDevList::Add(VDev& vdev){
		m_vdevList.push_back(make_pair(m_lastID++, vdev));
		return m_lastID-1;
	}

	int VDevList::Remove(int vdevID){
		for(vector<pair<int, VDev> >::iterator it=m_vdevList.begin();
			it!=m_vdevList.end();
			++it){
			if(it->first==vdevID){
				// found device
				m_vdevList.erase(it);
				return 0;
			}
		}
		return -1;
	}

}
