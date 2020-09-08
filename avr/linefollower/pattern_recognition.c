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
				  unsigned char courseness,
				  unsigned char min_width,
				  unsigned char max_width,
				  feature *features, unsigned long feature_len) {
  unsigned long feature_i=0;
  unsigned char state=0; // 0=under 1=over
  for (unsigned int n=0; n<img_len; n++) {
    if (feature_i<feature_len) {
      if (state==0) { // dark, searching for light
	if (img[n]>thresh+courseness) {
	  state=1;
	  features[feature_i].start=n;
	}
      } else { // light, searching for dark
	if (img[n]<thresh-courseness) {
	  state=0;
	  features[feature_i].end=n;
	  features[feature_i].width=n-features[feature_i].start;

	  // only save this one if it's large enough
	  if (features[feature_i].width>min_width &&
	      features[feature_i].width<max_width) {
	    feature_i++;
	  }
	}
      }
    }
  }
  return feature_i;
}

// we assume our navigation line is the "left" most (wrt to the data)
// light line - so find the centre position of this from all the features
// returns error code
unsigned int pattern_read_line(feature *features,
			       unsigned long features_len,
			       unsigned char *centre) {
  if (features_len<1) {
    return 1; // no features - all dark/light image
  }
  *centre = features[0].start+features[0].width/2;
  return 0;
}

unsigned int pattern_read_barcode(feature *features,
				  unsigned long features_len,
				  unsigned int skip,
				  unsigned char bits,
				  unsigned char *data) {

  unsigned int len=features_len-skip;
  if (len<1) {
    return 1; // not found - all dark/light image or just line
  }
  
  if (len<bits) {
    return 2; // some data, but not enough 
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
      *data|=1<<pos;
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
