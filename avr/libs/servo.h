#include <avr/io.h>
#include <fixed.h>

#ifndef PENELOPE_SERVO
#define PENELOPE_SERVO

#define SERVO_PORT PORTC
#define SERVO_DDR DDRC
#define SERVO_NUM 3

#define SERVO_MIN 1000
#define SERVO_MAX 4500
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

// to fit 32 byte radio packet
#define MAX_PATTERN_LENGTH 26 
#define NUM_SERVOS 3

#define MOTION_SEQ_PATTERN_ID_NULL 0
#define MOTION_SEQ_PATTERN_ID_STOPPED 1
#define MOTION_SEQ_PATTERN_ID_FORWARD 2
#define MOTION_SEQ_PATTERN_ID_BACKWARD 3

typedef struct {
  unsigned char pattern[MAX_PATTERN_LENGTH];
  unsigned char running;
  unsigned int length;
  unsigned int position;
  unsigned int speed; // fixed
  unsigned int timer; // fixed
  servo_state servo[NUM_SERVOS];
  unsigned int next_pattern_id;
  unsigned int pattern_loop_count;
} servo_motion_seq;

void servo_motion_seq_init(servo_motion_seq* seq, unsigned int length);
void servo_motion_seq_pattern(servo_motion_seq* seq, char *pattern);
void servo_motion_seq_update(servo_motion_seq* seq);

#endif
