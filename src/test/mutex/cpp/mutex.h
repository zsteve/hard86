#ifndef MUTEX_H
#define MUTEX_H

#include <Windows.h>

class Mutex{
public:
	Mutex();
	virtual ~Mutex();

    /**
     * Locks mutex, blocking if necessary
     */
	void Lock();
	/**
	 * Attempts to lock mutex. Does not block.
	 * @return true if locking succeeded, false if locking failed
	 */
	bool TryLock();
	/**
	 * Unlocks mutex
	 */
	void Unlock();

private:
protected:
	HANDLE m_hMutex;
};

#endif
