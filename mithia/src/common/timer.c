
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include "timer.h"
#include "malloc.h"
#include "strlib.h"

#define TIMER_MIN_INTERVAL 50
#define TIMER_MAX_INTERVAL 1000
#define cap_value(a, min, max) ((a >= max) ? max : (a <= min) ? min : a)


// timers (array)
static struct TimerData* timer_data = NULL;
static int timer_data_max = 0;
static int timer_data_num = 0;

// free timers (array)
static int* free_timer_list = NULL;
static int free_timer_list_max = 0;
static int free_timer_list_pos = 0;

// timer heap (ordered array of tid's)
static int timer_heap_num = 0;
static int timer_heap_max = 0;
static int* timer_heap = NULL;



/*----------------------------
 * 	Get tick time
 *----------------------------*/
int getDay(void) {
	time_t t;
	t=time(NULL);
	
	if (((t - 18000) % 604800) / 86400 < 4) {
		return ((t - 18000) % 604800) / 86400 + 4;
	} else {
		return ((t - 18000) % 604800) / 86400 - 3;
	}
}
int getHour(void) {
	time_t t;
	struct tm* d;
	t=time(NULL);
	d=localtime(&t);
	return d->tm_hour;
}
int getMinute(void) {
	time_t t;
	struct tm* d;
	t=time(NULL);
	d=localtime(&t);
	return d->tm_min;
}
int getSecond(void) {
	time_t t;
	struct tm* d;
	t=time(NULL);
	d=localtime(&t);
	return d->tm_sec;
}

void Log_Add(const char *name, const char* buf, ... )
{
	FILE *fp;
	StringBuf file;
	StringBuf b;
	va_list args;
	time_t t;
	struct tm* d;
	
	t=time(NULL);
	d=localtime(&t);
	
	StringBuf_Init(&file);
	StringBuf_Printf(&file,"logs/%s-%04d-%02d-%02d.log",name,d->tm_year+1900,d->tm_mon+1,d->tm_mday);
	//printf(StringBuf_Value(&file));
	fp=fopen(StringBuf_Value(&file),"a");
	if(fp==NULL) {
		StringBuf_Destroy(&file);
		printf("Error Creating Log - %s\n",name);
		return 0;
	}
	StringBuf_Destroy(&file);
	StringBuf_Init(&b);
	va_start(args,buf);
	StringBuf_Vprintf(&b,buf,args);
	va_end(args);
	fprintf(fp,StringBuf_Value(&b));
	fprintf(fp,"\n");
	StringBuf_Destroy(&b);
	fclose(fp);
}
/// platform-abstracted tick retrieval

static unsigned int tick(void)
{
#if defined(WIN32)
	return GetTickCount();
#elif (defined(_POSIX_TIMERS) && _POSIX_TIMERS > 0 && defined(_POSIX_MONOTONIC_CLOCK) /* posix compliant */) || (defined(__FreeBSD_cc_version) && __FreeBSD_cc_version >= 500005 /* FreeBSD >= 5.1.0 */)
	struct timespec tval;
	clock_gettime(CLOCK_MONOTONIC, &tval);
	return tval.tv_sec * 1000 + tval.tv_nsec / 1000000;
#else
	struct timeval tval;
	gettimeofday(&tval, NULL);
	return tval.tv_sec * 1000 + tval.tv_usec / 1000;
#endif
}
//////////////////////////////////////////////////////////////////////////
#if defined(TICK_CACHE) && TICK_CACHE > 1
//////////////////////////////////////////////////////////////////////////
// tick is cached for TICK_CACHE calls
static unsigned int gettick_cache;
static int gettick_count = 1;

unsigned int gettick_nocache(void)
{
	gettick_count = TICK_CACHE;
	gettick_cache = tick();
	return gettick_cache;
}

unsigned int gettick(void)
{
	return ( --gettick_count == 0 ) ? gettick_nocache() : gettick_cache;
}
//////////////////////////////
#else
//////////////////////////////
// tick doesn't get cached
unsigned int gettick_nocache(void)
{
	return tick();
}

unsigned int gettick(void)
{
	return tick();
}
//////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////

/*======================================
 * 	CORE : Timer Heap
 *--------------------------------------*/

// searches for the target tick's position and stores it in pos (binary search)
#define HEAP_SEARCH(target,from,to,pos) \
	do { \
		int max,pivot; \
		max = to; \
		pos = from; \
		while (pos < max) { \
			pivot = (pos + max) / 2; \
			if (DIFF_TICK(target, timer_data[timer_heap[pivot]].tick) < 0) \
				pos = pivot + 1; \
			else \
				max = pivot; \
		} \
	} while(0)

/// Adds a timer to the timer_heap
static void push_timer_heap(int tid)
{
	int pos;

	// check available space
	if( timer_heap_num >= timer_heap_max )
	{
		timer_heap_max += 256;
		if( timer_heap )
			REALLOC(timer_heap, int, timer_heap_max);
		else
			CALLOC(timer_heap, int, timer_heap_max);
		memset(timer_heap + (timer_heap_max - 256), 0, sizeof(int)*256);
	}

	// do a sorting from higher to lower
	if( timer_heap_num == 0 || DIFF_TICK(timer_data[tid].tick, timer_data[timer_heap[timer_heap_num - 1]].tick) < 0 )
		timer_heap[timer_heap_num] = tid; // if lower actual item is higher than new
	else
	{
		// searching position
		HEAP_SEARCH(timer_data[tid].tick,0,timer_heap_num-1,pos);
		// move elements
		memmove(&timer_heap[pos + 1], &timer_heap[pos], sizeof(int) * (timer_heap_num - pos));
		// save new element
		timer_heap[pos] = tid;
	}

	timer_heap_num++;
}

