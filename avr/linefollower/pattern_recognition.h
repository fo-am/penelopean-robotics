

#ifndef PATTERN_RECOGNITION
#define PATTERN_RECOGNITION

// one dimensional pattern recognition

unsigned char pattern_avg(unsigned char *img, unsigned long len);

typedef struct {
  unsigned int start;
  unsigned int end;
  unsigned int width;
} feature;


// record where the pattern goes over/under threshold
// assumed start is dark - so first one is a change to light
// returns number of features found
unsigned long pattern_to_features(unsigned char *img, unsigned long img_len,
				  unsigned char thresh,
				  feature *features, unsigned long feature_len);
unsigned int pattern_read_line(feature *features, unsigned long features_len, unsigned int *centre);
unsigned int pattern_read_barcode(feature *features, unsigned long features_len, unsigned int skip, unsigned char *data);

#endif
