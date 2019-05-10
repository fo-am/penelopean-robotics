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

#include "servo.h"

// initial pulse times in us for servo 0, 1, 2...7 (last value 6000 is the synchro gap)
unsigned int servo_pulse[9] = {SERVO_MID, SERVO_MID, SERVO_MID, 1500, 1500, 1500, 1500, 1500, 6000};

void servo_init() {
#ifndef UNIT_TEST
  //SERVO_DDR = 0xff>>(8-SERVO_NUM);
  SERVO_DDR = 0x07; 
  TCCR1B |= (1<<WGM12) | (1<<CS11);  // pwm mode 4,CTC, prescale=8
  TIMSK1 |= (1<<OCIE1A);             // enable T1_compareA interrupt 
  TCNT1 = 65530;
#endif
}

void servo_pulse_update() {
#ifndef UNIT_TEST
  static unsigned char servo_num = 0;
  if(servo_num < 4) { // leave space for i2c
    SERVO_PORT = (1<<servo_num);          // end pulse for servo (n), start pulse for servo (n+1)  
  }          
  OCR1A = servo_pulse[servo_num];        // set width of pulse
  servo_num++;                     // prepare next servo 
  if(servo_num > 8) servo_num = 0; // again from servo 0;
#endif
}

////////////////////////////////////////////////////////////

unsigned int degrees_to_pulse(int degrees) {
  unsigned int range = SERVO_DEG_MAX-SERVO_DEG_MIN;
  //unsigned int t = ((degrees-SERVO_DEG_MIN)*FIXED)/range;
  //return SERVO_MIN+(t*(SERVO_MAX-SERVO_MIN))/FIXED;
  float t = ((degrees-SERVO_DEG_MIN))/(float)range;
  return SERVO_MIN+(t*(SERVO_MAX-SERVO_MIN));
} 

void servo_state_init(servo_state *state, unsigned char id) {
  state->id = id;
  state->start_degrees = 0;
  state->end_degrees = 0;
  state->time = 0;
  state->speed = 0;
  state->amplitude = MAKE_FIXED(1.0);
  state->bias_degrees = 0;
  state->smooth = MAKE_FIXED(0.2);
}

int servo_current_degrees(servo_state *state) {
  // one day I'll figure out fixed point...
  float ftime = state->time/(float)FIXED;
  return (state->start_degrees * (1.0-ftime) +
	  state->end_degrees * ftime);
}

// start moving (or interrupt a current move) with 
// a new target position and speed
void servo_modify(servo_state *state, int target_degrees, unsigned int speed) {
  state->start_degrees = state->end_degrees; //servo_current_degrees(state);
  state->end_degrees = target_degrees;
  state->time = 0;
  state->speed = speed;
}

int smooth(int a, int b,float v) {
  return a*(1.0-v)+b*v;
}

void servo_update(servo_state *state) {
  if (state->time >= MAKE_FIXED(1.0)) {
    state->time = MAKE_FIXED(1.0);
  } else {
    state->time += state->speed;
  }  
  servo_pulse[state->id] = 
    smooth(servo_pulse[state->id],
	   degrees_to_pulse((servo_current_degrees(state)*
			     FIXED_TO_FLOAT(state->amplitude))+
			    state->bias_degrees),
	   FIXED_TO_FLOAT(state->smooth));
}

///////////////////////////////////////////

void servo_motion_seq_init(servo_motion_seq* seq, unsigned int length) {
  unsigned int i;
  for (i=0; i<length*NUM_SERVOS; i++) {
    seq->pattern[i]='0';
  }
  seq->length = length;
  seq->position = 0;
  seq->ms_per_step = 500;
  seq->timer = 0;
  for (unsigned int s=0; s<NUM_SERVOS; s++) {
    servo_state_init(&seq->servo[s], s);
  }
  seq->next_pattern_id = MOTION_SEQ_PATTERN_ID_STOPPED;
  seq->pattern_loop_count = 0;
}

void servo_motion_seq_pattern(servo_motion_seq* seq, char *pattern) {
  unsigned int i;
  for (i=0; i<seq->length*NUM_SERVOS; i++) {
    seq->pattern[i]=pattern[i];
  }
}

