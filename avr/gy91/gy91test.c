#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <util/delay_basic.h>
#include "gy91.h"

int main (void) {
  DDRD |= 0xff;
  PORTD &= ~0xff;
  DDRC = 0xff;
  i2c_init();
  if (gy91_init()) while(1) {}

  PORTD |= 0xff;
  _delay_ms(1000); 
  PORTD &= ~0xff; 

  float x,y,z;

  while(1) {
    uint8_t t=0;
    
    gy91_read_accel(&x,&y,&z);

    if (y>0.1) {
      PORTD |= 0xff;
      _delay_ms(50);
    } else {
      PORTD &= ~0xff;
    }
  }
}
