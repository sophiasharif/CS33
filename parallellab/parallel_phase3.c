#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "utils.h"

void parallel_convolution(long img[DIM_ROW+PAD][DIM_COL+PAD][DIM_RGB], long kernel[DIM_KERNEL][DIM_KERNEL], long ***convolved_img) {

  int row, col, kernel_row, kernel_col;

#pragma omp parallel for private(row, col, kernel_row, kernel_col)
    for (row = 0; row < DIM_ROW; row++) {
      for (col = 0; col < DIM_COL; col++) {

	int square_red = 0, square_green = 0, square_blue = 0;

	for (kernel_row = 0; kernel_row < DIM_KERNEL; kernel_row++) {
	  for (kernel_col = 0; kernel_col < DIM_KERNEL; kernel_col++) {

	      long kernel_square = kernel[kernel_row][kernel_col];
	      long* img_square_ptr = img[row+kernel_row][col+kernel_col];

	      square_red += img_square_ptr[0] * kernel_square;
	      square_green += img_square_ptr[1] * kernel_square;
	      square_blue += img_square_ptr[2] * kernel_square;
	      
	    }
	}
	  	  
	  convolved_img[row][col][0] = square_red / GBLUR_NORM;
	  convolved_img[row][col][1] = square_green / GBLUR_NORM;
	  convolved_img[row][col][2] = square_blue / GBLUR_NORM;

      }
    }
}
