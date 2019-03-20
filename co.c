#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <setjmp.h>
#include <stdlib.h>
#include <time.h>
#include "co.h"

#define MAX_CO 10

#if defined(__i386__)
  #define SP "%%esp"
#elif defined(__x86_64__)
  #define SP "%%rsp"
#endif

//srand(time(NULL));

struct co {
	uint8_t stack[4096];
	void *backup;
	bool state;
	jmp_buf my_buf;
	int label;
};

struct co *waiting[40];
int my_cnt=0;

struct co *current;

void co_init() {
  current = waiting[0] = (struct co*)malloc(sizeof(struct co));
  srand(time(NULL));
}

struct co* co_start(const char *name, func_t func, void *arg) {
  my_cnt++;
  //  int my_temp = my_cnt;
  struct co *coroutines = (struct co*)malloc(sizeof(struct co));
  waiting[my_cnt]->state = true;
  waiting[my_cnt]->label = my_cnt;
printf("Are you?\n");

  int i = setjmp(waiting[0]->my_buf);
  if(i==0){
  asm volatile("mov " SP ", %0; mov %1, " SP :
		  "=g"(coroutines->backup) :
		  "g"(coroutines->stack+4096));
  current = coroutines;
//  printf("Are you?");
  func(arg); // Test #2 hangs
  current -> state = false;
  waiting[current->label] = waiting[my_cnt];
  my_cnt--;
  int select = rand()%(my_cnt+1) +0;
  current = waiting[select];
  longjmp(waiting[select]->my_buf,waiting[select]->label); 
  asm volatile("mov %0," SP : : "g"(coroutines->backup));
  }
  else{
  return waiting[my_cnt];
  }
}

void co_yield() {
  int my_val = setjmp(current->my_buf);
  if(my_val==0){
	  int my_select = rand()%(my_cnt+1)+0;
	  current = waiting[my_select];
	  longjmp(waiting[my_select]->my_buf,waiting[my_select]->label);
  }
  else return ;
}

void co_wait(struct co *thd) {
  setjmp(current->my_buf);
  if(thd->state){
	  int se = rand()%(my_cnt)+1;
	  current = waiting[se];
	  longjmp(waiting[se]->my_buf,waiting[se]->label);
  }
  else {
	  free(thd);
  }
}

