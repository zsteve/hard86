/**
 * @file C++ MUTEX functions
 * C++ MUTEX functions for hard86
 * Stephen Zhang, 2014
 */

#include <Windows.h>
#include "mutex.h"

/*
	quick note :
	when MUTEX is signaled -> free
	when object is not signaled -> owned
*/

Mutex::Mutex() : m_state(0){
	m_nInstances=new int;
	(*m_nInstances)=1;
	m_hMutex=CreateMutex(NULL, FALSE, NULL);
}

Mutex::Mutex(const Mutex& src) : m_state(0){
	m_nInstances=src.m_nInstances;
	(*m_nInstances)++;
	m_hMutex=src.m_hMutex;
}

Mutex::Mutex(void* hMutex) : m_state(0){
	m_nInstances=new int;
	// since we are initializing from a raw handle
	// we don't know the lifetime of that handle.
	// therefore, in order to prevent usage of a released handle,
	// we will not attempt to use instance tracking or
	// releasing in the destructor.
	(*m_nInstances)=-1;
	m_hMutex=hMutex;
}

Mutex::~Mutex(){
	if((*m_nInstances)==-1){
		delete m_nInstances;
		return;
	}
	(*m_nInstances)--;
	if(!*m_nInstances){
		delete m_nInstances;
		m_nInstances=NULL;
		CloseHandle(m_hMutex);
	}
}

Mutex& Mutex::operator=(const Mutex& src){
	m_nInstances=src.m_nInstances;
	(*m_nInstances)++;
	m_hMutex=src.m_hMutex;
	return *this;
}

int Mutex::Lock(){
	int retv=WaitForSingleObject(m_hMutex, INFINITE);
	m_state++;
	return retv;
}

bool Mutex::TryLock(){
	DWORD dwReturn=WaitForSingleObject(m_hMutex, 0);
	if(dwReturn==WAIT_TIMEOUT){
		return false;
	}
	m_state++;
	return true;
}

int Mutex::Unlock(){
	int retv=ReleaseMutex(m_hMutex);
	m_state--;
	return retv;
}
