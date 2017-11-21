#include "clock.h"

void GetClockTime(struct timespec* time){
	clock_gettime(CLOCK_MONOTONIC, time);
}

void StartClock(Clock* clock){
	GetClockTime(&clock->begin);
	clock->start = clock->begin.tv_sec * NANOS + clock->begin.tv_nsec;
}

void EndClock(Clock* clock){
	GetClockTime(&clock->end);
	clock->elapsed = clock->end.tv_sec * NANOS + clock->end.tv_nsec - clock->start;
}

long long GetClockNanosecond(Clock* clock){
	return clock->elapsed;
}
