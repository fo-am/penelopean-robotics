#ifndef PENELOPE_FIXED
#define PENELOPE_FIXED

// fixed point maths ftw
#define FIXED 1000
#define MAKE_FIXED(x) (FIXED*x)
#define FIXED_TO_FLOAT(x) (x/(float)FIXED)

#endif
