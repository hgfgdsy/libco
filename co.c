#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <setjmp.h>
#include <stdlib.h>
#include <time.h>
#include "co.h"

#if defined(__i386__)
# define SP "%%esp"
#elif defined(__x86_64__)
  #define SP "%%rsp"
#endif

//srand(time(NULL));

struct co {
	uint8_t stack[1<<15];
	void *backup;
	bool state;
	jmp_buf my_buf;
	int label;
}__attribute__((aligned(16)));

//struct co *waiting[40];
int my_cnt=1;
int select1;
struct co *current;
struct co waiting[40];

void co_init() {
  current = (struct co*)&waiting[1];
  waiting[1].label = 1;
  srand(time(NULL));
}

struct co* co_start(const char *name, func_t func, void *arg) {
  my_cnt++;
  //  int my_temp = my_cnt;
/*  struct co *coroutines = (struct co*)malloc(sizeof(struct co));printf("Are you?\t%d\n",my_cnt);

  waiting[my_cnt] = coroutines;*/
  //  current = coroutines;
//  waiting[my_cnt] = (struct co*)malloc(sizeof(struct co));printf("Are you?\t%d\n",my_cnt);
//  waiting[my_cnt]->state = true;
//  waiting[my_cnt]->label = my_cnt;
  current = (struct co*)&waiting[my_cnt];
  int i = setjmp(waiting[1].my_buf);
  if(i==0){
  asm volatile("mov " SP ", %0; mov %1, " SP :
		  "=g"(waiting[my_cnt].backup) :
		  "g"(waiting[my_cnt].stack+(1<<15)));
  waiting[my_cnt].state = true;
  waiting[my_cnt].label = my_cnt;
//  current = waiting[cnt];
  printf("We start!\n");
  func(arg); // Test #2 hangs
  printf("Have you finished?\n");
  printf("Kao a\n");
  current -> state = false;
//  waiting[my_cnt+1] = waiting[current->label];
//  waiting[current->label] = waiting[my_cnt];
//  my_cnt--;
  bool decide = false;
  int my_temp = current->label;
  while(!decide){
  select1 = rand()%(my_cnt) +1;
  decide = waiting[select1].state;
  }
  current = (struct co*)&waiting[select1];
  longjmp(waiting[select1].my_buf,waiting[select1].label); 
  asm volatile("mov %0," SP : : "g"(waiting[my_temp].backup));
//  longjmp(waiting[select1]->my_buf,waiting[select1]->label);
//  return waiting[my_cnt];
  }
  else{
  printf("We back!\n");
  return (struct co*)&waiting[my_cnt];
  }
//   return waiting[my_cnt];
}

void co_yield() {
  int my_val = setjmp(current->my_buf);
  printf("%d\t%d\n",my_val,current->label);
  if(my_val==0){
	  bool decide=false;
	  int my_select;
	  while(!decide){
	  my_select = rand()%(my_cnt)+1;
	  decide = waiting[my_select].state;
	  }
	  printf("yieldselect=%d\n",my_select);
	  current = (struct co*)&waiting[my_select];
	  longjmp(waiting[my_select].my_buf,current->label);
  }
  else printf("Are you sure?\n");
//  return ;
}

void co_wait(struct co *thd) {
  setjmp(current->my_buf);
  printf("%d\n",thd->label);
  if(thd->state){
//	  int se = rand()%(my_cnt-1)+2;
//	  printf("select=%d\n",se);
	  current = (struct co*)&waiting[thd->label];
	  longjmp(waiting[thd->label].my_buf,1);
  }
  else {
//	  free(thd);
  }
}

