#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>
#include <assert.h>
#include <sys/types.h>
#include "currentthread.h"
#include "thread.h"

class MutexLock
{
public:
	MutexLock():holder_(0)
	{
		int ret = pthread_mutex_init(&mutex_, NULL);
		assert(ret == 0); 
		(void) ret;
	}
	
	~MutexLock()
	{
		assert(holder_ == 0);
		int ret = pthread_mutex_destroy(&mutex_);
		assert(ret == 0);
		(void)ret;
	}
	
	bool IsLockedByThisThread()
	{
		return holder_ == GetTid();
	}
	
	void AssertLocked()
	{
		assert(IsLockedByThisThread());
	}
	
	void Lock()
	{
		pthread_mutex_lock(&mutex_);
		holder_ = GetTid();
	}
	
	void Unlock()
	{
		holder_ = 0;
		pthread_mutex_unlock(&mutex_);
	}
	
	pthread_mutex_t* GetPthreadMutex()
	{
		return &mutex_;
	}
	
private:
	pthread_mutex_t mutex_;
	pid_t holder_;
};


//manager the MutexLock object  but not duty lifetime
class MutexLockGuard
{
public:
	explicit MutexLockGuard(MutexLock& mutex):mutex_(mutex)
	{
		mutex_.Lock();
	}
	
	~MutexLockGuard()
	{
		mutex_.Unlock();
	}
	
private:
	MutexLock& mutex_; //just ref
};

//don't allow define a no_name object 
#define MutexLockGuard(x) error "Missing guard object name" 

#endif // MUTEX_H