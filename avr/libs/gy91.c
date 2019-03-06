
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

#include "gy91.h"
#include "i2c-master.h"
#include <util/delay.h>

static const unsigned char i2c_gy91_accel=0x68<<1;
static const unsigned char i2c_gy91_compass=0x0c<<1;
static const unsigned char i2c_gy91_pressure=0x76<<1;

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

void gy91_read_mag(float *x, float *y, float *z) {
  static const unsigned char MAGNET_XOUT_L = 0x03;
  static const unsigned char MAGNET_XOUT_H = 0x04;
  static const unsigned char MAGNET_YOUT_L = 0x05;
  static const unsigned char MAGNET_YOUT_H = 0x06;
  static const unsigned char MAGNET_ZOUT_L = 0x07;
  static const unsigned char MAGNET_ZOUT_H = 0x08;

  unsigned char t=0x12;
  i2c_write_reg(i2c_gy91_compass, 0x0A, &t, 1);
  _delay_ms(50);

  short xout = i2c_read_16(i2c_gy91_compass, MAGNET_XOUT_H, MAGNET_XOUT_L);
  short yout = i2c_read_16(i2c_gy91_compass, MAGNET_YOUT_H, MAGNET_YOUT_L);
  short zout = i2c_read_16(i2c_gy91_compass, MAGNET_ZOUT_H, MAGNET_ZOUT_L);

  *x = 1200.0 * xout / 4096.0;
  *y = 1200.0 * yout / 4096.0;
  *z = 1200.0 * zout / 4096.0;
}
