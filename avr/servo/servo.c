#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>

int main(void) {
  DDRB |= 1 << PINB1; // Set pin 9 on arduino to output

  /* 1. Set Fast PWM mode 14: set WGM11, WGM12, WGM13 to 1 */
  /* 3. Set pre-scaler of 8 */
  /* 4. Set Fast PWM non-inverting mode */
  TCCR1A |= (1 << WGM11) | (1 << COM1A1);
  TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11);

  /* 2. Set ICR1 register: PWM period */
  ICR1 = 24999;

  /* 5. Set duty cycle */
  while(1) {
    OCR1A = 4899;

    _delay_ms(5000);

    OCR1A = 1199;

    _delay_ms(5000);
  }

  return 0;
}
