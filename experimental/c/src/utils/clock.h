// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef CLOCK_H_
#define CLOCK_H_

#include <time.h>
#include <stdint.h>

#define BILLION 1000000000L
#define NANOS 1000000000LL

typedef struct Clock {
	struct timespec begin;
	struct timespec end;

	long long start;
	long long elapsed;
} Clock;

void GetClockTime(struct timespec* time);
void StartClock(Clock* clock);
void EndClock(Clock* clock);
void SetClockDifference(Clock* clock);
long long GetClockNanosecond(Clock* clock);

#endif // CLOCK_H_
