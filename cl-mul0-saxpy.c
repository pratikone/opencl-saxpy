#include "timing.h"
#include "main-helper.h"


void saxpy(
  size_t N,
  const ELEMENT_TYPE a,
  const ELEMENT_TYPE *x,
  ELEMENT_TYPE *y)
{
  for (size_t i = 0; i < N; ++i)
    y[i*N] += a * x[i*N];
}

void mul(
  const ELEMENT_TYPE* A,
  const ELEMENT_TYPE* B,
  ELEMENT_TYPE* C,
  size_t N)
{
  size_t i,j;

  for (i = 0; i < N; ++i)
  {
    for (j = 0; j < N; ++j)
      saxpy(N, B[i*N+j], &A[i], &C[j]);
      // saxpy(N, B[j*N+i], &A[j], &C[i]);
  }
}


int main(int argc, char **argv)
{
  if (argc != 3)
  {
    fprintf(stderr, "USO: %s  <dimension>  <vueltas>\n", argv[0]);
    abort();
  }

  const long n = atol(argv[1]);
  const int ntrips = atoi(argv[2]);

  // --------------------------------------------------------------------------
  // allocate and initialize CPU memory
  // --------------------------------------------------------------------------

  long sizeN = n*n;
  ELEMENT_TYPE *a = (ELEMENT_TYPE *) malloc(sizeof(ELEMENT_TYPE) * sizeN);
  if (!a) { perror("alloc x"); abort(); }
  ELEMENT_TYPE *b = (ELEMENT_TYPE *) malloc(sizeof(ELEMENT_TYPE) * sizeN);
  if (!b) { perror("alloc y"); abort(); }
  ELEMENT_TYPE *c = (ELEMENT_TYPE *) malloc(sizeof(ELEMENT_TYPE) * sizeN);
  if (!c) { perror("alloc z"); abort(); }

  srand(2006);
  randomInit(a, sizeN);
  randomInit(b, sizeN);

  // --------------------------------------------------------------------------
  // run code on device
  // --------------------------------------------------------------------------

  timestamp_type time1, time2;
  get_timestamp(&time1);

  for (int trip = 0; trip < ntrips; ++trip)
  {
    zeroInit(c, sizeN);
    mul(a,b,c,n);
  }

  get_timestamp(&time2);
  double elapsed = timestamp_diff_in_seconds(time1,time2)/ntrips;
  printf("%f s\n", elapsed);
  printf("%f GB/s\n",
      3*sizeN*sizeof(ELEMENT_TYPE)/1e9/elapsed);

  // --------------------------------------------------------------------------
  // transfer back & check
  // --------------------------------------------------------------------------

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

  // --------------------------------------------------------------------------
  // clean up
  // --------------------------------------------------------------------------

  free(a);
  free(b);
  free(c);

  return 0;
}