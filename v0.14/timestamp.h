#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <stdint.h>
#include <time.h>
#include <string>

class Timestamp
{
public:
	Timestamp() : micro_second_timestamp_(0) {}	
	explicit Timestamp(int64_t micro_second_timestamp);

	std::string ToString() const;
	std::string ToFormattedString() const;

	int64_t GetMicrosecondTimestamp() const 
	{
		return micro_second_timestamp_; 
	}
	
	time_t GetSecondTimestamp() const
	{ 
		return static_cast<time_t>(micro_second_timestamp_ / kMicroSecondsPerSecond); 
	}

	void UpdateTimestamp(double interval)
	{
		micro_second_timestamp_ += static_cast<int64_t>(interval);
	}

	static Timestamp GetNow();
	static Timestamp GetAfter(int64_t delay);
	
	static const int kMicroSecondsPerSecond = 1000 * 1000;
		
public:
	inline friend bool operator<(Timestamp lhs, Timestamp rhs)
	{
		return lhs.GetMicrosecondTimestamp() < rhs.GetMicrosecondTimestamp();
	}

	inline friend bool operator>(Timestamp lhs, Timestamp rhs)
	{
		return lhs.GetMicrosecondTimestamp() > rhs.GetMicrosecondTimestamp();
	}

	inline friend bool operator==(Timestamp lhs, Timestamp rhs)
	{
		return lhs.GetMicrosecondTimestamp() == rhs.GetMicrosecondTimestamp();
	}
		
private:
	int64_t micro_second_timestamp_;
};

#endif // TIMESTAMP_H