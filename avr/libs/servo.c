#include "servo.h"

// initial pulse times in us for servo 0, 1, 2...7 (last value 6000 is the synchro gap)
unsigned int servo_pulse[9] = {1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 6000};

void servo_init() {
#ifndef UNIT_TEST
  //SERVO_DDR = 0xff>>(8-SERVO_NUM);
  SERVO_DDR = 0xff;
  TCCR1B |= (1<<WGM12) | (1<<CS11);  // pwm mode 4,CTC, prescale=8
  TIMSK1 |= (1<<OCIE1A);             // enable T1_compareA interrupt 
  TCNT1 = 65530;
#endif
}

void servo_pulse_update() {
#ifndef UNIT_TEST
  static unsigned char servo_num = 0;
  //if(servo_num < SERVO_NUM) {
    SERVO_PORT = (1<<servo_num);          // end pulse for servo (n), start pulse for servo (n+1)  
    //}          
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
	   degrees_to_pulse(servo_current_degrees(state)),
	   0.3);
}

void servo_motion_seq_init(unsigned char id, servo_motion_seq* seq, unsigned int length) {
  unsigned int i;
  for (i=0; i<length; i++) {
    seq->pattern[i]='0';
  }
  seq->length = length;
  seq->position = 0;
  seq->timer = 0;
  seq->speed = MAKE_FIXED(1.0);
  servo_state_init(&seq->servo, id);
}

void servo_motion_seq_pattern(servo_motion_seq* seq, char *pattern) {
  unsigned int i;
  for (i=0; i<seq->length; i++) {
    seq->pattern[i]=pattern[i];
  }
}

void servo_motion_seq_update(servo_motion_seq* seq) {
  seq->timer += seq->speed;
  if (seq->timer>=MAKE_FIXED(1.0)) {
    seq->timer = 0;

    switch (seq->pattern[seq->position]) {
    case 'D': servo_modify(&seq->servo, 90, seq->speed); break;
    case 'C': servo_modify(&seq->servo, 68, seq->speed); break;
    case 'B': servo_modify(&seq->servo, 45, seq->speed); break;
    case 'A': servo_modify(&seq->servo, 23, seq->speed); break;
    case '0': servo_modify(&seq->servo, 0, seq->speed); break;
    case 'a': servo_modify(&seq->servo, -23, seq->speed); break;
    case 'b': servo_modify(&seq->servo, -45, seq->speed); break;
    case 'c': servo_modify(&seq->servo, -68, seq->speed); break;
    case 'd': servo_modify(&seq->servo, -90, seq->speed); break;
    default: break;
    }

    seq->position++;
    if (seq->position>=seq->length) seq->position=0;

  }
  servo_update(&seq->servo);
}

#ifdef UNIT_TEST

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
  s.speed = MAKE_FIXED(0.1);

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
  servo_motion_seq_init(0, &seq, 4);
  seq.speed=10;
  servo_motion_seq_pattern(&seq, "dDdD");

  for (i=0; i<1000; i++) {
    servo_motion_seq_update(&seq);
    printf("z %d %d\n", seq.position, degrees_to_pulse(servo_current_degrees(&seq.servo)));
  }  

  printf("* %d\n",degrees_to_pulse(-90));
  printf("-> %d\n",sizeof(int));

  servo_motion_seq_init(0, &seq, 8);
  seq.speed=50;
  servo_motion_seq_pattern(&seq, "aAaAaAaA");

  int last=0;
  for (i=0; i<1000; i++) {
    servo_motion_seq_update(&seq);
    printf("%d %d\n", servo_current_degrees(&seq.servo), last-servo_pulse[0]);
    last=servo_pulse[0];
  }  
  
} 

#endif

