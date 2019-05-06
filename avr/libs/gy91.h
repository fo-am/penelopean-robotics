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

int gy91_init();
void gy91_read_accel(float *x, float *y, float *z);
void gy91_read_gyro(float *x, float *y, float *z);
float gy91_read_temp();

void gy91_calibrate_mag(unsigned int samples);
void gy91_read_mag_uncalibrated(float *x, float *y, float *z);
void gy91_read_mag(float *x, float *y, float *z);
float *gy91_mag_calibration_data();

#endif
