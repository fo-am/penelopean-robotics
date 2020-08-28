#include <avr/io.h>
#include <util/delay.h>
#include "camera.h"
#include "adc.h"

void read_camera(unsigned int exposure, unsigned char *image) {
  // pulse clock and si
  PORTB &= ~(1<<CLK_PIN);
  PORTB |= (1<<SI_PIN);
  PORTB |= (1<<CLK_PIN);
  PORTB &= ~(1<<SI_PIN);

  PORTB &= ~(1<<CLK_PIN);
    
  for (unsigned char j=0; j<128; j++) {
    PORTB |= (1<<CLK_PIN);
    PORTB &= ~(1<<CLK_PIN);
  }
 
  for (unsigned int e=0; e<exposure; e++) {
    _delay_us(1);
  }
  
  // pulse clock and si
  PORTB |= (1<<SI_PIN);
  PORTB |= (1<<CLK_PIN);
  PORTB &= ~(1<<SI_PIN);
 
  PORTB &= ~(1<<CLK_PIN);
  
  for (unsigned char j=0; j<128; j++) {
    _delay_us(20);
    image[j]=read_adc();
    
    PORTB |= (1<<CLK_PIN);
    PORTB &= ~(1<<CLK_PIN);
  }
 
  _delay_us(20);
}
