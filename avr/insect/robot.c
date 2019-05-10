// Penelopean Robotics Copyright (C) 2019 FoAM Kernow
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

#include <gy91.h>
#include <servo.h>
#include <fixed.h>
#include <math.h>
#include <robot.h>
#include <avr/eeprom.h>

// only have 1k of eeprom to play with...
// make sure it's init to zero with funky gcc stuff
// probably is zero anyway, but it makes the eep file so...
cell_t EEMEM ee_heap[HEAP_SIZE]={ [0 ... HEAP_SIZE-1] = 0 };

// yarn.asm: wait - forward - wait - back (1st version running)
//cell_t compiled_code[] = {6, 9, 3, 32, 5, 1, 8, 7, 5, 2, 8, 8, 6, 6, 5, 8, 14, 3, 44, 5, 1, 8, 8, 5, 0, 8, 9, 0, 6, 9, 3, 60, 5, 3, 8, 8, 5, 1, 8, 7, 6, 6, 5, 8, 14, 3, 72, 5, 1, 8, 8, 5, 0, 8, 9, 1, 32 };
//unsigned int compiled_code_len = 57;

// weft-simple.asm: wait - forward - wait - back (1st version running)
cell_t compiled_code[] = {6, 9, 3, 32, 5, 1, 8, 7, 5, 2, 8, 8, 6, 6, 5, 8, 14, 3, 44, 5, 1, 8, 8, 5, 0, 8, 9, 1, 32};
unsigned int compiled_code_len = 29;

void robot_init(robot_t *r, unsigned char id) {
  yarn_init(&r->machine);
  robot_reset(r,id);
}

void robot_reset(robot_t *r, unsigned char id) {
  r->running=1;
  r->id=id;
  r->machine.m_pc=REG_CODE_START;
  // set up motion sequencer for default walk patterns
  servo_motion_seq_init(&r->seq, 4);
  // 500 ms = 120 bpm
  r->seq.ms_per_step=500;

  // non-zero defaults
  yarn_poke(&r->machine,REG_SERVO_MS_PER_STEP,500); 
  yarn_poke(&r->machine,REG_SERVO_1_AMP,MAKE_FIXED(1.0));
  yarn_poke(&r->machine,REG_SERVO_2_AMP,MAKE_FIXED(1.0));
  yarn_poke(&r->machine,REG_SERVO_3_AMP,MAKE_FIXED(1.0));
  yarn_poke(&r->machine,REG_SERVO_1_SMOOTH,MAKE_FIXED(0.2));
  yarn_poke(&r->machine,REG_SERVO_2_SMOOTH,MAKE_FIXED(0.2));
  yarn_poke(&r->machine,REG_SERVO_3_SMOOTH,MAKE_FIXED(0.2));

  for (unsigned int i=0; i<compiled_code_len; i++) {
    yarn_poke(&r->machine,32+i,compiled_code[i]);
  }

}

void robot_halt(robot_t *r) {
  r->running=0;
}

void robot_tick(unsigned int delta_ms, robot_t *r) {
  if (r->running) {
    // update registers
    // for debugging, but pc should prob be an internal register anyway
    yarn_poke(&r->machine,REG_ROBOT_ID,r->id);
    yarn_poke(&r->machine,REG_PC_MIRROR,r->machine.m_pc);
    robot_update_sensors(r);
    robot_update_servos(r);
    yarn_run(&r->machine);
    // our ONE led - getting a bit tiresome
    if (yarn_peek(&r->machine,REG_LED)!=0) {
      PORTB |= 0x01;
    } else {
      PORTB &= ~0x01;
    }
    servo_motion_seq_update(delta_ms,&r->seq);
  }
}

void robot_write_ee_heap(robot_t *r) {
  for (int n=0; n<HEAP_SIZE; n++) {
    ee_heap[n]=r->machine.m_heap[n];
  }
}

void robot_read_ee_heap(robot_t *r) {
  for (int n=0; n<HEAP_SIZE; n++) {
    r->machine.m_heap[n]=ee_heap[n];
  }
}

float angle_compare(float a1, float a2) {
  return 180 - fabs(fabs(a1 - a2) - 180);
}

void robot_update_sensors(robot_t *r) {
  yarn_machine *m = &r->machine;
  float x,y,z;
/*  gy91_read_accel(&x,&y,&z);
  yarn_poke(m,REG_ACCEL_X,MAKE_FIXED(x));
  yarn_poke(m,REG_ACCEL_Y,MAKE_FIXED(y));
  yarn_poke(m,REG_ACCEL_Z,MAKE_FIXED(z));
  gy91_read_gyro(&x,&y,&z);
  yarn_poke(m,REG_GYRO_X,MAKE_FIXED(x));
  yarn_poke(m,REG_GYRO_Y,MAKE_FIXED(y));
  yarn_poke(m,REG_GYRO_Z,MAKE_FIXED(z));*/
  gy91_read_mag(&x,&y,&z);
/*  yarn_poke(m,REG_COMP_X,MAKE_FIXED(x/100.0f));
  yarn_poke(m,REG_COMP_Y,MAKE_FIXED(y/100.0f));
  yarn_poke(m,REG_COMP_Z,MAKE_FIXED(z/100.0f));*/
  float angle=atan2(y,x)*57.2958;
  angle+=180; // set range to 0-360
  yarn_poke(m,REG_COMP_ANGLE,(short)angle);
  // check and clear flag
  static float compare_angle=0;
  if (yarn_peek(m,REG_COMP_DELTA_RESET)==1) {
    yarn_poke(m,REG_COMP_DELTA_RESET,0);
    compare_angle=angle;
  }
  yarn_poke(m,REG_COMP_DELTA,(short)angle_compare(compare_angle,angle));
}

void robot_update_servos(robot_t *r) {
  yarn_machine *m = &r->machine;
  servo_motion_seq *seq = &r->seq;
  
  seq->ms_per_step = yarn_peek(m,REG_SERVO_MS_PER_STEP);

  // update servo movements
  seq->servo[0].amplitude = yarn_peek(m,REG_SERVO_1_AMP);
  seq->servo[0].bias_degrees = yarn_peek(m,REG_SERVO_1_BIAS);
  seq->servo[0].smooth = yarn_peek(m,REG_SERVO_1_SMOOTH);
  seq->servo[1].amplitude = yarn_peek(m,REG_SERVO_2_AMP);
  seq->servo[1].bias_degrees = yarn_peek(m,REG_SERVO_2_BIAS);
  seq->servo[1].smooth = yarn_peek(m,REG_SERVO_2_SMOOTH);
  seq->servo[2].amplitude = yarn_peek(m,REG_SERVO_3_AMP);
  seq->servo[2].bias_degrees = yarn_peek(m,REG_SERVO_3_BIAS);
  seq->servo[2].smooth = yarn_peek(m,REG_SERVO_3_SMOOTH);

  seq->next_pattern_id = yarn_peek(m,REG_SERVO_NEXT_PATTERN_ID);
  //seq->next_pattern_id = MOTION_SEQ_PATTERN_ID_FORWARD;

  if (yarn_peek(m,REG_SERVO_STEP_COUNT_RESET)==1) {
    yarn_poke(m,REG_SERVO_STEP_COUNT_RESET,0);
    seq->pattern_loop_count=0;
  }
  yarn_poke(m,REG_SERVO_STEP_COUNT,seq->pattern_loop_count);

}


