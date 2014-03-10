/**
 * @file C++ event functions
 * C++ event functions for hard86
 * Stephen Zhang, 2014
 */

#include <windows.h>
#include "event.h"

Event::Event(bool initialState){
	m_hEvent=CreateEvent(NULL, FALSE, (BOOL)initialState, NULL);
}

Event::Event(bool initialState, bool manualReset){
	m_hEvent=CreateEvent(NULL, (BOOL)manualReset, (BOOL)initialState, NULL);
}

Event::~Event(){
	CloseHandle(m_hEvent);
}

bool Event::Set(){
	return SetEvent(m_hEvent);
}

bool Event::Reset(){
	return ResetEvent(m_hEvent);
}

bool Event::WaitFor(){
	return WaitForSingleObject(m_hEvent, -1);
}

bool Event::TryWaitFor(){
	return (WaitForSingleObject(m_hEvent, 0)==WAIT_TIMEOUT) ? false : true;
}
