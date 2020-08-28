#include <avr/io.h>
#include "adc.h"

void adc_init(void) {
    // AREF = AVcc
    ADMUX = (1<<REFS0);
 
    // ADC Enable and prescaler of 128
    // 16000000/128 = 125000
    ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

unsigned short adc_read(unsigned char ch) {
  // select the corresponding channel 0~7
  // ANDing with ’7′ will always keep the value
  // of ‘ch’ between 0 and 7
  ch &= 0b00000111;  // AND operation with 7
  ADMUX = (ADMUX & 0xF8)|ch; // clears the bottom 3 bits before ORing
 
  // start single convertion
  // write ’1′ to ADSC
  ADCSRA |= (1<<ADSC);
 
  // wait for conversion to complete
  // ADSC becomes ’0′ again
  // till then, run loop continuously
  while(ADCSRA & (1<<ADSC));
 
  return (ADC);
}

float internal_vcc() {
  ADMUX = (0x01 << REFS0) /* AVCC with external capacitor at AREF pin */
	| (0 << ADLAR)        /* Left Adjust Result: disabled */
	| (0x0e << MUX0)      /* Internal Reference (VBG) */; 
  
  uint16_t ADC_RES_L = 0;
  uint16_t ADC_RES_H = 0;
  while(1) {
	if (ADCSRA & (0x01 << ADIF)) { /* check if ADC conversion complete */
	  ADC_RES_L = ADCL;
	  ADC_RES_H = ADCH;
	  adc_init();
	  return ( 0x400 * 1.1 ) / (ADC_RES_L + ADC_RES_H * 0x100)  /* calculate the Vcc value */;
	}
  }
}