// higher level pattern control... preset patterns for walking
void servo_motion_seq_load_next_pattern(servo_motion_seq *seq) {
  switch (seq->next_pattern_id) {
  case MOTION_SEQ_PATTERN_ID_NULL: {
    // skip
  } break;
  case MOTION_SEQ_PATTERN_ID_STOPPED: {
    servo_motion_seq_pattern(seq, "00000000000000000000000000");
  } break;
  case MOTION_SEQ_PATTERN_ID_FORWARD: {
    servo_motion_seq_pattern(seq, "AAaaAaaAAAaa00000000000000");
  } break;
  case MOTION_SEQ_PATTERN_ID_BACKWARD: {
    servo_motion_seq_pattern(seq, "AAaaaAAaAAaa00000000000000");
  } break;
  default: {

  } break;
  }
}

void servo_motion_seq_update(unsigned int delta_ms, servo_motion_seq* seq) {
  seq->timer += delta_ms;
  if (seq->timer>=seq->ms_per_step) {
    seq->timer = 0;    
    unsigned int servo_speed = MAKE_FIXED(delta_ms/(float)seq->ms_per_step);
    
    for (unsigned int s=0; s<NUM_SERVOS; s++) {
      switch (seq->pattern[seq->position+(s*seq->length)]) {
      case 'D': servo_modify(&seq->servo[s], 90, servo_speed); break;
      case 'C': servo_modify(&seq->servo[s], 68, servo_speed); break;
      case 'B': servo_modify(&seq->servo[s], 45, servo_speed); break;
      case 'A': servo_modify(&seq->servo[s], 23, servo_speed); break;
      case '0': servo_modify(&seq->servo[s], 0, servo_speed); break;
      case 'a': servo_modify(&seq->servo[s], -23, servo_speed); break;
      case 'b': servo_modify(&seq->servo[s], -45, servo_speed); break;
      case 'c': servo_modify(&seq->servo[s], -68, servo_speed); break;
      case 'd': servo_modify(&seq->servo[s], -90, servo_speed); break;
      default: break;
      }
    }

    seq->position++;
    if (seq->position>=seq->length) {
      seq->position=0;	
      seq->pattern_loop_count++;
      // time to load in next pattern (if it's not null)
      servo_motion_seq_load_next_pattern(seq);
    }
  }
  servo_update(&seq->servo[0]);
  servo_update(&seq->servo[1]);
  servo_update(&seq->servo[2]);
}

#ifdef UNIT_TEST
#include <stdio.h>

void main() {
  unsigned int i;
  printf("* %d\n",degrees_to_pulse(-90));
  printf("* %d\n",degrees_to_pulse(0));
  printf("* %d\n",degrees_to_pulse(90));
  printf("* %d\n",degrees_to_pulse(45));

  servo_state s;
  s.id = 0;
  s.start_degrees=-90;
  s.end_degrees=90;
  s.time = MAKE_FIXED(0.5);
  s.speed = 10;

  printf("x %d\n",servo_current_degrees(&s));

  for (i=0; i<10; i++) {
    servo_update(&s);
    printf("y %d\n", servo_pulse[s.id]);
  }

  s.time = MAKE_FIXED(1);
  s.start_degrees=23;
  s.end_degrees=-23;

  printf("x %d\n",servo_current_degrees(&s));

  for (i=0; i<10; i++) {
    servo_update(&s);
    printf("yy %d\n", servo_pulse[s.id]);
  }

  servo_motion_seq seq;
  servo_motion_seq_init(&seq, 4);
  seq.ms_per_step=1000;
  servo_motion_seq_pattern(&seq, "dDdDdDdDdDdD");
  seq.next_pattern_id=MOTION_SEQ_PATTERN_ID_NULL;

  for (i=0; i<1000; i++) {
    servo_motion_seq_update(20,&seq);
    printf("z %d %c %d\n", seq.position, seq.pattern[seq.position], degrees_to_pulse(servo_current_degrees(&seq.servo[0])));
  }  

  printf("* %d\n",degrees_to_pulse(-90));
  printf("-> %d\n",(int)sizeof(int));

  servo_motion_seq_init(&seq, 8);
  seq.ms_per_step=500;
  servo_motion_seq_pattern(&seq, "aAaAaAaAaAaAaAaAaAaAaAaA");
  seq.next_pattern_id=MOTION_SEQ_PATTERN_ID_NULL;

  int last=0;
  for (i=0; i<1000; i++) {
    servo_motion_seq_update(20,&seq);
    printf("%d %d\n", servo_current_degrees(&seq.servo[0]), last-servo_pulse[0]);
    last=servo_pulse[0];
  }  
  
} 

#endif

