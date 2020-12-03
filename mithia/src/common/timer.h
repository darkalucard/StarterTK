
#ifndef _TIMER_H_
#define _TIMER_H_

#define DIFF_TICK(a,b) ((int)((a)-(b)))
#define INVALID_TIMER -1

// timer flags
#define TIMER_ONCE_AUTODEL 0x01
#define TIMER_INTERVAL     0x02
#define TIMER_REMOVE_HEAP  0x10

struct TimerData {
	unsigned int tick;
	int (*func)(int,int);
	int type;
	unsigned int interval;
	int heap_pos;

	// general-purpose storage
	int id; 
	int data1;
	int data2;
	
};

int timer_insert(unsigned int, unsigned int,int (*)(int,int),int,int);
int timer_remove(int);
int timer_do();
int getDay(void);
int getHour(void);
int getMinute(void);
int getSecond(void);
int timer_clear();
void timer_init();
void Log_Add(const char*, const char*, ...);
unsigned int gettick_nocache();
unsigned int gettick();

#endif
