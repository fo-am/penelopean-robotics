#include <avr/io.h>
#include <util/delay.h>
#include "camera.h"

void read_camera(int exposure) {
  // pulse clock and si
  PORTB &= ~CLK_PIN;
  PORTB |= SI_PIN;
  PORTB |= CLK_PIN;
  PORTB &= ~SI_PIN;

  PORTB &= ~CLK_PIN;
    
  for (unsigned char j=0; j<128; j++) {
    PORTB |= CLK_PIN;
    PORTB &= ~CLK_PIN;
  }
 
  for (unsigned char j=0; j<exposure; j++) {
    _delay_ms(1);
  }
  
  PORTB |= SI_PIN;
  PORTB |= CLK_PIN;
  PORTB &= ~SI_PIN;
 
  PORTB &= ~CLK_PIN;
    
  for (unsigned char j=0; j<128; j++) {
    _delay_ms;
    PORTB &= ~CLK_PIN;
  }
 
  _delay_ms(20);
}
