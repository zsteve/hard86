/**
 * @file C++ MUTEX functions
 * C++ MUTEX functions for hard86
 * Stephen Zhang, 2014
 */

#ifndef MUTEX_H
#define MUTEX_H

#include <Windows.h>

class Mutex{
public:
	Mutex();
	Mutex(const Mutex& src);
	Mutex(void* hMutex);

	virtual ~Mutex();

	Mutex& operator=(const Mutex& src);

    /**
     * Locks MUTEX, blocking if necessary
     */
	int Lock();
	/**
	 * Attempts to lock MUTEX. Does not block.
	 * @return true if locking succeeded, false if locking failed
	 */
	bool TryLock();
	/**
	 * Unlocks MUTEX
	 */
	int Unlock();

	void* GetHandle(){ return m_hMutex; }
private:
protected:
	HANDLE m_hMutex;

	int* m_nInstances;
};

#endif
