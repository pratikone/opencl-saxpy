#ifndef MAIN_HELPER
#define MAIN_HELPER

#include <stdio.h>
#include <stdlib.h>

#define ELEMENT_TYPE float


void randomInit(ELEMENT_TYPE* data, size_t size)
{
  for (size_t i = 0; i < size; ++i)
    data[i] = rand() / (float)RAND_MAX;
    // data[i] = (float)i/10;
}

void zeroInit(ELEMENT_TYPE* data, size_t size)
{
  for (size_t i = 0; i < size; ++i)
    data[i] = 0.0f;
}

void printStatistics(const timestamp_type time1, const timestamp_type time2, int ntrips, size_t N)
{
  double elapsed = timestamp_diff_in_seconds(time1,time2)/ntrips;
  printf("%12f s\n", elapsed);
  printf("%12f MB/s\n",
      3*N*N*sizeof(ELEMENT_TYPE)/1e6/elapsed);
}

void printMatrix(const ELEMENT_TYPE* data, size_t n)
{
  for(size_t i = 0; i < n*n; i++)
  {
    printf(" %.4f", data[i]);
    if(((i + 1) % n) == 0)
      printf(";\n");
    else
      printf(",");
  }
  printf("\n");
}

void testResult(
  const ELEMENT_TYPE* A, const ELEMENT_TYPE* B, const ELEMENT_TYPE* C,
  size_t N, size_t i, size_t j)
{
  ELEMENT_TYPE value = 0.0f;
  for (size_t k = 0; k < N; ++k) {
    ELEMENT_TYPE elementA = A[i * N + k];
    ELEMENT_TYPE elementB = B[k * N + j];
    value += elementA * elementB;
  }
  printf("  C[%3u][%3u] is ",(uint)i, (uint)j);
  if (C[i * N + j] == value)
    printf("OK :)");
  else
    printf("NOT OK!!! expected be %f (was %f)", value, C[i * N + j]);
  printf("\n");
}

void printCheckResults(const ELEMENT_TYPE* a, const ELEMENT_TYPE* b, const ELEMENT_TYPE* c, size_t n)
{
  if (n < 30)
  {
    printf("\nMatrix A\n");
    printMatrix(a, n);
    
    printf("\nMatrix B\n");
    printMatrix(b, n);
    
    printf("\nMatrix C = A · B\n");
    printMatrix(c, n);
  }

  printf("\nTesting some results:\n\n");
  testResult(a,b,c,n,0,0);
  testResult(a,b,c,n,n/2,n/2);
  testResult(a,b,c,n,rand()%n,rand()%n);
  testResult(a,b,c,n,n-1,n-1);
}


size_t shrRoundUp( int group_size, int global_size ) 
{
    int r = global_size % group_size;
    if( r == 0 )
        return global_size;
    return global_size + group_size - r;
}

#endif