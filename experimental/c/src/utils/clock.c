#include "clock.h"

void GetClockTime(struct timespec* time){
	clock_gettime(CLOCK_MONOTONIC, time);
}

void StartClock(Clock* clock){
	GetClockTime(&clock->start);
}

void EndClock(Clock* clock){
	GetClockTime(&clock->end);
}

void SetClockDifference(Clock* clock){
	clock->diff = BILLION * (clock->end.tv_sec - clock->start.tv_sec) + (clock->end.tv_nsec - clock->start.tv_nsec);
}

long long unsigned int GetClockNanosecond(Clock* clock){
	return (long long unsigned int) clock->diff;
}
