#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "yarn.h"

#define REG_LED 3
#define CODE_START 64

void print_stack(yarn_machine *m) {
  char stack[1024]="\0";
  for (int i=yarn_stack_pos(m); i<HEAP_SIZE; i++) {
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

void print_info(yarn_machine *m) {
  char out[1024]="\0";
  char stack[1024]="\0";
  
  snprintf(out,1024,"%03d:%s (%03d) s_top:%03d wrk:%03d st:%03d stf:%03d led:%03d",
	   yarn_pc(m),
	   yarn_opcode_text(m->m_heap[yarn_pc(m)]),
	   cell_arg(m->m_heap[yarn_pc(m)]),
	   m->m_heap[yarn_stack_pos(m)],
	   m->m_heap[32],
	   yarn_stack_pos(m),
	   m->m_heap[24], // print the stack frame
	   m->m_heap[REG_LED]);
  printf("%s",out);
  printf("\n");
      
  print_stack(m);
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
    //printf("%d: [%s] %d %d %d\n",pos,yarn_opcode_text(data),data,
    //    cell_instr(data), cell_arg(data));
    yarn_poke(&m,pos++,data);
    fread(&data,2,1,f);
  }

  yarn_set_pc(&m,CODE_START);

  unsigned int min_stack=STACK_START;
  
  while(1) {

    //print_info(&m);
    char c;
    //    scanf("%c",&c);

      if (yarn_stack_pos(&m)<min_stack) {
	min_stack=yarn_stack_pos(&m);
	//printf("min stack now: %d\n",min_stack);
      }

      if (yarn_pc(&m)>min_stack) {
	print_info(&m);
	printf("heap/stack collision! [s%d - h%d]\n",yarn_pc(&m),min_stack);
	exit(0);
      }
      
    if (m.m_heap[REG_LED]!=0 ||
	m.m_heap[yarn_pc(&m)]==0) {
      print_info(&m);
      printf("halt: %d (stk:%d)\n",m.m_heap[REG_LED],yarn_stack_pos(&m));
      exit(0);
    }
    yarn_run(&m);
    //sleep(1);
  }
  
  return 0;
}
