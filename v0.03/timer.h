#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <functional>
#include "timestamp.h"

class Timer
{
public:
	typedef std::function<void()> TimerCallback;

	Timer(const TimerCallback& cb, Timestamp timestamp, double interval)
	: callback_(cb),
	  expiration_timestamp_(timestamp),
	  interval_(interval),
	  is_repeat_(interval > 0.0) {}
	
	// callback
	void Run() const { callback_(); }
	Timestamp GetExpirationTimestamp() const {return expiration_timestamp_; }
	bool IsRepeat() const { return is_repeat_; }
	void Continue();

private:
	const TimerCallback callback_;
	Timestamp expiration_timestamp_;
	const double interval_;
	const bool is_repeat_;
};

#endif // TIMER_H