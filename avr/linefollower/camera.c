// Penelopean Robotics Copyright (C) 2020 FoAM Kernow
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <avr/io.h>
#include <util/delay.h>
#include "camera.h"
#include "adc.h"

unsigned int camera_autoexposure(unsigned int exposure, unsigned char average) {
  // increase the exposure if it's dark on average and vice versa
  int new_exposure=(int)(exposure)-((int)(average)-127);
  if (exposure<0) exposure=0;
  if (exposure>4000) exposure=4000;
  return exposure;
}


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
