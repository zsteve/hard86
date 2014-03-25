/**
* @file virtual devices interface
* Stephen Zhang, 2014
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

using namespace std;

namespace nsVDev{

	// VDev

	VDev::VDev(InitFunc initFunc, TermFunc termFunc, AcceptMutexFunc acceptFunc, void* param1, void* param2){
		m_initFunc=initFunc;
		m_termFunc=termFunc;
		m_acceptMutexFunc=acceptFunc;

		m_params.first=param1;
		m_params.second=param2;
	}

	VDev::VDev(const VDev& src){
		this->m_initFunc=src.m_initFunc;
		this->m_termFunc=src.m_termFunc;
		this->m_acceptMutexFunc=src.m_acceptMutexFunc;
		this->m_params=src.m_params;
	}

	VDev::~VDev(){

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

	int VDev::Terminate(){
		return m_termFunc();
	}

	// VDevList

	VDevList* VDevList::m_instance=NULL;

	VDevList::VDevList() : m_vdevList(0){

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
		m_vdevList.push_back(make_pair(lastID++, vdev));
		return lastID-1;
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
