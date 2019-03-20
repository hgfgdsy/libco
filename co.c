#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <setjmp.h>
#include <stdlib.h>
#include <time.h>
#include "co.h"

#if defined(__i386__)
  #define SP "%%esp"
#elif defined(__x86_64__)
  #define SP "%%rsp"
#endif

//srand(time(NULL));

struct co {
	uint8_t stack[8192];
	void *backup;
	bool state;
	jmp_buf my_buf;
	int label;
}__attribute__((aligned(16)));

struct co *waiting[40];
int my_cnt=1;
int select;
struct co *current;

void co_init() {
  current = waiting[1] = (struct co*)malloc(sizeof(struct co));
  waiting[1]->label = 1;
  srand(time(NULL));
}

struct co* co_start(const char *name, func_t func, void *arg) {
  my_cnt++;
  //  int my_temp = my_cnt;
/*  struct co *coroutines = (struct co*)malloc(sizeof(struct co));printf("Are you?\t%d\n",my_cnt);

  waiting[my_cnt] = coroutines;
  waiting[my_cnt]->state = true;
  waiting[my_cnt]->label = my_cnt;
  current = coroutines;*/
  waiting[my_cnt] = (struct co*)malloc(sizeof(struct co));printf("Are you?\t%d\n",my_cnt);
  current = waiting[my_cnt];
  int i = setjmp(waiting[1]->my_buf);
  if(i==0){
  asm volatile("mov " SP ", %0; mov %1, " SP :
		  "=g"(waiting[my_cnt]->backup) :
		  "g"(waiting[my_cnt]->stack+8192));
  waiting[my_cnt]->state = true;
  waiting[my_cnt]->label = my_cnt;
//  current = waiting[cnt];
  printf("We start!\n");
  func(arg); // Test #2 hangs
//asm volatile("mov %0," SP : : "g"(coroutines->backup));
  printf("Have you finished?\n");
  printf("Kao a\n");
  current -> state = false;
  waiting[my_cnt+1] = waiting[current->label];
  waiting[current->label] = waiting[my_cnt];
  my_cnt--;
  select = rand()%(my_cnt) +1;
  current = waiting[select];
//  longjmp(waiting[select]->my_buf,waiting[select]->label); 
  asm volatile("mov %0," SP : : "g"(waiting[my_cnt+2]->backup));
  longjmp(waiting[select]->my_buf,waiting[select]->label);
  //    return waiting[my_cnt];
  }
  else{
  return waiting[my_cnt];
  }
}

void co_yield() {
  int my_val = setjmp(current->my_buf);
  printf("%d\t%d\n",my_val,current->label);
  if(my_val==0){
	  int my_select = rand()%(my_cnt)+1;
	  printf("yieldselect=%d\n",my_select);
	  current = waiting[my_select];
	  longjmp(waiting[my_select]->my_buf,current->label);
  }
  else printf("Are you sure?\n");
  return ;
}

void co_wait(struct co *thd) {
  setjmp(current->my_buf);
  printf("%d\n",thd->label);
  if(thd->state){
	  int se = rand()%(my_cnt-1)+2;
	  printf("select=%d\n",se);
	  current = waiting[se];
	  longjmp(waiting[se]->my_buf,waiting[se]->label);
  }
  else {
//	  free(thd);
  }
}

