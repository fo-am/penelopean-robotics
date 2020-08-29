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
				  unsigned char sensitivity,
				  unsigned char min_width,
				  unsigned char max_width,
				  feature *features, unsigned long feature_len);

unsigned int pattern_read_line(feature *features,
			       unsigned long features_len,
			       unsigned char *centre);

unsigned int pattern_read_barcode(feature *features,
				  unsigned long features_len,
				  unsigned int skip,
				  unsigned char bits,
				  unsigned char *data);

#endif
