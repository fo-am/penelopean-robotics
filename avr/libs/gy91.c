
// The GY-91 consists of the barometric pressure sensor BMP-280 and
// the multiple-sensor package MPU-9255.  The MPU-9255 is a System in
// Package (SiP) that combines two chips:
//
// MPU-6500, which contains 
// - a 3-axis gyroscope,
// - a 3-axis accelerometer, 
// - and an onboard Digital Motion Processor™ (DMP™) capable of
//   processing complex algorithms.  AK8963, a 3-axis digital
//   compass (magnetometer).

#include <util/delay.h>
#include <limits.h>
#include <avr/eeprom.h>
#include <stdlib.h>
#include "gy91.h"
#include "i2c-master.h"

static const unsigned char i2c_gy91_accel=0x68<<1;
static const unsigned char i2c_gy91_compass=0x0c<<1;
static const unsigned char i2c_gy91_pressure=0x76<<1;

// store calibration data in eeprom
float gy91_mag_xbias=0.0;
float gy91_mag_ybias=0.0;
float gy91_mag_zbias=0.0;
float gy91_mag_xscale=1.0f;
float gy91_mag_yscale=1.0f;
float gy91_mag_zscale=1.0f;

// helper to jam two 8bit values together
unsigned short i2c_read_16(unsigned char address, unsigned char high, unsigned char low) {
  unsigned char h,l=0;
  i2c_read_reg(address, high, &h, 1);
  i2c_read_reg(address, low, &l, 1);
  unsigned short v = (h<<8)+l;
  if (v>=0x8000) {
    return -((65535 - v) + 1);
  } else {
    return v;
  }
}

int gy91_init() {
  // check the chip id register on the BMP280 pressure sensor
  // to make sure the gy91 is attached and working...
  unsigned char t=0;
  i2c_read_reg(i2c_gy91_pressure,0xd0,&t,1);
  if (t!=0x58) {
    return 1;
  }

  // set bypass mode to activate magnetometer on i2c bus
  t=0x02;
  i2c_write_reg(i2c_gy91_accel, 0x37, &t, 1);
  _delay_ms(50);

  // set up magnetometer mode
  t=0x12;
  i2c_write_reg(i2c_gy91_compass, 0x0A, &t, 1);
  _delay_ms(50);

  return 0;
}

void gy91_read_accel(float *x, float *y, float *z) {
  static const unsigned char ACCEL_XOUT_H = 0x3B;
  static const unsigned char ACCEL_XOUT_L = 0x3C;
  static const unsigned char ACCEL_YOUT_H = 0x3D;
  static const unsigned char ACCEL_YOUT_L = 0x3E;
  static const unsigned char ACCEL_ZOUT_H = 0x3F;
  static const unsigned char ACCEL_ZOUT_L = 0x40;
  
  short xout = i2c_read_16(i2c_gy91_accel, ACCEL_XOUT_H, ACCEL_XOUT_L);
  short yout = i2c_read_16(i2c_gy91_accel, ACCEL_YOUT_H, ACCEL_YOUT_L);
  short zout = i2c_read_16(i2c_gy91_accel, ACCEL_ZOUT_H, ACCEL_ZOUT_L);

  *x = 2.0 * xout / 32768.0;
  *y = 2.0 * yout / 32768.0;
  *z = 2.0 * zout / 32768.0;
}

void gy91_read_gyro(float *x, float *y, float *z) {
  static const unsigned char GYRO_XOUT_H = 0x43;
  static const unsigned char GYRO_XOUT_L = 0x44;
  static const unsigned char GYRO_YOUT_H = 0x45;
  static const unsigned char GYRO_YOUT_L = 0x46;
  static const unsigned char GYRO_ZOUT_H = 0x47;
  static const unsigned char GYRO_ZOUT_L = 0x48;

  short xout = i2c_read_16(i2c_gy91_accel, GYRO_XOUT_H, GYRO_XOUT_L);
  short yout = i2c_read_16(i2c_gy91_accel, GYRO_YOUT_H, GYRO_YOUT_L);
  short zout = i2c_read_16(i2c_gy91_accel, GYRO_ZOUT_H, GYRO_ZOUT_L);

  *x = 250.0 * xout / 32768.0;
  *y = 250.0 * yout / 32768.0;
  *z = 250.0 * zout / 32768.0;
}

