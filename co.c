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
uint8_t *__stack;
void *__stack_backup;
func_t my_func;
void * my_arg;
//srand(time(NULL));
struct co {
	void *backup;
	bool state;
	jmp_buf my_buf;
	int label;
	uint8_t stack[1<<12];
}__attribute__((aligned(16)));

//struct co *waiting[40];
int my_cnt=1;
int select1;
struct co *current;
struct co waiting[40];

void co_init() {
  current = (struct co*)&waiting[1];
  waiting[1].label = 1;
  waiting[1].state = true;
  srand(time(NULL));
//  printf("SP="SP"\n");
}

struct co* co_start(const char *name, func_t func, void *arg) {
  my_cnt++;
  my_func = func;
  my_arg = arg;
  //  int my_temp = my_cnt;
/*  struct co *coroutines = (struct co*)malloc(sizeof(struct co));printf("Are you?\t%d\n",my_cnt);

  waiting[my_cnt] = coroutines;*/
  //  current = coroutines;
//  waiting[my_cnt] = (struct co*)malloc(sizeof(struct co));printf("Are you?\t%d\n",my_cnt);
//  waiting[my_cnt]->state = true;
//  waiting[my_cnt]->label = my_cnt;
//  current = (struct co*)&waiting[my_cnt];
  int i = setjmp(waiting[1].my_buf);
  if(i==0){
/*  asm volatile("mov " SP ", %0 ; mov %1, " SP:
		  "=g"(waiting[my_cnt].backup) :
		  "g"(waiting[my_cnt].stack+(1<<12)));
*/
//  printf("%ld\n",(long int)(waiting[my_cnt].stack+(1<<12)));
  __stack = (waiting[my_cnt].stack+sizeof(waiting[my_cnt].stack));
//  int vao=0;
//  printf("%d\n",vao);
  asm volatile("mov " SP ", %0; mov %1, " SP :
                 "=g"(__stack_backup) :
                 "g"(__stack));
  waiting[my_cnt].backup = __stack_backup;
  current = (struct co*)&waiting[my_cnt];
  waiting[my_cnt].state = true;
  waiting[my_cnt].label = my_cnt;
//  current = waiting[cnt];
//  printf("We start!\n"); 
  my_func(my_arg); // Test #2 hangs
  printf("Have you finished?\n");
//  printf("Kao a\n");
  current -> state = false;
//  waiting[my_cnt+1] = waiting[current->label];
//  waiting[current->label] = waiting[my_cnt];
//  my_cnt--;
  bool decide = false;
  int my_temp = current->label;
  printf("Here?\n");
  while(!decide){
  select1 = rand()%(my_cnt) +1;
  printf("select1=%d\n");
  decide = waiting[select1].state;
  }
  printf("So may here?\n");
  current = (struct co*)&waiting[select1];
  printf("Can you come here?\n");
  longjmp(waiting[select1].my_buf,waiting[select1].label); 
  __stack_backup = waiting[my_temp].backup;
  asm volatile("mov %0, " SP : : "g"(__stack_backup));
//  longjmp(waiting[select1]->my_buf,waiting[select1]->label);
//  return waiting[my_cnt];
  }
  else{
//  printf("We back!\n");
  return (struct co*)&waiting[my_cnt];
  }
//   return waiting[my_cnt];
}

void co_yield() {
  int my_val = setjmp(current->my_buf);
//  printf("%d\t%d\n",my_val,current->label);
  if(my_val==0){
	  bool decide=false;
	  int my_select;
	  while(!decide){
	  my_select = rand()%(my_cnt)+1;
	  decide = waiting[my_select].state;
	  }
//	  printf("yieldselect=%d\n",my_select);
	  current = (struct co*)&waiting[my_select];
	  longjmp(waiting[my_select].my_buf,current->label);
  }
//  else printf("Are you sure?\n");
//  return ;
}

void co_wait(struct co *thd) {
  setjmp(current->my_buf);
//  printf("%d\n",thd->label);
  if(thd->state){
	  //	  printf("select=%d\n",se);
          bool decide = false;
	  int se;
	  while(!decide){
          se = rand()%(my_cnt-1)+2;
	  decide = waiting[se].state;
	  }
	  current = (struct co*)&waiting[se];
	  longjmp(waiting[se].my_buf,1);
  }
  else {
//	  free(thd);
  }
}

