/**
* @file virtual devices interface
* Stephen Zhang, 2014
*/

#ifndef VDEV_H
#define VDEV_H

#include <vector>
#include <utility>
#include <iterator>

#include "../../emulator_engine/src/emulator_engine_interface.h"
#include "../../../system/multithreading/mutex/c/cmutex.h"

using namespace std;

namespace nsEmulatorComponent{

namespace nsVDev{

	/**
	 * Virtual Device class
	 */
	class VDev{
	protected:
		pair<void*, void*> m_params;
	public:
		/* Function Pointer Typedefs */
		typedef int(*InitFunc)(void*, void*);
		typedef int(*TermFunc)();
		typedef int(*AcceptMutexFunc)(MUTEX, sys_state_ptr);

		VDev(InitFunc initFunc, TermFunc termFunc, AcceptMutexFunc acceptFunc, void* param1=NULL, void* param2=NULL);
		VDev(const VDev& src);
		VDev(){}
		VDev& operator=(const VDev& rhs){
			this->m_acceptMutexFunc=rhs.m_acceptMutexFunc;
			this->m_initFunc=rhs.m_initFunc;
			this->m_termFunc=rhs.m_termFunc;
			return *this;
		}

		virtual ~VDev();
		int Initialize(void* param1, void* param2);
		int Initialize();
		int AcceptEmulationMutex(MUTEX emuMutex, sys_state_ptr sysState);
		int Terminate();

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
	class VDevList{
	private:
		VDevList();

		static VDevList* m_instance;
	public:

		typedef pair<int, VDev> VDevPair;

		static VDevList* GetInstance();

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

}
#endif
