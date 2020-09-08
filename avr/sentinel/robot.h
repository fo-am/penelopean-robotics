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
// change yarnemu.py + yarnc.scm compiler registers too

#define REG_ROBOT_ID 2
#define REG_LED 3
#define REG_COMP_ANGLE 4
#define REG_COMP_DELTA_RESET 5
#define REG_COMP_DELTA 6
#define REG_SERVO_STEP_COUNT 7
#define REG_SERVO_STEP_COUNT_RESET 8
#define REG_SERVO_NEXT_PATTERN_ID 9
// telemetry visible user area
#define REG_USR_A 10
#define REG_USR_B 11
#define REG_USR_C 12
#define REG_USR_D 13

#define REG_TEMPERATURE 14 
#define REG_SLEEP 15       
#define REG_I2C_DEVICE 16
#define REG_I2C_ADDR 17
#define REG_I2C_DATA 18
#define REG_I2C_CTRL 19
#define REG_ADC_1 20
#define REG_ADC_2 21
#define REG_ADC_3 22
#define REG_VCC 23

#define REG_SERVO_MS_PER_STEP 14 // <- temperature 
#define REG_SERVO_1_AMP 15       
#define REG_SERVO_2_AMP 16
#define REG_SERVO_3_AMP 17
#define REG_SERVO_1_BIAS 18
#define REG_SERVO_2_BIAS 19
#define REG_SERVO_3_BIAS 20
#define REG_SERVO_1_SMOOTH 21
#define REG_SERVO_2_SMOOTH 22
#define REG_SERVO_3_SMOOTH 23

// 24->32 user area (enough RAM for anybody!!)

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
void robot_sleep();

#endif
