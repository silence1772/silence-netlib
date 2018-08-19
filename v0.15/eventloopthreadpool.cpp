#include "eventloopthreadpool.h"
#include "eventloop.h"
#include "eventloopthread.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* base_loop)
	: base_loop_(base_loop),
	  is_started_(false),
	  thread_num_(0),
	  next_(0) {}

EventLoopThreadPool::~EventLoopThreadPool()
{

}

void EventLoopThreadPool::Start()
{
	base_loop_->AssertInBirthThread();
	is_started_ = true;
	for (int i = 0; i < thread_num_; ++i)
	{
		EventLoopThread* t = new EventLoopThread;
		threads_.push_back(std::unique_ptr<EventLoopThread>(t));
		loops_.push_back(t->StartLoop());
	}
}

EventLoop* EventLoopThreadPool::GetNextLoop()
{
	base_loop_->AssertInBirthThread();
	EventLoop* loop = base_loop_;

	if (!loops_.empty())
	{
		loop = loops_[next_];
		++next_;
		if (static_cast<size_t>(next_) >= loops_.size())
		{
			next_ = 0;
		}
	}
	return loop;
}