float gy91_read_temp() {
  static const unsigned char TEMP_OUT_H = 0x41;
  static const unsigned char TEMP_OUT_L = 0x42;
  unsigned short temp_out = i2c_read_16(i2c_gy91_accel, TEMP_OUT_H, TEMP_OUT_L);
  return temp_out / 340.0 + 36.53;
}

void gy91_read_mag_uncalibrated(float *x, float *y, float *z) {
  static const unsigned char MAGNET_XOUT_L = 0x03;
  static const unsigned char MAGNET_XOUT_H = 0x04;
  static const unsigned char MAGNET_YOUT_L = 0x05;
  static const unsigned char MAGNET_YOUT_H = 0x06;
  static const unsigned char MAGNET_ZOUT_L = 0x07;
  static const unsigned char MAGNET_ZOUT_H = 0x08;

  short xout = i2c_read_16(i2c_gy91_compass, MAGNET_XOUT_H, MAGNET_XOUT_L);
  short yout = i2c_read_16(i2c_gy91_compass, MAGNET_YOUT_H, MAGNET_YOUT_L);
  short zout = i2c_read_16(i2c_gy91_compass, MAGNET_ZOUT_H, MAGNET_ZOUT_L);

  *x = 1200.0 * xout / 4096.0;
  *y = 1200.0 * yout / 4096.0;
  *z = 1200.0 * zout / 4096.0;

}

void gy91_read_mag(float *x, float *y, float *z) {
  gy91_read_mag_uncalibrated(x,y,z);
  // apply calibration
  *x = *x/gy91_mag_xscale+gy91_mag_xbias;
  *y = *y/gy91_mag_yscale+gy91_mag_ybias;
  *z = *z/gy91_mag_zscale+gy91_mag_zbias;
}

void gy91_calibrate_mag(unsigned int samples) {
  float x=0,y=0,z=0;

  float xmin=9999,ymin=9999,zmin=9999;
  float xmax=-9999,ymax=-9999,zmax=-9999;

  float xbias=0.0;
  float ybias=0.0;
  float zbias=0.0;
  float xscale=1.0f;
  float yscale=1.0f;
  float zscale=1.0f;

  if (samples>1000) samples=1000;

  unsigned int n=0;
  while (n<samples) {
    gy91_read_mag_uncalibrated(&x, &y, &z);
    if (x<xmin) xmin=x;
    if (y<ymin) ymin=y;
    if (z<zmin) zmin=z;
    if (x>xmax) xmax=x;
    if (y>ymax) ymax=y;
    if (z>zmax) zmax=z;
    n++;
    _delay_ms(20);
  }
  
  xbias = (xmax+xmin)/2.0f;
  ybias = (ymax+ymin)/2.0f;
  zbias = (zmax+zmin)/2.0f;

  // get average max chord lengths in counts
  xscale = (xmax-xmin)/2.0f;
  yscale = (ymax-ymin)/2.0f;
  zscale = (zmax-zmin)/2.0f;
  float av = (xscale+yscale+zscale)/3.0f;

  // normalise  
  xscale = av/xscale;
  yscale = av/yscale;
  zscale = av/zscale;

  gy91_mag_xbias=xbias;
  gy91_mag_ybias=ybias;
  gy91_mag_zbias=zbias;
  gy91_mag_xscale=xscale;
  gy91_mag_yscale=yscale;
  gy91_mag_zscale=zscale;
}

// for testing - need to free this
float *gy91_mag_calibration_data() {
  float *ret=(float*)malloc(sizeof(float)*6);
  ret[0]=gy91_mag_xbias;
  ret[1]=gy91_mag_ybias;
  ret[2]=gy91_mag_zbias;
  ret[3]=gy91_mag_xscale;
  ret[4]=gy91_mag_yscale;
  ret[5]=gy91_mag_zscale;
  return ret;
}
