#ifndef THREAD_H
#define THREAD_H

#include <Windows.h>

typedef unsigned long dword;

class Thread{
public:
	Thread(LPTHREAD_START_ROUTINE threadProc, dword threadParam, int stackSize=NULL);
	virtual ~Thread();

	bool Start();
private:
protected:
	HANDLE m_hThread;
	DWORD m_threadID;
};

#endif