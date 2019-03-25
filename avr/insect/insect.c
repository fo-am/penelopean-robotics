#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <servo.h>
#include <nrf24l01.h>

#define UPDATE_FREQ_HZ 10.0
#define PENELOPE_NUM_SERVOS 3

// registers

// 0x00: ping LEDs command
// 0x01: sync command 
// 0x03: servo 1 amp [fixed]
// 0x04: servo 1 bias_degrees
// 0x05: servo 1 sequence speed [fixed]
// 0x06: servo 1 smoothing [fixed]
// 0x07 - 0x0a servo 2
// 0x0b - 0x0e servo 3

// new packet for modifying registers
typedef struct {
  unsigned char id;              // 1 - not needed
  unsigned int address;
  int value; // could be fixed point in some circumstances
} register_write_packet;

// incoming data packet over the radio
// this is the original packet used for the motion sequencer
typedef struct {
  unsigned char id;              // 1 - not needed
  // [sequence speed depreciated, via registers instead]
  unsigned int speed;            // 2 (fixed -> *= 1000) 
  unsigned int length;           // 2 
  char pattern[MAX_PATTERN_LENGTH]; // 26
} seq_pattern_packet;                 // = 32 bytes


ISR(TIMER1_COMPA_vect) {
  servo_pulse_update();
}

#define RATE 20

void walk_pattern(servo_motion_seq *seq) {
  servo_motion_seq_init(0, &seq[0], 4);
  seq[0].speed=20;
  servo_motion_seq_pattern(&seq[0], "AAaa0000000000000000000000");

  servo_motion_seq_init(0, &seq[1], 4);
  seq[1].speed=20;
  servo_motion_seq_pattern(&seq[1], "bBBb0000000000000000000000");

  servo_motion_seq_init(0, &seq[2], 4);
  seq[2].speed=20;
  servo_motion_seq_pattern(&seq[2], "AAaa0000000000000000000000");
}

int main (void) {
  _delay_us(nRF24L01p_TIMING_INITIAL_US);
  nRF24L01p_init(0, 0);
  nRF24L01p_config_pipe(nRF24L01p_PIPE_0, 0xA7A7A7A7A7, 32);
  nRF24L01p_config_channel(100);
  nRF24L01p_config_transceiver_mode(nRF24L01p_VALUE_CONFIG_PRIM_RX);
  
  DDRB |= 0x01;
  PORTB |= 0x01;
  _delay_ms(1000);
  PORTB &= ~0x01;

  unsigned int i;

  servo_init();
  sei();   

  servo_motion_seq seq[PENELOPE_NUM_SERVOS];
  for (i=0; i<PENELOPE_NUM_SERVOS; i++) {  
    servo_motion_seq_init(i, &seq[i], 4);
    seq[i].speed=MAKE_FIXED(1/RATE);
    servo_motion_seq_pattern(&seq[i], "00000000000000000000000000");
  }

  /*
  servo_motion_seq_init(0, &seq[0], 4);
  seq[0].speed=40;
  servo_motion_seq_pattern(&seq[0], "AAaa0000000000000000000000");

  servo_motion_seq_init(1, &seq[1], 4);
  seq[1].speed=40;
  servo_motion_seq_pattern(&seq[1], "bBBb0000000000000000000000");

  servo_motion_seq_init(2, &seq[2], 4);
  seq[2].speed=40;
  servo_motion_seq_pattern(&seq[2], "AAaa0000000000000000000000");
  */

  char msg[32];
  // clear packet as it might be possible to 
  // parse junk on startup if nrf status is set
  // due to noise but no message is present
  memset(&msg, 0, 32);
  
  for(;;) {
    for (i=0; i<PENELOPE_NUM_SERVOS; i++) {  
      servo_motion_seq_update(&seq[i]);
    }
    
    if (nRF24L01p_read_status(nRF24L01p_PIPE_0)) {
      nRF24L01p_read((void*)msg, 32, nRF24L01p_PIPE_0);
      PORTB |= 0x01;

      char msg_type = msg[0];
      
      // msgtype id speed speed len data ...
      if (msg_type=='M') {
	//seq[message.id].speed = message.speed;
	seq_pattern_packet *p=(seq_pattern_packet *)&msg[1];
 
	int pos=0;
	for (i=0; i<PENELOPE_NUM_SERVOS; i++) {  
	  seq[i].speed = p->speed;
	  seq[i].length = p->length;
	  for (int n=0; n<p->length; n++) {
	    seq[i].pattern[n] = p->pattern[n+pos];
	  }
	  pos += p->length;
	}  
      }
      
      // update a register/carry out a command
      if (msg_type=='W') {
	register_write_packet *p = (register_write_packet*)&msg[1];	
	switch (p->address) {  
	
	  // flash LEDs
	case 0x00 : {	
	  PORTB |= 0x01;
	  _delay_ms(50);
	  PORTB &= ~0x01;
	  _delay_ms(50);
	  PORTB |= 0x01;
	  _delay_ms(50);
	  PORTB &= ~0x01;
	} break;

	  // flash LEDs slowly
	case 0x99 : {	
	  PORTB |= 0x01;
	  _delay_ms(500);
	  PORTB &= ~0x01;
	  _delay_ms(500);
	  PORTB |= 0x01;
	  _delay_ms(500);
	  PORTB &= ~0x01;
	} break;
	
	  // sync
	case 0x02: {
	  for (i=0; i<PENELOPE_NUM_SERVOS; i++) {  
	    seq[i].timer = MAKE_FIXED(1.0); // cause next update to trigger 0
	    seq[i].position = 0;
	  }
	} break;

	  // servo control registers
	case 0x03: 
	  seq[0].servo.amplitude = p->value; 

	  if (p->value!=0) {
	    PORTB |= 0x01;
	    _delay_ms(50);
	    PORTB &= ~0x01;
	    _delay_ms(50);
	    PORTB |= 0x01;
	    _delay_ms(50);
	    PORTB &= ~0x01;
	    _delay_ms(50);
	    PORTB |= 0x01;
	    _delay_ms(50);
	    PORTB &= ~0x01;
	    _delay_ms(50);
	    PORTB |= 0x01;
	    _delay_ms(50);
	    PORTB &= ~0x01;
	  }
	  break;
	case 0x04: seq[0].servo.bias_degrees = p->value; break;
	case 0x05: seq[0].servo.speed = p->value; break;
	case 0x06: seq[0].servo.smooth = p->value; break;

	case 0x07: seq[1].servo.amplitude = p->value; break;
	case 0x08: seq[1].servo.bias_degrees = p->value; break;
	case 0x09: seq[1].servo.speed = p->value; break;
	case 0x0a: seq[1].servo.smooth = p->value; break;

	case 0x0b: seq[2].servo.amplitude = p->value; break;
	case 0x0c: seq[2].servo.bias_degrees = p->value; break;
	case 0x0d: seq[2].servo.speed = p->value; break;
	case 0x0e: seq[2].servo.smooth = p->value; break;

	}	
      }
    }
    
    _delay_ms(RATE);
    PORTB &= ~0x01;
  }
}
