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

// A bytecode interpreter designed to allow remote livecoding
// of robotics on avr microcontrollers or other small spaces.
// 16 bit machine, optimised for 1K heap/stack as that is about
// all I can use on atmega328

#ifndef YARN_MACHINE
#define YARN_MACHINE

typedef unsigned short cell_t;    
typedef short signed_cell_t;    

// we pack intructions and one argument into each 16 bit cell
//  instr argument
// fedcba9876543210
// iiiiiiaaaaaaaaaa
//   64     1024

unsigned char cell_instr(cell_t v);
cell_t cell_arg(cell_t v);

#define HEAP_SIZE 500
#define STACK_START (HEAP_SIZE-1)
#define REG_PC 0
#define REG_ST 1
#define REG_CODE_START 64

// yarn machine instructions
#define NOP   0 // do nothing

// flow control (careful: 10bit address arguments unless on stack)
#define JMP   1  // jump absolute
#define JPZ   2  // jump if top is zero
#define JSR   3  // subroutine jump - pushes pc then jumps
#define RTS   4  // return from subroutine - jump to stack top

// (so we could jmp to 16bit addr via)
// ldl16 addr
// rts
// or use some page mode thingy

// memory operators
#define LDL   5  // load literal to stack
#define LDL16 6  // load next address as 16 bit value
#define LD    7  // load contents of address to stack
#define LDI   8  // load value pointed to by contents of address
#define LDSI  9  // load value pointed to by top of stack
#define ST    10 // store value on top of stack to address
#define STI   11 // store value to location pointed to by contents of address
#define STS   12 // store stack pointer in location
#define STSI  13 // pop the address then pop the value to write to it 
#define DUP   14 // push a copy of the top value to the stack
#define DROP  15 // discard stack items
#define SWAP  16 // swap the top two items on the stack

// decision making
#define EQU  17 // push 1 if top two values are equal, 0 if not
#define LT   18 // push 1 if top two values are less than, 0 if not
#define GT   19 // push 1 if top two values are greater than, 0 if not
#define LTE  20 // push 1 if top two values are less than or equal, 0 if not
#define GTE  21 // ...
#define SLT  22 // signed versions
#define SGT  23
#define SLTE 24
#define SGTE 25

// maths
#define ADD  26
#define SUB  27
#define ADDL 28
#define SUBL 29
#define MUL  30
#define DIV  31
#define MOD  32
#define AND  33 // bitwise stuff
#define OR   34
#define XOR  35
#define NOT  36 // bitwise not
#define NOTL 37 // logical not (0/1 only)
#define RR   38 // rotate right
#define RL   39 // rotate left
#define SIN  40 // uses degrees, returns fixed point
#define COS  41 // "
#define TAN  42 // "

typedef struct {
  // used to have other things - now all stored on heap
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

// for debugging
char *yarn_opcode_text(cell_t opcode);

#endif
