#include <gy91.h>
#include <servo.h>
#include <fixed.h>
#include <math.h>
#include <robot.h>

// memory map (exposed via telemetry)

#define REG_PC_MIRROR 0 
#define REG_STACK_MIRROR 1 
#define REG_LED 2
#define REG_COMP_ANGLE 3
#define REG_COMP_DELTA_RESET 4
#define REG_COMP_DELTA 5
#define REG_SERVO_STEP_COUNT 6
#define REG_SERVO_STEP_COUNT_RESET 7
#define REG_SERVO_NEXT_PATTERN_ID 8
#define REG_SERVO_SPEED 9
#define REG_SERVO_1_AMP 10
#define REG_SERVO_2_AMP 11
#define REG_SERVO_3_AMP 12
#define REG_SERVO_1_BIAS 13
#define REG_SERVO_2_BIAS 14
#define REG_SERVO_3_BIAS 15
#define REG_SERVO_1_SMOOTH 16
#define REG_SERVO_2_SMOOTH 17
#define REG_SERVO_3_SMOOTH 18

// 19->32 user area (enough RAM for anybody!!)

#define REG_CODE_START 32

void robot_init(robot_t *r) {
  yarn_init(&r->machine);
  robot_reset(r);
}

void robot_reset(robot_t *r) {
  r->running=1;
  r->machine.m_pc=REG_CODE_START;
  // init walk pattern (running starts as 'off')
  servo_motion_seq_init(&r->seq, 4);
  r->seq.speed=MAKE_FIXED(20);
  // temp, start walking (default next pattern is null so should stick)
  servo_motion_seq_pattern(&r->seq, "AAaabBBbAAaa00000000000000");

  // non-zero defaults
  yarn_poke(&r->machine,REG_SERVO_SPEED,MAKE_FIXED(0.02)); // 20?
  yarn_poke(&r->machine,REG_SERVO_1_AMP,MAKE_FIXED(1.0));
  yarn_poke(&r->machine,REG_SERVO_2_AMP,MAKE_FIXED(1.0));
  yarn_poke(&r->machine,REG_SERVO_3_AMP,MAKE_FIXED(1.0));
  yarn_poke(&r->machine,REG_SERVO_1_SMOOTH,MAKE_FIXED(0.2));
  yarn_poke(&r->machine,REG_SERVO_2_SMOOTH,MAKE_FIXED(0.2));
  yarn_poke(&r->machine,REG_SERVO_3_SMOOTH,MAKE_FIXED(0.2));
}

void robot_halt(robot_t *r) {
  r->running=0;
}

void robot_tick(robot_t *r) {
  if (r->running) {
    // update registers
    // for debugging, but pc should prob be an internal register anyway
    yarn_poke(&r->machine,REG_PC_MIRROR,r->machine.m_pc);
    yarn_poke(&r->machine,REG_STACK_MIRROR,r->machine.m_stack_pos);
    robot_update_sensors(r);
    robot_update_servos(r);
    yarn_run(&r->machine);
    servo_motion_seq_update(&r->seq);
    // our ONE led - getting a bit tiresome
    if (yarn_peek(&r->machine,REG_LED)!=0) {
      PORTB |= 0x01;
    } else {
      PORTB &= ~0x01;
    }
  }
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
  yarn_poke(m,REG_COMP_ANGLE,(short)angle);
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

  seq->next_pattern_id = yarn_peek(m,REG_SERVO_NEXT_PATTERN_ID);
  
  if (yarn_peek(m,REG_SERVO_STEP_COUNT_RESET)==1) {
    yarn_poke(m,REG_SERVO_STEP_COUNT_RESET,0);
    seq->pattern_loop_count=0;
  }
  yarn_poke(m,REG_SERVO_STEP_COUNT,seq->pattern_loop_count);

}


