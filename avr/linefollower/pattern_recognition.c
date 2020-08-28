#include "pattern_recognition.h"

// one dimensional pattern recognition

unsigned char pattern_avg(unsigned char *img, unsigned long len) {
  unsigned long t=0;
  for (unsigned int n=0; n<len; n++) {
    t+=img[n];
  }
  t/=len;
  return (unsigned char)t;
}

// record where the pattern goes over/under threshold
// assumed start is dark - so first one is a change to light
// returns number of features found
unsigned long pattern_to_features(unsigned char *img, unsigned long img_len,
				  unsigned char thresh,
				  feature *features, unsigned long feature_len) {
  unsigned long feature_i=0;
  unsigned char state=0; // 0=under 1=over
  for (unsigned int n=0; n<img_len; n++) {
    if (feature_i<feature_len) {
      if (state==0) { // dark, searching for light
	if (img[n]>thresh) {
	  state=1;
	  features[feature_i].start=n;
	}
      } else { // light, searching for dark
	if (img[n]<thresh) {
	  state=0;
	  features[feature_i].end=n;
	  features[feature_i].width=n-features[feature_i].start;
	  feature_i++;
	}
      }
    }
  }
  return feature_i;
}

// we assume our navigation line is the "left" most (wrt to the data)
// light line - so find the centre position of this from all the features
// returns error code
unsigned int pattern_read_line(feature *features, unsigned long features_len, unsigned int *centre) {
  if (features_len<1) {
    return 1; // no features - all dark/light image
  }
  *centre = features[0].start+features[0].width/2;
  return 0;
}

unsigned int pattern_read_barcode(feature *features, unsigned long features_len, unsigned int skip, unsigned char *data) {
  unsigned int len=features_len-skip;
  if (len<1) {
    return 1; // not found - all dark/light image or just line
  }
  
  if (len<4) {
    return 2; // some data, but not enough for 4 bits
  }
  
  // find average width of feature
  float t=0;
  for (unsigned int f=skip; f<features_len; f++) {
    t+=features[f].width;
  }
  t/=len;
  
  *data=0;
  
  // read over/under avg width
  unsigned char pos=0;
  for (unsigned int f=skip; f<features_len; f++) {    
    if (features[f].width>t) {
      printf("high\n");
      *data|=1<<pos;
    } else {
      printf("low\n");
    }
    pos++;
  }
  return 0;
}


/*

#include <stdio.h>


// testing
void main() {
  unsigned char img[] = {0,0,10,255,200,10,0,0,200,100,50,10,0,0,10,0};
  
  unsigned char avg=pattern_avg(img,sizeof(img));
  printf("avg = %d\n",avg);
  
  feature features[20];  
  unsigned long len = pattern_to_features(img,sizeof(img),avg,features,20);
  printf("num features: %ld\n",len);
  for (unsigned int c=0; c<len; c++) {
    printf("feature: %d\n",features[c].width);
  }
  
  unsigned int centre = 0;
  printf("reading line: %d\n",pattern_read_line(features,len,&centre));
  printf("centre: %d\n",centre);
  
  // 1 0 0 1 ish = 9
  unsigned char code_img[] = {0,0,100,255,200,200,10,0,0,200,100,0,0,0,0,120,121,0,0,10,10,200,200,130,120,120,20,0,0,0};
  len=pattern_to_features(code_img,sizeof(code_img),pattern_avg(code_img,sizeof(code_img)),features,20);
  
  for (unsigned int c=0; c<len; c++) {
    printf("feature: %d %d\n",features[c].start,features[c].width);
  }
  
  unsigned char data=0;
  pattern_read_barcode(features, len, 0, &data);
  printf("data: %d\n",data);
  
}


*/
