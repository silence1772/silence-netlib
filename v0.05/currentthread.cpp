#include "currentthread.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>

__thread int t_cache_tid = 0;
__thread char t_tid_string[32];
__thread int t_tid_string_length = 6;
__thread const char* t_thread_name = "unknown";

void CacheTid()
{
	if (t_cache_tid == 0)
	{
		t_cache_tid = static_cast<pid_t>(syscall(SYS_gettid));
		t_tid_string_length = snprintf(t_tid_string, sizeof(t_tid_string), "%5d", t_cache_tid);
	}
}
