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
unsigned char counter=0;
unsigned int  exposure=4000;
#define image_size 128
unsigned char image[image_size];
#define features_size 20
feature features[features_size];
unsigned int num_features=0;

// registers 127 - 255 image data

uint8_t i2c_read(uint8_t reg) {
  switch (reg) {
  case 0: return I2C_ADDR; break; 
  case 1: return counter++; break;
  case 2: return exposure & 0xff; break;
  case 3: return (exposure>>8) & 0xff; break;  
    // todo add linefollower logic, threshold etc
  case 31: return num_features; break;
  default:
    // features
    if (reg>=32 && reg<127) {
      unsigned int feature_i = (reg-32)/3;
      unsigned int i = (reg-32)%3;
      switch (i) {
      case 0: return features[feature_i].start; break;
      case 1: return features[feature_i].end; break;
      default: return features[feature_i].width; break;
      }
    }
    
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
    //read_camera(exposure,image);
    for (unsigned int n=0; n<image_size; n++) {
      unsigned char v = 60;
      if (n>20 && n<33) v=200;
      if (n>80 && n<90) v=20;
      if (n>100 && n<112) v=180;
      image[n]=rand()%20+v;
    }
    
    unsigned char avg = pattern_avg(image,image_size);
    // todo alter exposure to centre avg
    num_features=pattern_to_features(image,image_size,avg,features,features_size); 
  }
}




