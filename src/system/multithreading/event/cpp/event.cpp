/**
 * @file C++ event functions
 * C++ event functions for hard86
 * Stephen Zhang, 2014
 */

#include <windows.h>
#include "event.h"

Event::Event(bool initialState){
	m_nInstances=new int;
	*m_nInstances=1;
	m_hEvent=CreateEvent(NULL, FALSE, (BOOL)initialState, NULL);
}

Event::Event(bool initialState, bool manualReset){
	m_nInstances=new int;
	*m_nInstances=1;
	m_hEvent=CreateEvent(NULL, (BOOL)manualReset, (BOOL)initialState, NULL);
}

Event::Event(const Event& src){
	m_nInstances=src.m_nInstances;
	*m_nInstances++;
	m_hEvent=src.m_hEvent;
}

Event::~Event(){
	*m_nInstances--;
	if(!*m_nInstances){
		delete m_nInstances;
		CloseHandle(m_hEvent);
	}
}

Event& Event::operator=(const Event& src){
	*m_nInstances--;
	if(!*m_nInstances){
		delete m_nInstances;
		CloseHandle(m_hEvent);
	}
	return *this;
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
