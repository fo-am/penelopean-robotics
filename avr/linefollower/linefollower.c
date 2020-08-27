#include <avr/io.h>
#include <avr/interrupt.h>
//#include <avr/eeprom.h>
#include <util/delay_basic.h>
#include <util/delay.h>
#include "usiTwiSlave.h"

////////////////////////////////////////////////////////////////
// adc

// 8 bit mode, should be sufficient, using ADC2
void init_adc() {
  ADMUX =   (1 << ADLAR) |     // left shift result
            (0 << REFS1) |     // Sets ref. voltage to VCC, bit 1
            (0 << REFS0) |     // Sets ref. voltage to VCC, bit 0
            (0 << MUX3)  |     // use ADC2 for input (PB4), MUX bit 3
            (0 << MUX2)  |     // use ADC2 for input (PB4), MUX bit 2
            (1 << MUX1)  |     // use ADC2 for input (PB4), MUX bit 1
            (0 << MUX0);       // use ADC2 for input (PB4), MUX bit 0

  ADCSRA =  (1 << ADEN)  |     // Enable ADC 
            (1 << ADPS2) |     // set prescaler to 64, bit 2 
            (1 << ADPS1) |     // set prescaler to 64, bit 1 
            (0 << ADPS0);      // set prescaler to 64, bit 0  
}

char read_adc() {
  ADCSRA |= (1 << ADSC);         // start ADC measurement
  while (ADCSRA & (1 << ADSC) ); // wait till conversion complete 
  return ADCH;
}

//////////////////////////////////////////////////////////////
// camera

#define CLK_PIN PB1
#define SI_PIN PB3
char image[128];

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

///////////////////////////////////////////////////////////

#define       I2C_ADDR 0x0a
unsigned char counter=0;
unsigned int  exposure=4000;

// registers 127 - 255 image data


uint8_t i2c_read(uint8_t reg) {
  switch (reg) {
  case 0: return I2C_ADDR; break; 
  case 1: return counter++; break;
  case 2: return exposure & 0xff; break;
  case 3: return (exposure>>8) & 0xff; break;
    // todo add linefollower logic, threshold etc
  default:
    // raw image data is here
    if (reg>=127) {
      return image[reg-127];
    } else {
      return 255;
    }
  }
}

void i2c_write(uint8_t reg, uint8_t value) {
  switch (reg) {
  case 2: exposure|=value; break;
  case 3: exposure|=(int)(value)<<8; break;
  }
}

///////////////////////////////////////////////////////////

int main() {
  DDRB = 0x00; // all inputs
  DDRB |= CLK_PIN; // output
  DDRB |= SI_PIN; // output
  init_adc();
  usiTwiSlaveInit(I2C_ADDR, i2c_read, i2c_write);
  sei();
  
  while (1) {
    read_camera(exposure);
    _delay_ms(100);    
  }
}




