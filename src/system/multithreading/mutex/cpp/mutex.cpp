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

Mutex::Mutex(){
	m_hMutex=CreateMutex(NULL, FALSE, NULL);
}

Mutex::Mutex(const Mutex& src){
	m_hMutex=src.m_hMutex;
}

Mutex::Mutex(void* hMutex){
	m_hMutex=hMutex;
}

Mutex::~Mutex(){
	CloseHandle(m_hMutex);
}

void Mutex::Lock(){
	WaitForSingleObject(m_hMutex, INFINITE);
}

bool Mutex::TryLock(){
	DWORD dwReturn=WaitForSingleObject(m_hMutex, 0);
	if(dwReturn==WAIT_TIMEOUT){
		return false;
	}
	return true;
}

void Mutex::Unlock(){
	ReleaseMutex(m_hMutex);
}
