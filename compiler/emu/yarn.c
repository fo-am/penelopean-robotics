// Spork Factory Copyright (C) 2012 David Griffiths
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <stdlib.h>
#include <math.h>
#include "yarn.h"

//////////////////////////////////////////////////////////

void yarn_init(yarn_machine *m) {
  m->m_heap=(cell_t*)malloc(sizeof(cell_t)*HEAP_SIZE);
  for (int n=0; n<HEAP_SIZE; n++) {
    m->m_heap[n]=0;
  }
  yarn_reset(m);
}

void yarn_reset(yarn_machine *m) {
  m->m_heap[REG_PC]=0;
  m->m_heap[REG_ST]=STACK_START;
}

cell_t yarn_peek(const yarn_machine* m, cell_t addr) {
  return m->m_heap[addr%HEAP_SIZE];
}

void yarn_poke(yarn_machine* m, cell_t addr, cell_t data) {
  m->m_heap[addr%HEAP_SIZE]=data;
}

void yarn_run(yarn_machine* m) {
  cell_t pc=yarn_pc(m);
  cell_t instr=yarn_peek(m,pc++);
  switch(instr) {
  case NOP: break;
  case JMP: pc=yarn_peek(m,pc++); break;
  case JPR: pc+=(signed_cell_t)yarn_peek(m,pc); break;
  case JPZ: if (yarn_stack_count(m,1) && yarn_pop(m)==0) pc=yarn_peek(m,pc); else pc++; break;
  case JPRZ: if (yarn_stack_count(m,1) && yarn_pop(m)==0) pc+=(signed_cell_t)yarn_peek(m,pc); else pc++; break;
  case JSR: yarn_push(m,pc+1); pc=yarn_peek(m,pc++); break;
  case RTS: if (yarn_stack_count(m,1)) pc=yarn_pop(m); break;
  case LDL: yarn_push(m,yarn_peek(m,pc++)); break;
  case LD: yarn_push(m,yarn_peek(m,yarn_peek(m,pc++))); break;
  case LDI: yarn_push(m,yarn_peek(m,yarn_peek(m,yarn_peek(m,pc++)))); break;
  case LDSI: yarn_push(m,yarn_peek(m,yarn_pop(m))); break;
  case ST: if (yarn_stack_count(m,1)) yarn_poke(m,yarn_peek(m,pc++),yarn_pop(m)); break;
  case STI: if (yarn_stack_count(m,1)) yarn_poke(m,yarn_peek(m,yarn_peek(m,pc++)),yarn_pop(m)); break;
  case STS: yarn_poke(m,yarn_peek(m,pc++),yarn_stack_pos(m)); break;
  case STSI: if (yarn_stack_count(m,2)) { cell_t addr=yarn_pop(m); cell_t val=yarn_pop(m); yarn_poke(m,addr,val); } break;
  case DUP: if (yarn_stack_count(m,1)) yarn_push(m,yarn_top(m)); break;
  case DROP: for (unsigned int i=0; i<yarn_peek(m,pc); i++) { yarn_pop(m); } pc++; break;
  case SWAP: { cell_t a=yarn_pop(m); cell_t b=yarn_pop(m); yarn_push(m,a); yarn_push(m,b); } break;
  case EQU: if (yarn_stack_count(m,2)) yarn_push(m,yarn_pop(m)==yarn_pop(m)); break;
  case LT: if (yarn_stack_count(m,2)) yarn_push(m,yarn_pop(m)<yarn_pop(m)); break;
  case GT: if (yarn_stack_count(m,2)) yarn_push(m,yarn_pop(m)>yarn_pop(m)); break;
  case LTE: if (yarn_stack_count(m,2)) yarn_push(m,yarn_pop(m)<=yarn_pop(m)); break;
  case GTE: if (yarn_stack_count(m,2)) yarn_push(m,yarn_pop(m)>=yarn_pop(m)); break;
  case SLT: if (yarn_stack_count(m,2)) yarn_push(m,(signed_cell_t)yarn_pop(m)<(signed_cell_t)yarn_pop(m)); break;
  case SGT: if (yarn_stack_count(m,2)) yarn_push(m,(signed_cell_t)yarn_pop(m)>(signed_cell_t)yarn_pop(m)); break;
  case SLTE: if (yarn_stack_count(m,2)) yarn_push(m,(signed_cell_t)yarn_pop(m)<=(signed_cell_t)yarn_pop(m)); break;
  case SGTE: if (yarn_stack_count(m,2)) yarn_push(m,(signed_cell_t)yarn_pop(m)>=(signed_cell_t)yarn_pop(m)); break;
  case ADD: if (yarn_stack_count(m,2)) yarn_push(m,yarn_pop(m)+yarn_pop(m)); break;
  case SUB: if (yarn_stack_count(m,2)) yarn_push(m,yarn_pop(m)-yarn_pop(m)); break;
  case ADDL: if (yarn_stack_count(m,1)) yarn_push(m,yarn_pop(m)+yarn_peek(m,pc++)); break;
  case SUBL: if (yarn_stack_count(m,1)) yarn_push(m,yarn_pop(m)-yarn_peek(m,pc++)); break;
  case INC: if (yarn_stack_count(m,1)) yarn_push(m,yarn_pop(m)+1); break;
  case DEC: if (yarn_stack_count(m,1)) yarn_push(m,yarn_pop(m)-1); break;
  case AND: if (yarn_stack_count(m,2)) yarn_push(m,yarn_pop(m)&yarn_pop(m)); break;
  case OR: if (yarn_stack_count(m,2)) yarn_push(m,yarn_pop(m)|yarn_pop(m)); break;
  case XOR: if (yarn_stack_count(m,2)) yarn_push(m,yarn_pop(m)^yarn_pop(m)); break;
  case NOT: if (yarn_stack_count(m,1)) yarn_push(m,~yarn_pop(m)); break;
  case NOTL: if (yarn_stack_count(m,1)) yarn_push(m,!yarn_pop(m)); break;
  case RR: if (yarn_stack_count(m,2)) yarn_push(m,yarn_pop(m)>>(yarn_peek(m,pc++)%8)); break;
  case RL: if (yarn_stack_count(m,2)) yarn_push(m,yarn_pop(m)<<(yarn_peek(m,pc++)%8)); break;
  case SIN: if (yarn_stack_count(m,1)) yarn_push(m,sin(yarn_pop(m)*57.2958)); break;
  case COS: if (yarn_stack_count(m,1)) yarn_push(m,cos(yarn_pop(m)*57.2958)); break;
  case TAN: if (yarn_stack_count(m,1)) yarn_push(m,tan(yarn_pop(m)*57.2958)); break;
  case RND: yarn_push(m,rand()); break;
  default : break;
  };
  yarn_set_pc(m,pc);
}

