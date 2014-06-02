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

#ifndef VDEV_H
#define VDEV_H

#include <vector>
#include <utility>
#include <iterator>
#include <string>

#include "../../emulator_engine/src/emulator_engine_interface.h"
#include "../../../system/multithreading/mutex/c/cmutex.h"

using namespace std;

#define DLLEXPORT __declspec(dllexport)

namespace nsVDev{

	/**
	 * Virtual Device class
	 */
	class DLLEXPORT VDev{
	protected:
		pair<void*, void*> m_params;
		std::wstring m_fileName;

	public:

#ifdef _WIN32
		HMODULE m_hModule;
#endif

		/* Function Pointer Typedefs */
		typedef int(*InitFunc)(void*, void*);
		typedef int(*TermFunc)();
		typedef int(*AcceptMutexFunc)(MUTEX, sys_state_ptr);
#ifdef _WIN32
		VDev(InitFunc initFunc, TermFunc termFunc, AcceptMutexFunc acceptFunc, wstring& fileName, HMODULE hModule, void* param1=NULL, void* param2=NULL);
#else
		VDev(InitFunc initFunc, TermFunc termFunc, AcceptMutexFunc acceptFunc, wstring& fileName, void* param1=NULL, void* param2=NULL);

#endif
		VDev(const VDev& src);
		VDev(){}
		VDev& operator=(const VDev& rhs){
			this->m_acceptMutexFunc=rhs.m_acceptMutexFunc;
			this->m_initFunc=rhs.m_initFunc;
			this->m_termFunc=rhs.m_termFunc;
			this->m_fileName=rhs.m_fileName;
			return *this;
		}

		virtual ~VDev();
		int Initialize(void* param1, void* param2);
		int Initialize();
		int AcceptEmulationMutex(MUTEX emuMutex, sys_state_ptr sysState);
		int Terminate();
		bool Free();
		std::wstring& GetFileName(){ return m_fileName; }

		pair<void*, void*>& GetParams(){
			return m_params;
		}

	private:
	protected:

		InitFunc m_initFunc;
		TermFunc m_termFunc;
		AcceptMutexFunc m_acceptMutexFunc;
	};

	/**
	 * Virtual Device list
	 */
	class DLLEXPORT VDevList{
	private:
		VDevList();

		static VDevList* m_instance;
	public:

#ifdef _WIN32
		
		bool LoadVDevDLL(const string& path);

#endif

		static void InitializeAll(){
			for(iterator it=GetInstance()->begin();
				it!=GetInstance()->end();
				++it){
				(*it).second.Initialize();
			}
		}

		static void TerminateAll(){
			for(iterator it=GetInstance()->begin();
				it!=GetInstance()->end();
				++it){
				(*it).second.Terminate();
			}
		}

		typedef pair<int, VDev> VDevPair;

		static VDevList* GetInstance();

		static bool HasInstance(){ return m_instance ? true : false; }
		static void DisposeInstance(){
			delete m_instance;
			m_instance=NULL;
		}

		virtual ~VDevList();

		int Add(VDev& vdev);
		int Remove(int vdevID);

		// iterator class
		class iterator : \
			public std::iterator<random_access_iterator_tag, pair<int, VDev>, int>
		{
		protected:
			vector<VDevPair>::iterator m_it;
		public:
			iterator(vector<VDevPair>::iterator it){ m_it=it; }
			iterator(const iterator& it){ m_it=it.m_it; }
			iterator& operator++(){ ++m_it; return *this; }
			iterator& operator--(){ --m_it; return *this; }
			bool operator==(const iterator& rhs){ return m_it==rhs.m_it; }
			bool operator!=(const iterator& rhs){ return !(*this==rhs); }
			iterator& operator+(int i){ m_it+=i; return *this; }
			VDevPair& operator*(){ return *m_it; }
		};

		iterator begin(){ return iterator(m_vdevList.begin()); }
		iterator end(){ return iterator(m_vdevList.end()); }
		bool empty(){
			return m_vdevList.empty();
		}
	private:
	protected:
		/*	virtual devices are stored as a vector
			of pair<int, VDev>, where int is the ID,
			and VDev is the device
			*/
		vector<pair<int, VDev> > m_vdevList;

		int m_lastID;
	};

}

#undef DLLEXPORT

#endif
