#include <stdlib.h>
#include <stdio.h>

#include "utils.h"

//This code is sequential and correct.
void sequential_convolution(long img[DIM_ROW+PAD][DIM_COL+PAD][DIM_RGB], long kernel[DIM_KERNEL][DIM_KERNEL], long ***convolved_img) {
    int row, col, pixel, kernel_row, kernel_col;

    for (pixel = 0; pixel < DIM_RGB; pixel++) {
        for (col = 0; col < DIM_COL; col++) {
            for (row = 0; row < DIM_ROW; row++) {
                for (kernel_col = 0; kernel_col < DIM_KERNEL; kernel_col++) {
                    for (kernel_row = 0; kernel_row < DIM_KERNEL; kernel_row++) {
                        convolved_img[row][col][pixel] += img[row+kernel_row][col+kernel_col][pixel] * kernel[kernel_row][kernel_col];
                    }
                }
                convolved_img[row][col][pixel] /= GBLUR_NORM;
            }    
        }
    }
}
