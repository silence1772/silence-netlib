#ifndef EVENTLOOP_THREAD_H
#define EVENTLOOP_THREAD_H

#include "condition.h"
#include "mutex.h"
#include "thread.h"

class EventLoop;

class EventLoopThread
{
public:
	EventLoopThread();
	~EventLoopThread();

	EventLoop* StartLoop();

private:
	void ThreadFunc();

	EventLoop* loop_;
	bool is_exiting_;
	Thread thread_;
	MutexLock mutex_;
	Condition cond_;
};

#endif // EVENTLOOP_THREAD_H