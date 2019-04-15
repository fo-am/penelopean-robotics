
#include <yarn.h>
#include <gy91.h>
#include <servo.h>

#ifndef PENELOPE_ROBOT
#define PENELOPE_ROBOT

typedef struct {
  yarn_machine machine;
  servo_motion_seq seq;
} robot_t;

void robot_init(robot_t *r);
void robot_tick(robot_t *r);
void robot_update_sensors(robot_t *r);
void robot_update_servos(robot_t *r);

#endif
