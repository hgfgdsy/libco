#include <stdio.h>
#include "co.h"

#define MAX_CO 10

#if defined(__i386__)
  #define SP "%%esp"
#elif defined(__x86_64__)
  #define SP "%%rsp"
#endif

struct co {
	uint8_t stack[4096];
	void *backup;
	bool state;

};

int cnt=0;

struct co coroutines[MAX_CO];

void co_init() {
}

struct co* co_start(const char *name, func_t func, void *arg) {
  int my_temp = my_cnt;
  asm volatile("mov " SP ", %0; mov %1, " SP :
		  "=g"(coroutines[my_cnt].backup) :
		  "g"((coroutines[my_cnt].stack+4096)&(~0xf)));
  func(arg); // Test #2 hangs
  asm volatile("mov %0," SP : : "g"(coroutines[my_temp].backup));
  return coroutines[my_temp];
}

void co_yield() {
}

void co_wait(struct co *thd) {
}

