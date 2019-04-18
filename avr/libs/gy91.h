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
