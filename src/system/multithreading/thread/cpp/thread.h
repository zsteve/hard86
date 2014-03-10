/**
 * @file C++ thread functions
 * C++ thread functions for hard86
 * Stephen Zhang, 2014
 */

#ifndef THREAD_H
#define THREAD_H

#include <Windows.h>

typedef unsigned long dword;

class Thread{
public:
	Thread(LPTHREAD_START_ROUTINE threadProc, dword threadParam, int stackSize=NULL);
	virtual ~Thread();

	bool Start();
	bool Pause();
	bool Kill();
private:
protected:
	HANDLE m_hThread;
	DWORD m_threadID;
};

#endif
