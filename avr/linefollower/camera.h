#ifndef CAMERA
#define CAMERA

#define CLK_PIN PB1
#define SI_PIN PB3
char image[128];

void read_camera(int exposure);

#endif
