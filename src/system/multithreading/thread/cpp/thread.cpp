/**
 * @file C++ thread functions
 * C++ thread functions for hard86
 * Stephen Zhang, 2014
 */

#include "thread.h"
#include <Windows.h>

Thread::Thread(LPTHREAD_START_ROUTINE threadProc, dword threadParam, int stackSize){
	SECURITY_ATTRIBUTES sa;
	sa.bInheritHandle=false;
	sa.lpSecurityDescriptor=NULL;
	sa.nLength=sizeof(SECURITY_ATTRIBUTES);
	m_hThread=CreateThread(&sa, stackSize,
							threadProc,
							(LPVOID)threadParam,
							CREATE_SUSPENDED,
							&m_threadID);
	m_state=Suspended;
}

Thread::~Thread(){
	WaitForSingleObject(m_hThread, -1);
}

bool Thread::Start(){
	m_state=Running;
	return ResumeThread(m_hThread);
}

bool Thread::Pause(){
	m_state=Suspended;
	return SuspendThread(m_hThread);
}

bool Thread::Kill(){
	m_state=Terminated;
	return TerminateThread(m_hThread, 0);
}
