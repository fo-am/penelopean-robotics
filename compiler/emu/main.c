#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "yarn.h"

#define REG_ROBOT_ID 0 
#define REG_PC_MIRROR 1 
#define REG_LED 2
#define REG_COMP_ANGLE 3
#define REG_COMP_DELTA_RESET 4
#define REG_COMP_DELTA 5
#define REG_SERVO_STEP_COUNT 6
#define REG_SERVO_STEP_COUNT_RESET 7
#define REG_SERVO_NEXT_PATTERN_ID 8
// telemetry visible user area
#define REG_USR_A 9
#define REG_USR_B 10
#define REG_USR_C 11
#define REG_USR_D 12

#define REG_SERVO_MS_PER_STEP 13  // arg step not=step counter type of step
#define REG_SERVO_1_AMP 14
#define REG_SERVO_2_AMP 15
#define REG_SERVO_3_AMP 16
#define REG_SERVO_1_BIAS 17
#define REG_SERVO_2_BIAS 18
#define REG_SERVO_3_BIAS 19
#define REG_SERVO_1_SMOOTH 20
#define REG_SERVO_2_SMOOTH 21
#define REG_SERVO_3_SMOOTH 22

#define CODE_START 32

void print_stack(yarn_machine *m) {
  char stack[1024]="\0";
  for (int i=HEAP_SIZE; i>HEAP_SIZE-10; i--) {
    char t[1024];
    if (yarn_stack_pos(m)==i) {
      snprintf(t,1024,"[%03d] ", m->m_heap[i]);
    } else {
      snprintf(t,1024,"%03d ", m->m_heap[i]);
    }
    strcat(stack,t);
  }
  printf("%s\n",stack);
}

int main(int argc, char **argv) {
  if (argc<2) {
    printf("usage: yarnemu <binfilename>\n");
    return 1;
  }

  FILE *f = fopen(argv[1],"rb");
  if (f==NULL) {
    printf("yarnemu could not find %s\n",argv[1]);
    return 1;
  }

  yarn_machine m;
  yarn_init(&m);
  
  unsigned int pos=CODE_START;
  unsigned short data=0;
  fread(&data,2,1,f);
  while(!feof(f)) {
    //printf("%d: %d\n",pos,data);
    yarn_poke(&m,pos++,data);
    fread(&data,2,1,f);
  }

  yarn_set_pc(&m,CODE_START);

  unsigned int min_stack=STACK_START;
  
  while(1) {

      char out[1024]="\0";
      char stack[1024]="\0";

      snprintf(out,1024,"%03d:%s (%03d) s_top:%03d wrk:%03d st:%03d stf:%03d led:%03d",
      	       yarn_pc(&m),
      	       yarn_opcode_text(m.m_heap[yarn_pc(&m)]),
      	       m.m_heap[yarn_pc(&m)+1],
      	       m.m_heap[yarn_stack_pos(&m)],
      	       m.m_heap[32],
      	       yarn_stack_pos(&m),
      	       m.m_heap[24], // print the stack frame
      	       m.m_heap[REG_LED]);
      printf("%s",out);
      printf("\n");
      
      print_stack(&m);
      //printf("\n");
      char c;
      //      scanf("%c",&c);

      if (yarn_stack_pos(&m)<min_stack) {
	min_stack=yarn_stack_pos(&m);
	//printf("min stack now: %d\n",min_stack);
      }

      if (yarn_pc(&m)>min_stack) {
	printf("heap/stack collision! [s%d - h%d]\n",yarn_pc(&m),min_stack);
	exit(0);
      }
      
    if (m.m_heap[REG_LED]!=0 ||
	m.m_heap[yarn_pc(&m)]==0) {
      printf("%d (stk:%d)\n",m.m_heap[REG_LED],yarn_stack_pos(&m));
      exit(0);
    }
    yarn_run(&m);
    //sleep(1);
  }
  
  return 0;
}
