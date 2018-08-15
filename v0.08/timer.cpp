#include "timer.h"

void Timer::Continue()
{
	if (is_repeat_)
	{
		expiration_timestamp_.UpdateTimestamp(interval_);
	}
}