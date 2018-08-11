#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include <functional>
#include <string>
#include "atomic.h"

class Thread
{
public:
	typedef std::function<void ()> ThreadFunc;

	explicit Thread(ThreadFunc func, const std::string& name = std::string());
	~Thread();

	void Start();
	int Join();

	bool IsStarted() const { return is_started_; }
	pid_t GetTid() const { return tid_; }
	const std::string& name() const { return name_; }

	static int GetNumCreated() { return num_created_.Get(); }

private:
	static void* StartThread(void* obj);
	void RunInThread();

	bool is_started_;
	bool is_joined_;
	pthread_t pthread_id_;
	pid_t tid_;
	ThreadFunc thread_func_;
	std::string name_;

	static Atomic<int64_t> num_created_;
};

#endif // THREAD_H