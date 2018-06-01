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
  char pattern[MAX_PATTERN_LENGTH]; // 10
} message_packet;                 // = 16 bytes

ISR(TIMER1_COMPA_vect) {
  servo_pulse_update();
}

#define RATE 20

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
    servo_motion_seq_pattern(&seq[i], "A0a0");
  }

  message_packet message;
  byte str[96];

  unsigned int safepulse = 1000;

  for(;;) {
    for (i=0; i<PENELOPE_NUM_SERVOS; i++) {  
      servo_motion_seq_update(&seq[i]);
    }

    /* if (safepulse&0x20) servo_pulse[0]=degrees_to_pulse(90);  */
    /* else servo_pulse[0]=degrees_to_pulse(0);  */
    /* safepulse+=1; */
    /* if (safepulse>4500) safepulse=1000; */


    if (nRF24L01p_read_status(nRF24L01p_PIPE_0)) {
      nRF24L01p_read(str, 32, nRF24L01p_PIPE_0);
      
      if (str[0]=='H' && 
	  str[1]=='E' && 
	  str[2]=='L' && 
	  str[3]=='O') {
	PORTB |= 0x01;
	_delay_ms(100);
	PORTB &= ~0x01;
      }

      // msgtype id speed speed len data ...
      /*      if (message.type=='M') {
	
	seq[message.id].speed = message.speed;
	seq[message.id].length = message.length;
	for (i=0; i<seq[message.id].length; i++) {
	  seq[message.id].pattern[i] = message.pattern[i];
	}	  
	}   */   
    }
    
    _delay_ms(RATE);
  }
}
