//dimensions for images
#define DIM_ROW 5000
#define DIM_COL 5000
//do NOT modify DIM_RGB, it is always 3
#define DIM_RGB 3

//dimensions for convolution kernel in phase 3
//modify when testing for different kernel sizes
#define DIM_KERNEL 5

//dimensions for padded_original_img convolution in phase 3
#define PAD_OFFSET (DIM_KERNEL/2)
#define PAD 2*PAD_OFFSET

//normalization factors for gaussian blur
#define GBLUR_3x3_NORM 16
#define GBLUR_5x5_NORM 256

//actual normalization factor used in solutions
//modify when testing for different kernel sizes
#define GBLUR_NORM GBLUR_5x5_NORM
