
#include <yarn.h>
#include <gy91.h>
#include <servo.h>

#ifndef PENELOPE_ROBOT
#define PENELOPE_ROBOT

typedef struct {
  unsigned char running;
  yarn_machine machine;
  servo_motion_seq seq;
  unsigned int last_move_type;
  unsigned int last_seq_pos;
} robot_t;

void robot_init(robot_t *r);
void robot_reset(robot_t *r);
void robot_halt(robot_t *r);
void robot_tick(robot_t *r);
void robot_update_sensors(robot_t *r);
void robot_update_servos(robot_t *r);

#endif