/*==========================
 * 	Timer Management
 *--------------------------*/

/// Returns a free timer id.
static int acquire_timer(void)
{
	int tid;

	// select a free timer
	if (free_timer_list_pos) {
		do {
			tid = free_timer_list[--free_timer_list_pos];
		} while(tid >= timer_data_num && free_timer_list_pos > 0);
	} else
		tid = timer_data_num;

	// check available space
	if( tid >= timer_data_num )
		for (tid = timer_data_num; tid < timer_data_max && timer_data[tid].type; tid++);
	if (tid >= timer_data_num && tid >= timer_data_max)
	{// expand timer array
		timer_data_max += 256;
		if( timer_data )
			REALLOC(timer_data, struct TimerData, timer_data_max);
		else
			CALLOC(timer_data, struct TimerData, timer_data_max);
		memset(timer_data + (timer_data_max - 256), 0, sizeof(struct TimerData)*256);
	}

	if( tid >= timer_data_num )
		timer_data_num = tid + 1;

	return tid;
}

/// Starts a new timer that is deleted once it expires (single-use).
/// Returns the timer's id.
int timer_insert(unsigned int tick, unsigned int interval,int (*func)(int,int),int data1,int data2)
{
	int tid;
	
	tid = acquire_timer();
	timer_data[tid].tick     = gettick()+tick;
	timer_data[tid].func     = func;
	timer_data[tid].id       = tid;
	timer_data[tid].data1    = data1;
	timer_data[tid].data2	 = data2;
	timer_data[tid].type     = TIMER_INTERVAL;
	timer_data[tid].interval = interval;
	push_timer_heap(tid);
	//printf("Added: %u\n",(int)func);
	return tid;
}


/// Retrieves internal timer data
const struct TimerData* get_timer(int tid)
{
	return ( tid >= 0 && tid < timer_data_num ) ? &timer_data[tid] : NULL;
}

/// Marks a timer specified by 'id' for immediate deletion once it expires.
/// Param 'func' is used for debug/verification purposes.
/// Returns 0 on success, < 0 on failure.
int timer_remove(int tid)
{
	if( tid < 0 || tid >= timer_data_num )
	{
		printf("timer_remove error: no such timer %d\n",tid);
		//ShowError("delete_timer error : no such timer %d\n", tid);
		return -1;
	}

	timer_data[tid].func = NULL;
	timer_data[tid].type = TIMER_ONCE_AUTODEL;

	return 0;
}

/// Adjusts a timer's expiration time.
/// Returns the new tick value, or -1 if it fails.



/// Executes all expired timers.
/// Returns the value of the smallest non-expired timer (or 1 second if there aren't any).
int timer_do(unsigned int tick)
{
	int diff = 1000; // return value
	int toDel=0;
	// process all timers one by one
	while( timer_heap_num )
	{
		int tid = timer_heap[timer_heap_num - 1]; // last element in heap (smallest tick)
		toDel=0;
		diff = DIFF_TICK(timer_data[tid].tick, tick);
		if( diff > 0 )
			break; // no more expired timers to process

		--timer_heap_num; // suppress the actual element from the table

		// mark timer as 'to be removed'
		timer_data[tid].type |= TIMER_REMOVE_HEAP;
		//if(tid=3915) printf("CONNECT CHAR!\n");
		if( timer_data[tid].func )
		{
			//printf("%u\n",(int)timer_data[tid].func);
			if( diff <= -1000 )
				// 1秒以上の大幅な遅延が発生しているので、
				// timer処理タイミングを現在値とする事で
				// 呼び出し時タイミング(引数のtick)相対で処理してる
				// timer関数の次回処理タイミングを遅らせる
				toDel=timer_data[tid].func(timer_data[tid].data1,timer_data[tid].data2);
			else
				toDel=timer_data[tid].func(timer_data[tid].data1,timer_data[tid].data2);
				
		}
		//printf("Diff: %d\n",diff);
		if(toDel) timer_remove(tid);
			
		// in the case the function didn't change anything...
		if( timer_data[tid].type & TIMER_REMOVE_HEAP )
		{
			timer_data[tid].type &= ~TIMER_REMOVE_HEAP;

			switch( timer_data[tid].type )
			{
			case TIMER_ONCE_AUTODEL:
				timer_data[tid].type = 0;
				if (free_timer_list_pos >= free_timer_list_max) {
					free_timer_list_max += 256;
					REALLOC(free_timer_list,int,free_timer_list_max);
					memset(free_timer_list + (free_timer_list_max - 256), 0, 256 * sizeof(int));
				}
				free_timer_list[free_timer_list_pos++] = tid;
			break;
			case TIMER_INTERVAL:
				if( DIFF_TICK(timer_data[tid].tick, tick) < -1000 )
					timer_data[tid].tick = tick + timer_data[tid].interval;
				else
					timer_data[tid].tick += timer_data[tid].interval;
				timer_data[tid].type &= ~TIMER_REMOVE_HEAP;
				push_timer_heap(tid);
			break;
			}
		}
	}
	
	return cap_value(diff, TIMER_MIN_INTERVAL, TIMER_MAX_INTERVAL);
}



void timer_init()
{
}

int timer_clear()
{
	

	if (timer_data) FREE(timer_data);
	if (timer_heap) FREE(timer_heap);
	if (free_timer_list) FREE(free_timer_list);
}
