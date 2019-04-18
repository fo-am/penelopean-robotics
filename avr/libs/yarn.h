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

#ifndef YARN_MACHINE
#define YARN_MACHINE

typedef unsigned short cell_t;    
typedef short signed_cell_t;    
#define STACK_SIZE 8
#define HEAP_SIZE 256

// yarn machine instructions
#define NOP   0 // do nothing

// flow control
#define JMP   1 // jump absolute
#define JPR   2 // jupp relative (operand can be negative)
#define JPZ   3 // jump if top is zero
#define JPRZ  4 // jupp relative (operand can be negative) if top is zero

// memory operators
#define LDL   5 // load literal to stack
#define LD    6 // load contents of address to stack
#define LDI   7 // load value pointed to by contents of address
#define ST    8 // store value on top of stack to address
#define STI   9 // store value to location pointed to by contents of address
#define DUP   10 // push a copy of the top value to the stack

// decision making
#define EQU  11 // push 1 if top two values are equal, 0 if not
#define LT   12 // push 1 if top two values are less than, 0 if not
#define GT   13 // push 1 if top two values are greater than, 0 if not
#define LTE  14 // push 1 if top two values are less than or equal, 0 if not
#define GTE  15 // ...
#define SLT  16 // signed versions
#define SGT  17
#define SLTE 18
#define SGTE 19

// maths
#define ADD  20
#define SUB  21
#define INC  22
#define DEC  23
#define AND  24 // bitwise stuff
#define OR   25
#define XOR  26
#define NOT  27
#define RR   28 // rotate right
#define RL   29 // rotate left
#define SIN  30 // uses degrees
#define COS  31 // "
#define TAN  32 // "
#define RND  33

// misc helper for looping
#define INCI 34 // increment value in address in place
#define DECI 35 // decrement value in address in place
#define INCP 36 // increment value in address in place and push
#define DECP 37 // decrement value in address in place and push

typedef struct {
  cell_t m_pc;
  int m_stack_pos;
  cell_t *m_stack;
  cell_t *m_heap;
} yarn_machine;
    
void yarn_init(yarn_machine *t);
void yarn_run(yarn_machine *t);
cell_t yarn_peek(const yarn_machine *m, cell_t addr);
void yarn_poke(yarn_machine *m, cell_t addr, cell_t data);
void yarn_run(yarn_machine *m);
const cell_t yarn_stack_count(yarn_machine* m, cell_t c);
void yarn_push(yarn_machine *m, cell_t data);
cell_t yarn_pop(yarn_machine *m);
cell_t yarn_top(yarn_machine *m);

#endif
