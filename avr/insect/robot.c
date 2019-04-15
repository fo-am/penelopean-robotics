

#include <gy91.h>
#include <servo.h>
#include <fixed.h>
#include <math.h>
#include <robot.h>


// memory map (exposed via telemetry)
#define REG_PC_MIRROR 0 
#define REG_ACCEL_X 1 
#define REG_ACCEL_Y 2
#define REG_ACCEL_Z 3
#define REG_GYRO_X 4 
#define REG_GYRO_Y 5
#define REG_GYRO_Z 6
#define REG_COMP_X 7 
#define REG_COMP_Y 8
#define REG_COMP_Z 9
#define REG_COMP_ANGLE 10 
#define REG_COMP_DELTA_RESET 11 
#define REG_COMP_DELTA 12

#define REG_SERVO_MOVE 13
#define REG_SERVO_DIR 14
#define REG_SERVO_SPEED 15
#define REG_SERVO_1_AMP 16
#define REG_SERVO_2_AMP 17
#define REG_SERVO_3_AMP 18
#define REG_SERVO_1_BIAS 19
#define REG_SERVO_2_BIAS 20
#define REG_SERVO_3_BIAS 21
#define REG_SERVO_1_SMOOTH 22
#define REG_SERVO_2_SMOOTH 23
#define REG_SERVO_3_SMOOTH 24
#define REG_LED 25
// 26->32 user telemetry vars

void robot_init(robot_t *r) {
  yarn_init(&r->machine);
  // init walk pattern (running starts as 'off')
  servo_motion_seq_init(&r->seq, 4);
  r->seq.speed=MAKE_FIXED(20);
  servo_motion_seq_pattern(&r->seq, "AAaabBBbAAaa00000000000000");
}

void robot_tick(robot_t *r) {
  robot_update_sensors(r);
  robot_update_servos(r);
  yarn_run(&r->machine);
  servo_motion_seq_update(&r->seq);
}


void robot_update_sensors(robot_t *r) {
  yarn_machine *m = &r->machine;
  // update registers
  yarn_poke(m,REG_PC_MIRROR,m->m_pc);
  float x,y,z;
  gy91_read_accel(&x,&y,&z);
  yarn_poke(m,REG_ACCEL_X,MAKE_FIXED(x));
  yarn_poke(m,REG_ACCEL_Y,MAKE_FIXED(y));
  yarn_poke(m,REG_ACCEL_Z,MAKE_FIXED(z));
  gy91_read_gyro(&x,&y,&z);
  yarn_poke(m,REG_GYRO_X,MAKE_FIXED(x));
  yarn_poke(m,REG_GYRO_Y,MAKE_FIXED(y));
  yarn_poke(m,REG_GYRO_Z,MAKE_FIXED(z));
  gy91_read_mag(&x,&y,&z);
  yarn_poke(m,REG_COMP_X,MAKE_FIXED(x));
  yarn_poke(m,REG_COMP_Y,MAKE_FIXED(y));
  yarn_poke(m,REG_COMP_Z,MAKE_FIXED(z));
  float angle=atan2(y,x)*57.2958;
  yarn_poke(m,REG_COMP_ANGLE,MAKE_FIXED(angle));
  // check and clear flag
  static float comp_angle=0;
  if (yarn_peek(m,REG_COMP_DELTA_RESET)==1) {
    yarn_poke(m,REG_COMP_DELTA_RESET,0);
    comp_angle=angle;
  }
  yarn_poke(m,REG_COMP_DELTA,MAKE_FIXED(comp_angle-angle));
}

void robot_update_servos(robot_t *r) {
  yarn_machine *m = &r->machine;
  servo_motion_seq *seq = &r->seq;

  // update servo movements
  seq->servo[0].amplitude = yarn_peek(m,REG_SERVO_1_AMP);
  seq->servo[0].bias_degrees = yarn_peek(m,REG_SERVO_1_BIAS);
  seq->servo[0].speed = yarn_peek(m,REG_SERVO_SPEED);
  seq->servo[0].smooth = yarn_peek(m,REG_SERVO_1_SMOOTH);

  seq->servo[1].amplitude = yarn_peek(m,REG_SERVO_2_AMP);
  seq->servo[1].bias_degrees = yarn_peek(m,REG_SERVO_2_BIAS);
  seq->servo[1].speed = yarn_peek(m,REG_SERVO_SPEED);
  seq->servo[1].smooth = yarn_peek(m,REG_SERVO_2_SMOOTH);

  seq->servo[2].amplitude = yarn_peek(m,REG_SERVO_3_AMP);
  seq->servo[2].bias_degrees = yarn_peek(m,REG_SERVO_3_BIAS);
  seq->servo[2].speed = yarn_peek(m,REG_SERVO_SPEED);
  seq->servo[2].smooth = yarn_peek(m,REG_SERVO_3_SMOOTH);
}


