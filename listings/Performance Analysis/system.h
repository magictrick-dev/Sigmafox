#pragma once

size_t system_timestamp_frequency();
size_t system_timestamp();
size_t system_cpustamp();

#if defined(_WIN32)
#include <windows.h>
#include <intrin.h>

inline size_t 
system_timestamp_frequency()
{
	LARGE_INTEGER frequency = {};
	QueryPerformanceFrequency(&frequency);
	return frequency.QuadPart;
}

inline size_t
system_timestamp()
{
	LARGE_INTEGER value = {};
	QueryPerformanceCounter(&value);
	return value.QuadPart;
}

inline size_t
system_cpustamp()
{
    return __rdtsc();
}

#endif

