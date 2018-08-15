#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <vector>
#include <memory>
#include "currentthread.h"
#include "mutex.h"

class EventItem;
class Epoller;
class TimerQueue;
class Timestamp;

class EventLoop
{
public:
	typedef std::function<void()> TimerCallback;
	typedef std::function<void()> Functor;

	EventLoop();
	~EventLoop();

	void Loop();
	void Quit() { quit_ = true; }
	void UpdateEventItem(EventItem* eventitem);

	bool IsInBirthThread() const { return thread_id_ == GetTid(); }
	void AssertInBirthThread();

	void RunAt(const Timestamp& timestamp, const TimerCallback& cb);
	void RunAfter(double delay, const TimerCallback& cb);
	void RunEvery(double interval, const TimerCallback& cb);

	void RunInLoop(Functor func);
	void QueueInLoop(Functor func);
	void Wakeup();
private:
	void HandleWakeup();
	void DoFunctorQueue();

	typedef std::vector<EventItem*> EventItemList;

	bool quit_;
	const pid_t thread_id_;
	EventItemList active_eventitems_;
	std::unique_ptr<Epoller> epoller_;
	std::unique_ptr<TimerQueue> timer_queue_;

	int wakeup_fd_;
	bool is_handling_functor_queue_;
	std::unique_ptr<EventItem> wakeup_eventitem_;
	MutexLock mutex_;
	std::vector<Functor> functor_queue_;
};

#endif // EVENTLOOP_H
