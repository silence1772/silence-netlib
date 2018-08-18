#ifndef EVENTLOOP_THREAD_POOL_H
#define EVENTLOOP_THREAD_POOL_H

#include <vector>
#include <memory>
#include "condition.h"
#include "mutex.h"
#include "thread.h"

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool
{
public:
	EventLoopThreadPool(EventLoop* base_loop);
	~EventLoopThreadPool();

	void SetThreadNum(int thread_num) { thread_num_ = thread_num; }
	void Start();
	
	EventLoop* GetNextLoop();

private:
	EventLoop* base_loop_;
	bool is_started_;
	int thread_num_;
	int next_;
	std::vector<std::unique_ptr<EventLoopThread> > threads_;
	std::vector<EventLoop*> loops_;
};

#endif // EVENTLOOP_THREAD_POOL_H