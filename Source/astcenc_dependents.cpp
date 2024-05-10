
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

/* See header for documentation */
int get_cpu_count()
{
	return static_cast<int>(sysconf(_SC_NPROCESSORS_ONLN));
}

/* See header for documentation */
double get_time()
{
	timeval tv;
	gettimeofday(&tv, 0);
	return static_cast<double>(tv.tv_sec) + static_cast<double>(tv.tv_usec) * 1.0e-6;
}