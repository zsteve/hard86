#include <Windows.h>
#include "mutex.h"

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
	if(dwReturn==WAIT_ABANDONED ||
		dwReturn==WAIT_TIMEOUT ||
		dwReturn==WAIT_FAILED){
		return false;
	}else{
		return true;
	}
}

void Mutex::Unlock(){
	ReleaseMutex(m_hMutex);
}
