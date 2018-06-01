// up to 8 servos on portB sequentially pulsed
// uses timer1 in CTC mode
// synchro gap is constant, 6 ms
// frame is variable
// min.frame = 8*1+6 = 14 ms
// max.frame = 8*2+6 = 22 ms

// Mega88, 8 MHz
#define F_CPU 8000000UL

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

// initial pulse times in us for servo 0, 1, 2...7 (last value 6000 is the synchro gap)
unsigned int servo[9] = {1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 6000};

//----------------------------------------------------------------

ISR(TIMER1_COMPA_vect)
{
  static unsigned char servo_num;
  PORTC = (1<<servo_num);          // end pulse for servo (n), start pulse for servo (n+1)            
  OCR1A = servo[servo_num];        // set width of pulse
  servo_num++;                     // prepare next servo 
  if(servo_num > 8) servo_num = 0; // again from servo 0;
}


int main (void)
{
      DDRC = 255;                        // portb output
      TCCR1B |= (1<<WGM12) | (1<<CS11);  // pwm mode 4,CTC, prescale=8
      TIMSK1 |= (1<<OCIE1A);             // enable T1_compareA interrupt 
      TCNT1 = 65530;
      sei();   

      uint32_t servo_min = 1000;
      uint32_t servo_max = 4500;
      uint32_t servo_mid = servo_min + ((servo_max-servo_min)/2);

      int32_t triangle_a = 0;
      uint8_t triangle_a_state = 0;
      int32_t triangle_b = 100;
      uint8_t triangle_b_state = 0;

      int32_t tri_max=200;
      int32_t tri_min=-200;

      int32_t mid_pos=-100;

      for(;;)                   
      {
	servo[0] = servo_mid+triangle_a;          // servos 0 and 1 left
	servo[1] = servo_mid+mid_pos;          // servos 0 and 1 left
	servo[2] = servo_mid+triangle_a;          // servos 0 and 1 left
	
	 
	 if (triangle_a_state) triangle_a-=4;
	 else triangle_a+=4;
	 if (triangle_a>tri_max) {
	   triangle_a_state=1;
	   mid_pos=400;
	 }
	 if (triangle_a<tri_min) {
	   triangle_a_state=0;
	   mid_pos=-400;
	 }

	 _delay_ms(20);
      }
}
