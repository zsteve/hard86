/**
* @file virtual devices interface
* Stephen Zhang, 2014
*/

#ifndef VDEV_H
#define VDEV_H

#include <vector>
#include <utility>

#include "../../emulator_engine/src/emulator_engine_interface.h"
#include "../../../system/multithreading/mutex/c/mutex.h"

using namespace std;

/**
 * Virtual Device class
 */
class VDev{
public:
	/* Function Pointer Typedefs */
	typedef int(*InitFunc)(void*, void*);
	typedef int(*TermFunc)();
	typedef int(*AcceptMutexFunc)(MUTEX, sys_state_ptr);

	VDev(InitFunc initFunc, TermFunc termFunc, AcceptMutexFunc acceptFunc);
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
	int AcceptEmulationMutex(MUTEX emuMutex, sys_state_ptr sysState);
	int Terminate();

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
public:
	VDevList();
	virtual ~VDevList();
	
	int Add(VDev& vdev);
	int Remove(int vdevID);
	
	// iterator class
	class iterator{
		
	};
private:
protected:
	/*	virtual devices are stored as a vector
		of pair<int, VDev>, where int is the ID,
		and VDev is the device
	*/
	vector<pair<int, VDev> > m_vdevList;
	
	int lastID=0;
};
#endif