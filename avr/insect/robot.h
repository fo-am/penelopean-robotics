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

#include <yarn.h>
#include <gy91.h>
#include <servo.h>

#ifndef PENELOPE_ROBOT
#define PENELOPE_ROBOT

// memory map (exposed via telemetry)
// change yarn.py compiler registers too

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

// 19->32 user area (enough RAM for anybody!!)

#define REG_CODE_START 32

typedef struct {
  unsigned char id;
  unsigned char running;
  yarn_machine machine;
  servo_motion_seq seq;
  unsigned int last_move_type;
  unsigned int last_seq_pos;
} robot_t;

void robot_init(robot_t *r, unsigned char id);
void robot_reset(robot_t *r, unsigned char id);
void robot_halt(robot_t *r);
void robot_tick(unsigned int delta_ms, robot_t *r);
void robot_update_sensors(robot_t *r);
void robot_update_servos(robot_t *r);
void robot_write_ee_heap(robot_t *r);
void robot_read_ee_heap(robot_t *r);

#endif
