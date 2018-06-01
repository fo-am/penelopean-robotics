#include <avr/io.h>

#define SERVO_PORT PORTC
#define SERVO_DDR DDRC
#define SERVO_NUM 3

//#include <stdio.h>
//#define int short

#define SERVO_MIN 1000
#define SERVO_MAX 4500
#define SERVO_DEG_MIN -90
#define SERVO_DEG_MAX 90

void servo_init();
void servo_pulse_update();

// fixed point maths ftw
#define FIXED 1000
#define MAKE_FIXED(x) (FIXED*x)

unsigned int servo_pulse[9];

unsigned int degrees_to_pulse(int degrees);

typedef struct {
  unsigned char id;
  int start_degrees;
  int end_degrees;
  unsigned int time; // fixed point
  unsigned int speed; // fixed point
} servo_state;

void servo_state_init(servo_state *state, unsigned char id);
int servo_current_degrees(servo_state *state);
void servo_modify(servo_state *state, int target_degrees, unsigned int speed);
void servo_update(servo_state *state);

// to fit 32 byte radio packet
#define MAX_PATTERN_LENGTH 26 

typedef struct {
  unsigned char pattern[MAX_PATTERN_LENGTH];
  unsigned int length;
  unsigned int position;
  unsigned int speed; // fixed
  unsigned int timer; // fixed
  servo_state servo;
} servo_motion_seq;

void servo_motion_seq_init(unsigned char id, servo_motion_seq* seq, unsigned int length);
void servo_motion_seq_pattern(servo_motion_seq* seq, char *pattern);
void servo_motion_seq_update(servo_motion_seq* seq);
