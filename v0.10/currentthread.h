#ifndef CURRENT_THREAD_H
#define CURRENT_THREAD_H

extern __thread int t_cache_tid;
extern __thread char t_tid_string[32];
extern __thread int t_tid_string_length;
extern __thread const char* t_thread_name;

void CacheTid();

inline int GetTid()
{
	// Branch Predictions
	if (__builtin_expect(t_cache_tid == 0, 0))
	{
		CacheTid();
	}
	return t_cache_tid;
}

inline const char* GetTidString() { return t_tid_string; }

inline int GetTidStringLength() { return t_tid_string_length; }

inline const char* GetName() { return t_thread_name; }

#endif // CURRENT_THREAD_H