/**
 * @file C++ thread functions
 * C++ thread functions for hard86
 * Stephen Zhang, 2014
 */

#include "thread.h"
#include <Windows.h>

Thread::Thread(LPTHREAD_START_ROUTINE threadProc, dword threadParam, int stackSize) : m_suspendCount(0), m_maxSuspendCount(-1){
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
	int retv=ResumeThread(m_hThread);
	if(retv==-1) return false;
	m_suspendCount=retv-1;
	return true;
}

bool Thread::Pause(){
	if(m_suspendCount==m_maxSuspendCount) return false;
	m_state=Suspended;
	int retv=SuspendThread(m_hThread);
	if(retv==-1) return false;
	m_suspendCount=retv+1;
	return true;
}

bool Thread::Kill(){
	m_state=Terminated;
	return TerminateThread(m_hThread, 0);
}
