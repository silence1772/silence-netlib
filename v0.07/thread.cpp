#include "thread.h"
#include <pthread.h>
#include "currentthread.h"
#include "log/logger.h"

Atomic<int64_t> Thread::num_created_;

Thread::Thread(ThreadFunc func, const std::string& name)
	: is_started_(false),
	  is_joined_(false),
	  pthread_id_(0),
	  tid_(0),
	  thread_func_(std::move(func)),
	  name_(name)
{
	int num = num_created_.IncreaseAndGet();
	if (name_.empty())
	{
		char buf[32];
		snprintf(buf, sizeof buf, "Thread%d", num);
		name_ = buf;
	}
}

Thread::~Thread()
{
	if (is_started_ && !is_joined_)
	{
		pthread_detach(pthread_id_);
	}
}

void* Thread::StartThread(void* obj)
{
	Thread* thread = static_cast<Thread*>(obj);
	thread->RunInThread();
	return NULL;
}
 
void Thread::RunInThread()
{
	tid_ = GetTid();
	t_thread_name = name_.empty() ? "defaultThread" : name_.c_str();
	try
	{
		thread_func_();
	}
	catch(...)
	{
		throw std::exception();
		abort();
	}
}

void Thread::Start()
{
	is_started_ = true;
	if (pthread_create(&pthread_id_, NULL, StartThread, this))
	{
		is_started_ = false;
		LOG_SYSFATAL << "failed in pthread_create";
	}
}

int Thread::Join()
{
	is_joined_ = true;
	return pthread_join(pthread_id_, NULL);
}