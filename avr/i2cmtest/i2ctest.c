#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "i2c-master.h"

static const uint8_t gy91_accel=0x68<<1;
static const uint8_t gy91_compass=0x0c<<1;
static const uint8_t gy91_pressure=0x76<<1;

int main (void) {
  DDRD |= 0xff;
  PORTD &= ~0xff;
  DDRC = 0xff;
  i2c_init();

  PORTD |= 0xff;
  _delay_ms(1000); 
  PORTD &= ~0xff; 


  while(1) {
    uint8_t t=0;
    i2c_readReg(gy91_pressure,0xd0,&t,1);
      
    if (t==0x58) {
      PORTD |= 0xff;
      _delay_ms(50);
      PORTD &= ~0xff;
      _delay_ms(50);
    }
  }
}
