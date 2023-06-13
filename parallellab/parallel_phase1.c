#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "utils.h"

//This code is buggy! Find the bug and speed it up.                                                                                                               
void parallel_avg_pixel(long img[DIM_ROW][DIM_COL][DIM_RGB], long *avgs) {
  int row, col;
  long count = 0;

#pragma omp parallel for reduction(+:count, avgs[:DIM_RGB]) private(row, col)
    for (row = 0; row < DIM_ROW; row++){
      for (col = 0; col < DIM_COL; col++) {
	  avgs[0] += img[row][col][0];
	  avgs[1] += img[row][col][1];
	  avgs[2] += img[row][col][2];
          count++;
      }
    }

    avgs[0] /= count;
    avgs[1] /= count;
    avgs[2] /= count; 
}
