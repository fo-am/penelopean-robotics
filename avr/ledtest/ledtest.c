#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

int main (void) {

  DDRB |= 0xff;
  DDRC |= 0xff;
  DDRD |= 0xff;
  PORTB |= 0xff;
  PORTC |= 0xff;
  PORTD |= 0xff;
  _delay_ms(1000);
  PORTB &= ~0xff;
  PORTC &= ~0xff;
  PORTD &= ~0xff;
  
  for(;;) {
    PORTB |= 0xff;
    PORTC |= 0xff;
    PORTD |= 0xff;
    _delay_ms(50);
    PORTB &= ~0xff;
    PORTC &= ~0xff;
    PORTD &= ~0xff;
    _delay_ms(50);
    PORTB |= 0xff;
    PORTC |= 0xff;
    PORTD |= 0xff;
    _delay_ms(50);
    PORTB &= ~0xff;
    PORTC &= ~0xff;
    PORTD &= ~0xff;
    _delay_ms(1000);   
  }
}
