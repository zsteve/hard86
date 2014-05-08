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
	// Warning : usage of Kill() is dangerous, as thread will be terminated immediately
	// Any resources allocated by the thread will not be released.
	bool Kill();

	void* GetHandle(){ return m_hThread; }

	enum ThreadState{
		Running, Suspended, Terminated
	};

	ThreadState State(){ return m_state; }
private:
protected:
	HANDLE m_hThread;
	DWORD m_threadID;
	ThreadState m_state;
};

#endif
