/**
 * @file C++ mutex functions
 * C++ mutex functions for hard86
 * Stephen Zhang, 2014
 */

#include <Windows.h>
#include "mutex.h"

/*
	quick note :
	when mutex is signaled -> free
	when object is not signaled -> owned
*/

Mutex::Mutex(){
	m_hMutex=CreateMutex(NULL, FALSE, NULL);
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