cell_t yarn_pc(const yarn_machine *m) {
  return m->m_heap[REG_PC];
}

void yarn_set_pc(yarn_machine *m, cell_t v) {
  m->m_heap[REG_PC]=v;
}

cell_t yarn_stack_pos(const yarn_machine *m) {
  return m->m_heap[REG_ST];
}

cell_t yarn_stack_count(const yarn_machine* m, cell_t c) { 
  return (c-1)<=STACK_START-m->m_heap[REG_ST];
}

void yarn_push(yarn_machine* m, cell_t data) {
  m->m_heap[--m->m_heap[REG_ST]]=data;
}

cell_t yarn_pop(yarn_machine* m) {
  if (m->m_heap[REG_ST]>=0) {
    cell_t ret=m->m_heap[m->m_heap[REG_ST]];
    m->m_heap[REG_ST]++;
    return ret;
  }
  return 0;   
}

cell_t yarn_top(const yarn_machine* m) {
  if (m->m_heap[REG_ST]<=STACK_START) {
    return m->m_heap[REG_ST];
  }
  return 0;
}


char *yarn_opcode_text(cell_t opcode) {
  switch(opcode) {
  case NOP: return "nop "; 
  case JMP: return "jmp ";
  case JPR: return "jpr ";
  case JPZ: return "jpz ";
  case JPRZ: return "jprz";
  case JSR: return "jsr ";
  case RTS: return "rts ";
  case LDL: return "ldl ";
  case LD: return "ld  ";
  case LDI: return "ldi ";
  case LDSI: return "ldsi";
  case ST: return "st  ";
  case STI: return "sti ";
  case STS: return "sts ";
  case STSI: return "stsi";
  case DUP: return "dup ";
  case DROP: return "drop";
  case SWAP: return "swap";
  case EQU: return "equ ";
  case LT: return "lt  ";
  case GT: return "gt  ";
  case LTE: return "lte ";
  case GTE: return "gte ";
  case SLT: return "slt ";
  case SGT: return "sgt ";
  case SLTE: return "slte";
  case SGTE: return "sgte";
  case ADD: return "add ";
  case SUB: return "sub ";
  case ADDL: return "addl";
  case SUBL: return "subl";
  case INC: return "inc ";
  case DEC: return "dec ";
  case AND: return "and ";
  case OR: return "or  ";
  case XOR: return "xor ";
  case NOT: return "not ";
  case NOTL: return "notl";
  case RR: return "rr  ";
  case RL: return "ll  ";
  case SIN: return "sin ";
  case COS: return "cos ";
  case TAN: return "tan ";
  case RND: return "rnd ";
  default: return"????";
  };   
}


//#define UNIT_TEST
#ifdef UNIT_TEST

#include<stdio.h>

void main() {
  yarn_machine y;
  yarn_init(&y);

  cell_t c[] = {LDL, 10,    // 0
		ST, 50,     // 2
		DECP, 50,   // 4
		JPRZ, 3,    // 6
		JMP, 4,     // 8
		LDL, 99,
		LDL, 20,
                XOR,
		ST, 50};

  for (unsigned int n=0; n<sizeof(c); n++) {
    y.m_heap[n+32]=c[n];
  }

  for (unsigned int n=0; n<100; n++) {
    printf("%d %d\n",y.m_pc,y.m_heap[50]);
    yarn_run(&y);
  }
}

#endif
