// Penelopean Robotics Copyright (C) 2019 FoAM Kernow
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

#ifndef GY91_SENSOR
#define GY91_SENSOR

static const unsigned char  MAG_WIA      = 0x00;
static const unsigned char  MAG_INFO     = 0x01;
static const unsigned char  MAG_ST1      = 0x02;
static const unsigned char  MAG_HXL      = 0x03;
static const unsigned char  MAG_HXH      = 0x04;
static const unsigned char  MAG_HYL      = 0x05;
static const unsigned char  MAG_HYH      = 0x06;
static const unsigned char  MAG_HZL      = 0x07;
static const unsigned char  MAG_HZH      = 0x08;
static const unsigned char  MAG_ST2      = 0x09;
static const unsigned char  MAG_CNTL1    = 0x0a;
static const unsigned char  MAG_CNTL2    = 0x0b;
static const unsigned char  MAG_ASTC     = 0x0c;
static const unsigned char  MAG_TS1      = 0x0d;
static const unsigned char  MAG_TS2      = 0x0e;
static const unsigned char  MAG_I2CDIS   = 0x0f;
static const unsigned char  MAG_ASAX     = 0x10;
static const unsigned char  MAG_ASAY     = 0x11;
static const unsigned char  MAG_ASAZ     = 0x12;
static const unsigned char  MAG_RSV      = 0x13;

static const unsigned char MAG_MODE_POWER_DOWN = 0x00;
static const unsigned char MAG_MODE_SINGLE     = 0x01;
static const unsigned char MAG_MODE_CONT_1     = 0x02;
static const unsigned char MAG_MODE_EXTERN     = 0x04;
static const unsigned char MAG_MODE_CONT_2     = 0x06;
static const unsigned char MAG_MODE_TEST       = 0x08;
static const unsigned char MAG_MODE_FUSE       = 0x15;

static const unsigned char MAG_OUT_14BIT       = 0x00;
static const unsigned char MAG_OUT_16BIT       = 0x16;

static const unsigned char  TEMP_OUT_H = 0x41;
static const unsigned char  TEMP_OUT_L = 0x42;

static const unsigned char GYRO_XOUT_H = 0x43;
static const unsigned char GYRO_XOUT_L = 0x44;
static const unsigned char GYRO_YOUT_H = 0x45;
static const unsigned char GYRO_YOUT_L = 0x46;
static const unsigned char GYRO_ZOUT_H = 0x47;
static const unsigned char GYRO_ZOUT_L = 0x48;

static const unsigned char ACCEL_XOUT_H = 0x3B;
static const unsigned char ACCEL_XOUT_L = 0x3C;
static const unsigned char ACCEL_YOUT_H = 0x3D;
static const unsigned char ACCEL_YOUT_L = 0x3E;
static const unsigned char ACCEL_ZOUT_H = 0x3F;
static const unsigned char ACCEL_ZOUT_L = 0x40;
  

int gy91_init();
void gy91_read_accel(float *x, float *y, float *z);
void gy91_read_gyro(float *x, float *y, float *z);
float gy91_read_temp();

void gy91_calibrate_mag(unsigned int samples);
void gy91_read_mag_uncalibrated(float *x, float *y, float *z);
void gy91_read_mag(float *x, float *y, float *z);
float *gy91_mag_calibration_data();
float *gy91_mag_sense_data();

#endif
