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
  m->m_stack=(cell_t*)malloc(sizeof(cell_t)*STACK_SIZE);
  for (int n=0; n<STACK_SIZE; n++) {
    m->m_stack[n]=0;
  }
  m->m_heap=(cell_t*)malloc(sizeof(cell_t)*HEAP_SIZE);
  for (int n=0; n<HEAP_SIZE; n++) {
    m->m_heap[n]=0;
  }
  yarn_reset(m);
}

void yarn_reset(yarn_machine *m) {
  m->m_pc=0;
  m->m_stack_pos=-1;
}

cell_t yarn_peek(const yarn_machine* m, cell_t addr) {
  return m->m_heap[addr%HEAP_SIZE];
}

void yarn_poke(yarn_machine* m, cell_t addr, cell_t data) {
  m->m_heap[addr%HEAP_SIZE]=data;
}

void yarn_run(yarn_machine* m) {
  cell_t instr=yarn_peek(m,m->m_pc);
  m->m_pc++;
  switch(instr) {
  case NOP: break;
  case JMP: m->m_pc=yarn_peek(m,m->m_pc++); break;
  case JPR: m->m_pc+=(signed_cell_t)yarn_peek(m,m->m_pc); break;
  case JPZ: if (yarn_stack_count(m,1) && yarn_pop(m)==0) m->m_pc=yarn_peek(m,m->m_pc); else m->m_pc++; break;
  case JPRZ: if (yarn_stack_count(m,1) && yarn_pop(m)==0) m->m_pc+=(signed_cell_t)yarn_peek(m,m->m_pc); else m->m_pc++; break;
  case LDL: yarn_push(m,yarn_peek(m,m->m_pc++)); break;
  case LD: yarn_push(m,yarn_peek(m,yarn_peek(m,m->m_pc++))); break;
  case LDI: yarn_push(m,yarn_peek(m,yarn_peek(m,yarn_peek(m,m->m_pc++)))); break;
  case ST: if (yarn_stack_count(m,1)) yarn_poke(m,yarn_peek(m,m->m_pc++),yarn_pop(m)); break;
  case STI: if (yarn_stack_count(m,1)) yarn_poke(m,yarn_peek(m,yarn_peek(m,m->m_pc++)),yarn_pop(m)); break;
  case DUP: if (yarn_stack_count(m,1)) yarn_push(m,yarn_top(m)); break;
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
  case INC: if (yarn_stack_count(m,1)) yarn_push(m,yarn_pop(m)+1); break;
  case DEC: if (yarn_stack_count(m,1)) yarn_push(m,yarn_pop(m)-1); break;
  case AND: if (yarn_stack_count(m,2)) yarn_push(m,yarn_pop(m)&yarn_pop(m)); break;
  case OR: if (yarn_stack_count(m,2)) yarn_push(m,yarn_pop(m)|yarn_pop(m)); break;
  case XOR: if (yarn_stack_count(m,2)) yarn_push(m,yarn_pop(m)^yarn_pop(m)); break;
  case NOT: if (yarn_stack_count(m,1)) yarn_push(m,~yarn_pop(m)); break;
  case RR: if (yarn_stack_count(m,2)) yarn_push(m,yarn_pop(m)>>(yarn_peek(m,m->m_pc++)%8)); break;
  case RL: if (yarn_stack_count(m,2)) yarn_push(m,yarn_pop(m)<<(yarn_peek(m,m->m_pc++)%8)); break;
  case SIN: if (yarn_stack_count(m,1)) yarn_push(m,sin(yarn_pop(m)*57.2958)); break;
  case COS: if (yarn_stack_count(m,1)) yarn_push(m,cos(yarn_pop(m)*57.2958)); break;
  case TAN: if (yarn_stack_count(m,1)) yarn_push(m,tan(yarn_pop(m)*57.2958)); break;
  case RND: yarn_push(m,rand()); break;
  case INCI: {
    cell_t d=yarn_peek(m,m->m_pc++); 
    yarn_poke(m,d,yarn_peek(m,d)+1); 
  } break;
  case DECI: {
    cell_t d=yarn_peek(m,m->m_pc++); 
    yarn_poke(m,d,yarn_peek(m,d)-1); 
  } break;
  case INCP: {
    cell_t d=yarn_peek(m,m->m_pc++); 
    yarn_poke(m,d,yarn_peek(m,d)+1); 
    yarn_push(m,yarn_peek(m,d));
  } break;
  case DECP: {
    cell_t d=yarn_peek(m,m->m_pc++); 
    yarn_poke(m,d,yarn_peek(m,d)-1); 
    yarn_push(m,yarn_peek(m,d));
  } break;
  default : break;
  };   
}

const cell_t yarn_stack_count(yarn_machine* m, cell_t c) { 
  return (c-1)<=m->m_stack_pos; 
}

void yarn_push(yarn_machine* m, cell_t data) {
  if (m->m_stack_pos<STACK_SIZE-1) {
    m->m_stack[++m->m_stack_pos]=data;
  }
}

cell_t yarn_pop(yarn_machine* m) {
  if (m->m_stack_pos>=0) {
    cell_t ret=m->m_stack[m->m_stack_pos];
    m->m_stack_pos--;
    return ret;
  }
  return 0;   
}

cell_t yarn_top(yarn_machine* m) {
  if (m->m_stack_pos>=0) {
    return m->m_stack[m->m_stack_pos];
  }
  return 0;
}

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
    y.m_heap[n]=c[n];
  }

  for (unsigned int n=0; n<100; n++) {
    printf("%d %d\n",y.m_pc,y.m_heap[50]);
    yarn_run(&y);
  }
}

#endif
