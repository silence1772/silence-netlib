#ifndef CONDITION_H
#define CONDITION_H

#include <pthread.h>
#include "mutex.h"

class Condition
{
public:
	explicit Condition(MutexLock& mutex) : mutex_(mutex)
	{
		pthread_cond_init(&pcon_, NULL);
	}
	
	~Condition()
	{
		pthread_cond_destroy(&pcon_);
	}
	
	void Wait()
	{
		pthread_cond_wait(&pcon_, mutex_.GetPthreadMutex());
	}
	
	void Notify()
	{
		pthread_cond_signal(&pcon_);
	}
	
	void NotifyAll()
	{
		pthread_cond_broadcast(&pcon_);
	}
	
private:
	MutexLock& mutex_;
	pthread_cond_t pcon_;
};

#endif // CONDITION_H