#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>
#include <unistd.h>

#include "utils.h"

//input for phases 1 and 2
long original_img[DIM_ROW][DIM_COL][DIM_RGB];
//input for phase 3
long padded_original_img[DIM_ROW+PAD][DIM_COL+PAD][DIM_RGB];

void print_modified_img(long ***img) {
  int row, col, pixel;

  for (row = 0; row < DIM_ROW; row++) {
    for (col = 0; col < DIM_COL; col++) {
      for (pixel = 0; pixel < DIM_RGB; pixel++) {
        printf("%d ", img[row][col][pixel]);
      }
      printf("\t");
    }
    printf("\n");
  }
}

void print_original_img(long img[DIM_ROW][DIM_COL][DIM_RGB]) {
  int row, col, pixel;

  for (row = 0; row < DIM_ROW; row++) {
    for (col = 0; col < DIM_COL; col++) {
      for (pixel = 0; pixel < DIM_RGB; pixel++) {
        printf("%d ", img[row][col][pixel]);
      }
      printf("\t");
    }
    printf("\n");
  }
}

void print_padded_img(long img[DIM_ROW+PAD][DIM_COL+PAD][DIM_RGB]) {
  int row, col, pixel;

  for (row = 0; row < DIM_ROW + PAD; row++) {
    for (col = 0; col < DIM_COL + PAD; col++) {
      for (pixel = 0; pixel < DIM_RGB; pixel++) {
        printf("%d ", img[row][col][pixel]);
      }
      printf("\t");
    }
    printf("\n");
  }
}

int matrix_compare(long ***sequential_img, long ***parallelized_img) {
  int row, col, pixel;
  long expected, observed;
  int error = 0;

  for (row = 0; row < DIM_ROW; row++) {
    for (col = 0; col < DIM_COL; col++) {
      for (pixel = 0; pixel < DIM_RGB; pixel++) {
        expected = sequential_img[row][col][pixel];
        observed = parallelized_img[row][col][pixel];
        if (expected != observed) {
          //printf("Pixel mistmatch at (%d, %d). Should be %d, got %d.\n", row, col, expected, observed);
          error = 1;
        }
      }
    }
  }
  return error;
}

