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
#include <avr/interrupt.h>
//#include <avr/eeprom.h>
#include <util/delay_basic.h>
#include <util/delay.h>
#include "usiTwiSlave.h"
#include "camera.h"
#include "adc.h"
#include "pattern_recognition.h"

#define I2C_ADDR 0x0a
#define IMAGE_SIZE 128
#define FEATURES_SIZE 20

unsigned char image[IMAGE_SIZE];
feature features[FEATURES_SIZE];
unsigned int num_features=0;
unsigned char counter=0;

// change constants in yarnc.scm too
#define ADDR_ID 0
#define ADDR_I2C 1 
#define ADDR_ALIVE 2
#define ADDR_EXPOSURE_LO 3 // r/w
#define ADDR_EXPOSURE_HI 4 // r/w
#define ADDR_COURSENESS 5  // r/w
#define ADDR_MIN_WIDTH 6   // r/w
#define ADDR_MAX_WIDTH 7   // r/w
#define ADDR_AVERAGE 8
#define ADDR_LINE_MIDPOS 9
#define ADDR_BARCODE_BITS 10 // r/w
#define ADDR_BARCODE_SKIP 11 // r/w
#define ADDR_BARCODE_DATA 12  
#define ADDR_LINE_ERROR 13
#define ADDR_BARCODE_ERROR 14
#define ADDR_AUTOEXPOSURE 15 // r/w
#define ADDR_NUM_FEATURES 31
#define ADDR_FEATURES_START 32
#define ADDR_RAW_IMAGE_START 127

unsigned int  g_exposure=1000;
unsigned char g_courseness=10;
unsigned char g_min_width=20;
unsigned char g_max_width=255;
unsigned char g_average=0;
unsigned char g_line_midpos=0;
unsigned char g_barcode_bits=4;
unsigned char g_barcode_skip=1;
unsigned char g_barcode_data=0;
unsigned char g_line_error=0;
unsigned char g_barcode_error=0;
unsigned char g_autoexposure=100;

uint8_t i2c_read(uint8_t reg) {
  switch (reg) {
  case ADDR_ID: return 0x1f; break; 
  case ADDR_I2C: return I2C_ADDR; break; 
  case ADDR_ALIVE: return counter++; break;
  case ADDR_EXPOSURE_LO: return g_exposure & 0xff; break;
  case ADDR_EXPOSURE_HI: return (g_exposure>>8) & 0xff; break;
  case ADDR_COURSENESS: return g_courseness; break;
  case ADDR_MIN_WIDTH: return g_min_width; break;
  case ADDR_MAX_WIDTH: return g_max_width; break;
  case ADDR_AVERAGE: return g_average; break;
  case ADDR_LINE_MIDPOS: return g_line_midpos; break;
  case ADDR_BARCODE_BITS: return g_barcode_bits; break;
  case ADDR_BARCODE_SKIP: return g_barcode_skip; break;
  case ADDR_BARCODE_DATA: return g_barcode_data; break;
  case ADDR_LINE_ERROR: return g_line_error; break;
  case ADDR_BARCODE_ERROR: return g_barcode_error; break;
  case ADDR_AUTOEXPOSURE: return g_autoexposure; break;
    // feature outputs
  case ADDR_NUM_FEATURES: return num_features; break;
  default:

    // output features
    if (reg>=ADDR_FEATURES_START && reg<3*FEATURES_SIZE) {
      unsigned int i = (reg-ADDR_FEATURES_START)/3;

      switch ((reg-ADDR_FEATURES_START)%3) {
      case 0: return features[i].start; break;
      case 1: return features[i].end; break;
      default: return features[i].width; break;
      }
    }    

    // raw image data is here...
    if (reg>=ADDR_RAW_IMAGE_START) {
      return image[reg-ADDR_RAW_IMAGE_START];
    }
    
    return 255;
  }
}

void i2c_write(uint8_t reg, uint8_t value) {
  switch (reg) {
  case ADDR_EXPOSURE_LO: g_exposure|=value; break;
  case ADDR_EXPOSURE_HI: g_exposure|=(int)(value)<<8; break;
  case ADDR_COURSENESS: g_courseness=value; break;
  case ADDR_MIN_WIDTH: g_min_width=value; break;
  case ADDR_MAX_WIDTH: g_max_width=value; break;
  case ADDR_BARCODE_BITS: g_barcode_bits=value; break;
  case ADDR_BARCODE_SKIP: g_barcode_skip=value; break;
  case ADDR_AUTOEXPOSURE: g_autoexposure=value; break;
  }
}

///////////////////////////////////////////////////////////

int main() {
  DDRB = 0x00; // all inputs
  DDRB |= (1<<CLK_PIN); // output
  DDRB |= (1<<SI_PIN); // output
  init_adc();
  usiTwiSlaveInit(I2C_ADDR, i2c_read, i2c_write);
  sei();

  while (1) {
    read_camera(g_exposure,image);

    g_average = pattern_avg(image,IMAGE_SIZE);

    // alter exposure to centre avg
    if (g_autoexposure) {
      g_exposure=camera_autoexposure(g_exposure,g_average,g_autoexposure);
    }

    num_features=pattern_to_features(image,IMAGE_SIZE,
				     g_average,
				     g_courseness,
				     g_min_width,
				     g_max_width,
				     features,FEATURES_SIZE);
    
    g_line_error=pattern_read_line(features,FEATURES_SIZE,
				   &g_line_midpos);

    g_barcode_error=pattern_read_barcode(features,FEATURES_SIZE,
					 g_barcode_skip,
					 g_barcode_bits,
					 &g_barcode_data);    
  }
}




