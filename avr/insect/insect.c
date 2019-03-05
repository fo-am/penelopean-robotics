#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <servo.h>
#include <nrf24l01.h>

#define UPDATE_FREQ_HZ 10.0
#define PENELOPE_NUM_SERVOS 3

// incoming data packet over the radio
// todo - use type to dispatch to different message types
typedef struct {
  unsigned char type;            // 1
  unsigned char id;              // 1
  unsigned int speed;            // 2 (fixed -> *= 1000)
  unsigned int length;           // 2 
  char pattern[MAX_PATTERN_LENGTH]; // 26
} message_packet;                 // = 32 bytes

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

  servo_motion_seq_init(0, &seq[0], 4);
  seq[0].speed=40;
  servo_motion_seq_pattern(&seq[0], "AAaa0000000000000000000000");

  servo_motion_seq_init(1, &seq[1], 4);
  seq[1].speed=40;
  servo_motion_seq_pattern(&seq[1], "bBBb0000000000000000000000");

  servo_motion_seq_init(2, &seq[2], 4);
  seq[2].speed=40;
  servo_motion_seq_pattern(&seq[2], "AAaa0000000000000000000000");

  message_packet message;
  // clear packet as it might be possible to 
  // parse junk on startup if nrf status is set
  // due to noise but no message is present
  memset(&message, 0, sizeof(message_packet));
  
  unsigned int safepulse=SERVO_MIN;
  char dir=0;

  for(;;) {
    for (i=0; i<PENELOPE_NUM_SERVOS; i++) {  
      servo_motion_seq_update(&seq[i]);
    }

    //if (dir==0) safepulse+=20;
    //else safepulse-=20;
    //servo_pulse[0]=safepulse;
    ////servo_pulse[0]=SERVO_MIN+(SERVO_MAX-SERVO_MIN)/2;
    //if (safepulse>SERVO_MAX) dir=1;
    //if (safepulse<SERVO_MIN) dir=0;
    
    if (nRF24L01p_read_status(nRF24L01p_PIPE_0)) {
      nRF24L01p_read(&message, 32, nRF24L01p_PIPE_0);
      PORTB |= 0x01;
      
      if (message.type=='H') {
	PORTB |= 0x01;
	_delay_ms(50);
	PORTB &= ~0x01;
	_delay_ms(50);
	PORTB |= 0x01;
	_delay_ms(50);
	PORTB &= ~0x01;
      }

      // msgtype id speed speed len data ...
      if (message.type=='M') {
	//seq[message.id].speed = message.speed;
	int pos=0;
	for (i=0; i<PENELOPE_NUM_SERVOS; i++) {  
	  seq[i].speed = message.speed;
	  seq[i].length = message.length;
	  for (int n=0; n<message.length; n++) {
	    seq[i].pattern[n] = message.pattern[n+pos];
	  }
	  pos += message.length;
	}  
      }

      // sync
      if (message.type=='S') {
	for (i=0; i<PENELOPE_NUM_SERVOS; i++) {  
	  seq[i].timer = MAKE_FIXED(1.0); // cause next update to trigger 0
	  seq[i].position = 0;
	}
      }

    }
    
    _delay_ms(RATE);
    PORTB &= ~0x01;
  }
}
