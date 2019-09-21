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
#define HEAP_SIZE 2048
#define STACK_START HEAP_SIZE
#define REG_PC 0
#define REG_ST 1

// yarn machine instructions
#define NOP   0 // do nothing

// flow control
#define JMP   1  // jump absolute
#define JPR   2  // jupp relative (operand can be negative)
#define JPZ   3  // jump if top is zero
#define JPRZ  4  // jupp relative (operand can be negative) if top is zero
#define JSR   5 
#define RTS   6

// memory operators
#define LDL   7  // load literal to stack
#define LD    8  // load contents of address to stack
#define LDI   9  // load value pointed to by contents of address
#define LDSI  10  // load value pointed to by top of stack
#define ST    11 // store value on top of stack to address
#define STI   12 // store value to location pointed to by contents of address
#define STS   13 // store stack pointer in location
#define STSI  14 // pop the address then pop the value to write to it 
#define DUP   15 // push a copy of the top value to the stack
#define DROP  16
#define SWAP  17

// decision making
#define EQU  18 // push 1 if top two values are equal, 0 if not
#define LT   19 // push 1 if top two values are less than, 0 if not
#define GT   20 // push 1 if top two values are greater than, 0 if not
#define LTE  21 // push 1 if top two values are less than or equal, 0 if not
#define GTE  22 // ...
#define SLT  23 // signed versions
#define SGT  24
#define SLTE 25
#define SGTE 26

// maths
#define ADD  27
#define SUB  28
#define ADDL 29
#define SUBL 30
#define INC  31
#define DEC  32
#define AND  33 // bitwise stuff
#define OR   34
#define XOR  35
#define NOT  36
#define NOTL 37
#define RR   38 // rotate right
#define RL   39 // rotate left
#define SIN  40 // uses degrees
#define COS  41 // "
#define TAN  42 // "
#define RND  43

typedef struct {
  cell_t *m_heap;
} yarn_machine;

void yarn_init(yarn_machine *t);
void yarn_reset(yarn_machine *t);
void yarn_run(yarn_machine *t);
cell_t yarn_pc(const yarn_machine *t);
void yarn_set_pc(yarn_machine *t, cell_t v);
cell_t yarn_stack_pos(const yarn_machine *t);
cell_t yarn_peek(const yarn_machine *m, cell_t addr);
void yarn_poke(yarn_machine *m, cell_t addr, cell_t data);
void yarn_run(yarn_machine *m);
cell_t yarn_stack_count(const yarn_machine* m, cell_t c);
void yarn_push(yarn_machine *m, cell_t data);
cell_t yarn_pop(yarn_machine *m);
cell_t yarn_top(const yarn_machine *m);

char *yarn_opcode_text(cell_t opcode);

#endif
