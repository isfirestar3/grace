
#include "driver_base.h"


driver_base::driver_base()
{
}


driver_base::~driver_base()
{
}

uint64_t driver_base::get_timestamp()
{
#if _WIN32
	LARGE_INTEGER counter;
	static LARGE_INTEGER frequency = { 0 };
	if (0 == frequency.QuadPart) {
		if (!QueryPerformanceFrequency(&frequency)) {
			return 0;
		}
	}

	if (QueryPerformanceCounter(&counter)) {
		return (1000 * counter.QuadPart / frequency.QuadPart);
	}
	return 0;
#else
        // gcc -lrt
        struct timespec tsc;
	if (clock_gettime(CLOCK_MONOTONIC, &tsc) >= 0) { // CLOCK_REALTIME CLOCK_MONOTONIC
		return tsc.tv_sec * 1000 + tsc.tv_nsec / 1000 / 1000; // 
	}
	return 0;
#endif
}

int driver_base::set_driver_callback(void(*driver_call_back)(void *functional_object))
{
	driver_call_back_ = driver_call_back;
	return 0;
}

void driver_base::callback_data(void* v)
{
	if (driver_call_back_)
	{
		driver_call_back_(v);
	}
}
