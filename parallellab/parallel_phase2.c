#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "utils.h"

//This code is buggy! Find the bug and speed it up.
void parallel_to_grayscale(long img[DIM_ROW][DIM_COL][DIM_RGB], long ***grayscale_img, long *min_max_gray) {

  int min_gray = 256;
  int max_gray = -1;

#pragma omp parallel for reduction(min: min_gray) reduction(max: max_gray)
    for (int row = 0; row < DIM_ROW; row++){
      for (int col = 0; col < DIM_COL; col++) {

	int ave = (img[row][col][0] + img[row][col][1] + img[row][col][2]) / 3;

	grayscale_img[row][col][0] = ave;
	grayscale_img[row][col][1] = ave;
	grayscale_img[row][col][2] = ave;
	  
	if (ave < min_gray)
	  min_gray = ave;
	if (ave > max_gray)
	  max_gray = ave;

      }
    }

    min_max_gray[0] = min_gray;
    min_max_gray[1] = max_gray;
}