int main(int argc, const char* argv[])
{
  int row, col, pixel;
  struct timespec start, finish;
  double seqDelay, parDelay;
  long rgb_val;
  int error;

  long *sequential_avgs;
  long *parallel_avgs;

  long ***sequential_modified_img;
  long ***parallel_modified_img;

  long *sequential_min_max_gray;
  long *parallel_min_max_gray;

  srand(time(NULL));

  //initialize input image arrays
  for (row=0; row<DIM_ROW; row++) {
    for (col=0; col<DIM_COL; col++) {
      for (pixel=0; pixel<DIM_RGB; pixel++) {
          rgb_val = (long)(rand()%256);
          original_img[row][col][pixel] = rgb_val;
          padded_original_img[row+PAD_OFFSET][col+PAD_OFFSET][pixel] = rgb_val;
      }
    }
  }

  //initialize phase 3 gaussian blur kernels
  long gblur_3x3_kernel[3][3] = {
    {1, 2, 1},
    {2, 4, 2},
    {1, 2, 1}
  };

  long gblur_5x5_kernel[5][5] = {
    {1, 4, 6, 4, 1},
    {4, 16, 24, 16, 4},
    {6, 24, 36, 24, 6},
    {4, 16, 24, 16, 4},
    {1, 4, 6, 4, 1}
  };

  //phase 1 testing
  /**********************************************************************************************/
  error = 0;
  sequential_avgs = (long*)calloc(DIM_RGB, sizeof(long));
  parallel_avgs = (long*)calloc(DIM_RGB, sizeof(long));

  printf("Running phase 1.\n");

  clock_gettime(CLOCK_MONOTONIC, &start);
  sequential_avg_pixel(original_img, sequential_avgs);
  clock_gettime(CLOCK_MONOTONIC, &finish);

  seqDelay = (finish.tv_sec - start.tv_sec);
  seqDelay += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
  printf("Sequential version took %lf time units\n",seqDelay);

  printf("Warning! If the your parallel solution seems like it is taking forever, CTRL-C. You most likely have not fixed the bug.\n");
  clock_gettime(CLOCK_MONOTONIC, &start);
  parallel_avg_pixel(original_img, parallel_avgs);
  clock_gettime(CLOCK_MONOTONIC, &finish);

  parDelay = (finish.tv_sec - start.tv_sec);
  parDelay += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
  printf("Parallel version took %lf time units\n", parDelay);

  for (pixel = 0; pixel < DIM_RGB; pixel++) {
    if (sequential_avgs[pixel] != parallel_avgs[pixel]) {
      error = 1;
      break;
    }
  }

  if (error == 1) {
    printf("Your phase 1 results are incorrect.\n The expected result was [%d, %d, %d].\n The observed result was [%d, %d, %d].\n\n", sequential_avgs[0],sequential_avgs[1],sequential_avgs[2],parallel_avgs[0],parallel_avgs[1],parallel_avgs[2]);
  }
  else {
    printf("Your phase 1 results are correct.\n");
    printf("Your phase 1 speedup was %fx.\n\n", (float)(seqDelay / parDelay));
  }

  free(sequential_avgs);
  free(parallel_avgs);
  /**********************************************************************************************/

  //phase 2 testing
  /**********************************************************************************************/
  //dynamically allocate modification arrays
  sequential_modified_img = (long***)calloc(DIM_ROW, sizeof(long**));
  parallel_modified_img = (long***)calloc(DIM_ROW, sizeof(long**));
  for (row = 0; row < DIM_ROW; row++) {
    sequential_modified_img[row] = (long**)calloc(DIM_COL, sizeof(long*));
    parallel_modified_img[row] = (long**)calloc(DIM_COL, sizeof(long*));
    for (col = 0; col < DIM_COL; col++) {
      sequential_modified_img[row][col] = (long*)calloc(DIM_RGB, sizeof(long));
      parallel_modified_img[row][col] = (long*)calloc(DIM_RGB, sizeof(long));
    }
  }

  sequential_min_max_gray = (long*)calloc(2, sizeof(long));
  parallel_min_max_gray = (long*)calloc(2, sizeof(long));

  printf("Running phase 2.\n");

  clock_gettime(CLOCK_MONOTONIC, &start);
  sequential_to_grayscale(original_img, sequential_modified_img, sequential_min_max_gray);
  clock_gettime(CLOCK_MONOTONIC, &finish);

  seqDelay = (finish.tv_sec - start.tv_sec);
  seqDelay += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
  printf("Sequential version took %lf time units\n",seqDelay);

  clock_gettime(CLOCK_MONOTONIC, &start);
  parallel_to_grayscale(original_img, parallel_modified_img, parallel_min_max_gray);
  clock_gettime(CLOCK_MONOTONIC, &finish);

  parDelay = (finish.tv_sec - start.tv_sec);
  parDelay += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
  printf("Parallel version took %lf time units\n", parDelay);

  error = matrix_compare(sequential_modified_img, parallel_modified_img);
  if (error != 1 && (sequential_min_max_gray[0] != parallel_min_max_gray[0] || 
      sequential_min_max_gray[1] != parallel_min_max_gray[1]))
    error = 2;

  if (error == 1) {
    printf("Your phase 2 image results are incorrect.\n\n");
  }
  else if (error == 2) {
    printf("Your phase 2 min max gray results are incorrect.\n");
    printf("The expected min max gray result was %d,%d\n", sequential_min_max_gray[0], sequential_min_max_gray[1]);
    printf("The observed min max gray result was %d,%d\n\n", parallel_min_max_gray[0], parallel_min_max_gray[1]);
  }
  else {
    printf("Your phase 2 results are correct. However, they may only be correct for this run, make sure to take care of possible data races!\n");
    printf("Your phase 2 speedup was %fx.\n\n", (float)(seqDelay / parDelay));
  }

  free(sequential_min_max_gray);
  free(parallel_min_max_gray);
  /**********************************************************************************************/

  //phase 3 testing
  /**********************************************************************************************/
  for (row=0; row<DIM_ROW; row++) {
    for (col=0; col<DIM_COL; col++) {
      for (pixel=0; pixel<DIM_RGB; pixel++) {
          sequential_modified_img[row][col][pixel] = 0;
          parallel_modified_img[row][col][pixel] = 0;
      }
    }
  }

  printf("Running phase 3.\n");

  clock_gettime(CLOCK_MONOTONIC, &start);
  sequential_convolution(padded_original_img, gblur_5x5_kernel, sequential_modified_img);
  clock_gettime(CLOCK_MONOTONIC, &finish);

  seqDelay = (finish.tv_sec - start.tv_sec);
  seqDelay += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
  printf("Sequential version took %lf time units\n",seqDelay);

  clock_gettime(CLOCK_MONOTONIC, &start);
  parallel_convolution(padded_original_img, gblur_5x5_kernel, parallel_modified_img);
  clock_gettime(CLOCK_MONOTONIC, &finish);

  parDelay = (finish.tv_sec - start.tv_sec);
  parDelay += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
  printf("Parallel version took %lf time units\n", parDelay);

  error = matrix_compare(sequential_modified_img, parallel_modified_img);

  if (error == 1) {
    printf("Your phase 3 image results are incorrect.\n\n");
  }
  else {
    printf("Your phase 3 results are correct.\n");
    printf("Your phase 3 speedup was %fx.\n\n", (float)(seqDelay / parDelay));
  }
  /**********************************************************************************************/

  //cleanup
  for (row = 0; row < DIM_ROW; row++) {
    for (col = 0; col < DIM_COL; col++) {
      free(sequential_modified_img[row][col]);
      free(parallel_modified_img[row][col]);
    }
    free(sequential_modified_img[row]);
    free(parallel_modified_img[row]);
  }
  free(sequential_modified_img);
  free(parallel_modified_img);

  return 0;

}
