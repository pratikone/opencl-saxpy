#include "timing.h"
#include "main-helper.h"


void mul(
  const ELEMENT_TYPE* A,
  const ELEMENT_TYPE* B,
  ELEMENT_TYPE* C,
  size_t N,
  size_t BLOCK_SIZE)
{
  size_t ROW_A = N;
  size_t ROW_B = N;
    for( int i = 0; i < ROW_A / BLOCK_SIZE; i++ )
    {
        for( int j = 0; j < ROW_B / BLOCK_SIZE; j++ )
        {
            for( int k = 0; k < ROW_A ; k++ )
            {
                for( int ii = 0; ii < BLOCK_SIZE; ii++ )
                {
                    for( int jj = 0; jj < BLOCK_SIZE; jj++ )
                    {
                        int row_c = i * BLOCK_SIZE + ii;
                        int col_c = j * BLOCK_SIZE + jj;
                        int row_a = row_c;
                        int col_a = k;
                        int row_b = k;
                        int col_b = col_c;
//                         if( (i==0) && (j==0) && (ii==0) && (jj==0) )
                        {
                            // printf(" row c %i, col c %i => i %i j %i k %i ii %i jj %i\n", row_c, col_c, i, j, k, ii, jj);
//                                 printf(" a[%i][%i] = %f ", row_a, col_a, ptrA[ row_a * ROW_A + col_a ]);
//                                 printf(" b[%i][%i] = %f \n", row_b, col_b, ptrB[ row_b * ROW_B + col_b ]);
                        }
                        C[ row_c * ROW_A + col_c ] += A[ row_a * ROW_A + col_a ] * B[ row_b * ROW_B + col_b ];
                    }                    
                }
            }                    
        }        
    }
}

int main(int argc, char **argv)
{
  if (argc != 4)
  {
    fprintf(stderr, "USO: %s  <dimension>  <vueltas>  <bloque>\n", argv[0]);
    abort();
  }

  const long n = atol(argv[1]);
  const int ntrips = atoi(argv[2]);
  const int BLOCK_SIZE = atoi(argv[3]);

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
    mul(a,b,c,n,BLOCK_SIZE);
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