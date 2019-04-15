#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <util/delay_basic.h>
#include "gy91.h"

int main (void) {
  DDRB |= 0x01;
  PORTB &= ~0x01;
  DDRB = 0x01;
  i2c_init();
  if (gy91_init()) while(1) {}

  PORTB |= 0x01;
  _delay_ms(1000); 
  PORTB &= ~0x01; 

  float x,y,z;

  while(1) {
    uint8_t t=0;
    
    gy91_read_accel(&x,&y,&z);

    if (y>0.1) {
      PORTB |= 0x01;
      _delay_ms(50);
    } else {
      PORTB &= ~0x01;
    }
  }
}
