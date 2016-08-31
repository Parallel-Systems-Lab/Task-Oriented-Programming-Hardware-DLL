#include "timing.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

//typedef struct stat stat_t;
//stat_t file_stat;

typedef struct timespec timespec_t;
#define BILLION 1000000000L
//#define BILLION   1L //smaller values
timespec_t tick_time;
unsigned rt_tick()
{
clock_gettime(CLOCK_MONOTONIC, &tick_time);
return 0;
}
unsigned rt_tock()
{
timespec_t tock_time;
long long unsigned int diff; 
clock_gettime(CLOCK_MONOTONIC, &tock_time);
	diff= BILLION * (tock_time.tv_sec - tick_time.tv_sec) + tock_time.tv_nsec - tick_time.tv_nsec;
//	printf("elapsed time = %llu nanoseconds\n", (long long unsigned int) diff);
return (unsigned)diff;
}
int time_test (void)
{
	unsigned long long diff;
	timespec_t start, end;

	/* measure monotonic time */
	clock_gettime(CLOCK_MONOTONIC, &start);	/* mark start time */
	//sleep(1);	/* do stuff */
	clock_gettime(CLOCK_MONOTONIC, &end);	/* mark the end time */

	diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
	printf("elapsed time = %llu nanoseconds\n", (long long unsigned int) diff);

	/* now re-do this and measure CPU time */
	/* the time spent sleeping will not count (but there is a bit of overhead */
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);	/* mark start time */
	//sleep(1);	/* do stuff */
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);		/* mark the end time */

	diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
	printf("elapsed process CPU time = %llu nanoseconds\n", (long long unsigned int) diff);

	return EXIT_SUCCESS;
}
