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
}

Thread::~Thread(){
	TerminateThread(m_hThread, NULL);
}

bool Thread::Start(){
	return ResumeThread(m_hThread);
}