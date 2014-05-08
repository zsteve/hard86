/**
 * @file C++ event functions
 * C++ event functions for hard86
 * Stephen Zhang, 2014
 */

#ifndef EVENT_H
#define EVENT_H

#include <windows.h>

class Event{
	public:
		Event(bool initialState=false);
		Event(bool initialState, bool manualReset);

		Event(const Event& src);

		virtual ~Event();

		Event& operator=(const Event& src);

		bool Set();
		bool Reset();
		bool WaitFor();
		bool TryWaitFor();
	protected:
		HANDLE m_hEvent;

		int* m_nInstances;
	private:
};

#endif
