#include <stdlib.h>
#include <stdio.h>

#include "utils.h"

//This code is sequential and correct.
void sequential_to_grayscale(long img[DIM_ROW][DIM_COL][DIM_RGB], long ***grayscale_img, long *min_max_gray) {
    int row, col, pixel, gray_pixel;
    long min_gray = 256;
    long max_gray = -1;

    for (col = 0; col < DIM_COL; col++) {
        for (row = 0; row < DIM_ROW; row++){
            for (gray_pixel = 0; gray_pixel < DIM_RGB; gray_pixel++) {
                for (pixel = 0; pixel < DIM_RGB; pixel++) {
                    grayscale_img[row][col][gray_pixel] += img[row][col][pixel];
                }
                grayscale_img[row][col][gray_pixel] /= 3;
                if (grayscale_img[row][col][gray_pixel] < min_gray) {
                    min_max_gray[0] = grayscale_img[row][col][gray_pixel];
                    min_gray = grayscale_img[row][col][gray_pixel];
                }
                if (grayscale_img[row][col][gray_pixel] > max_gray) {
                    min_max_gray[1] = grayscale_img[row][col][gray_pixel];
                    max_gray = grayscale_img[row][col][gray_pixel];
                }
            }
        }
    }
}