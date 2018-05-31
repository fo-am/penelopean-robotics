#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <servo.h>
#include <nrf24l01.h>

#define UPDATE_FREQ_HZ 10.0

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

  servo_motion_seq seq;
  servo_motion_seq_init(0, &seq, 4);
  seq.speed=MAKE_FIXED(1/UPDATE_FREQ_HZ);
  servo_motion_seq_pattern(&seq, "0AaA");

  unsigned char message[32];

  for(;;) {
    servo_motion_seq_update(&seq);

    if (nRF24L01p_read_status(nRF24L01p_PIPE_0))
    {
      nRF24L01p_read(message, 32, nRF24L01p_PIPE_0);
      
      if (message[0]=='H' && 
	  message[1]=='E' && 
	  message[2]=='L' && 
	  message[3]=='O') {
	PORTB |= 0x01;
	_delay_ms(100);
	PORTB &= ~0x01;
      }
    }

    _delay_ms((1/UPDATE_FREQ_HZ)*1000);
  }
}
