#ifndef PERF_H__
#define PERF_H__

#ifdef PERF_STATISTICS
#include <sys/time.h>

#define USECS_COUNT    1000000

static inline void perf_tv_start(struct timeval *pretv)
{
	gettimeofday(pretv,NULL);
}

static inline void perf_tv_sample(struct timeval *pretv,struct timeval *sumtv)
{
	long scosts;
	long uscosts;	
	long sum_scosts;
	long sum_uscosts;

	struct timeval current_tv;
	gettimeofday(&current_tv,NULL);

	uscosts = (long)current_tv.tv_usec - pretv->tv_usec;
	scosts = (long)current_tv.tv_sec - pretv->tv_sec;

	if(uscosts <= 0){
		if(scosts <= 0){
			fprintf(stderr,"Time overflow!\n");
			return;
		}
		scosts -= 1;
		uscosts = current_tv.tv_usec + USECS_COUNT;
		uscosts -= pretv->tv_usec;
	}
	
	sum_scosts = scosts + sumtv->tv_sec;
	sum_uscosts = uscosts + sumtv->tv_usec;

	if(sum_uscosts >= USECS_COUNT){
		sum_scosts += sum_uscosts / USECS_COUNT;
		sum_uscosts = sum_uscosts % USECS_COUNT;
	}

	sumtv->tv_usec = sum_uscosts;
	sumtv->tv_sec = sum_scosts;
}

#else
#define perf_tv_start(...)
#define perf_tv_sample(...)
#endif

#endif
