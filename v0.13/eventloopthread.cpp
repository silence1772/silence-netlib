#include "eventloopthread.h"
#include "eventloop.h"

EventLoopThread::EventLoopThread()
	: loop_(NULL),
	  is_exiting_(false),
	  thread_(std::bind(&EventLoopThread::ThreadFunc, this)),
	  mutex_(),
	  cond_(mutex_) {}

EventLoopThread::~EventLoopThread()
{
	is_exiting_ = true;
	loop_->Quit();
	thread_.Join();
}

EventLoop* EventLoopThread::StartLoop()
{
	thread_.Start();
	{
		MutexLockGuard lock(mutex_);
		while (loop_ == NULL)
		{
			cond_.Wait();
		}
	}
	return loop_;
}

void EventLoopThread::ThreadFunc()
{
	EventLoop loop;
	{
		MutexLockGuard lock(mutex_);
		loop_ = &loop;
		cond_.Notify();
	}
	loop.Loop();
}