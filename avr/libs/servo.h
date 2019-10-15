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

#ifndef UNIT_TEST
#include <avr/io.h>
#endif
#include "fixed.h"

#ifndef PENELOPE_SERVO
#define PENELOPE_SERVO

#define SERVO_PORT PORTC
#define SERVO_DDR DDRC
#define SERVO_NUM 3

#define SERVO_MIN 1000
#define SERVO_MAX 4500
#define SERVO_MID 2750
#define SERVO_DEG_MIN -90
#define SERVO_DEG_MAX 90

void servo_init();
void servo_pulse_update();

unsigned int servo_pulse[9];

unsigned int degrees_to_pulse(int degrees);

typedef struct {
  unsigned char id;
  int start_degrees;
  int end_degrees;
  unsigned int time; // fixed point
  unsigned int speed; // fixed point
  unsigned int amplitude; // fixed
  unsigned int bias_degrees; // degrees
  unsigned int smooth; // fixed
} servo_state;

void servo_state_init(servo_state *state, unsigned char id);
int servo_current_degrees(servo_state *state);
void servo_modify(servo_state *state, int target_degrees, unsigned int speed);
void servo_update(servo_state *state);

// to fit 32 byte radio packet (or not any more)
#define MAX_PATTERN_LENGTH 12 
#define NUM_SERVOS 3

#define MOTION_SEQ_PATTERN_ID_NULL 0
#define MOTION_SEQ_PATTERN_ID_STOPPED 1
#define MOTION_SEQ_PATTERN_ID_FORWARD 2
#define MOTION_SEQ_PATTERN_ID_BACKWARD 3
#define MOTION_SEQ_PATTERN_ID_LEFT1 4
#define MOTION_SEQ_PATTERN_ID_LEFT2 5
#define MOTION_SEQ_PATTERN_ID_LEFT3 6
#define MOTION_SEQ_PATTERN_ID_LEFT4 7
#define MOTION_SEQ_PATTERN_ID_RIGHT1 8
#define MOTION_SEQ_PATTERN_ID_RIGHT2 9
#define MOTION_SEQ_PATTERN_ID_RIGHT3 10
#define MOTION_SEQ_PATTERN_ID_RIGHT4 11
#define MOTION_SEQ_PATTERN_ID_SILLY 12

typedef struct {
  unsigned char pattern[MAX_PATTERN_LENGTH];
  unsigned int length;
  unsigned int position;
  unsigned int ms_per_step; // fixed
  unsigned int timer; // fixed
  servo_state servo[NUM_SERVOS];
  unsigned int next_pattern_id;
  unsigned int pattern_loop_count;
} servo_motion_seq;

void servo_motion_seq_init(servo_motion_seq* seq, unsigned int length);
void servo_motion_seq_pattern(servo_motion_seq* seq, char *pattern);
void servo_motion_seq_update(unsigned int delta_ms, servo_motion_seq* seq);

#endif
