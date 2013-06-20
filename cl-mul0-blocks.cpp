#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 2
#define SIZE_MULTIPLE 2

#define ROW_A (SIZE_MULTIPLE * BLOCK_SIZE)
#define COL_A (SIZE_MULTIPLE * BLOCK_SIZE)
#define ROW_B (SIZE_MULTIPLE * BLOCK_SIZE)
#define COL_B ROW_A
#define ROW_C ROW_B
#define COL_C COL_A

void randomInit(float* data, int size)
{
   for (int i = 0; i < size; ++i)
   data[i] = rand() / (float)RAND_MAX;
}

void printMatrix(float* data, int size, int rowSize, const char* name)
{
   printf("\nMatrix %s\n", name);
   for(int i = 0; i < size; i++)
   {
      printf("%f ", data[i]);
      if(((i + 1) % rowSize) == 0)
        printf("\n");
   }
   printf("\n");
}

int main( int argc, char* argv[] )
{   
    float *ptrA, *ptrB, *ptrC;
    unsigned int memSizeA, memSizeB, memSizeC;
    
    unsigned int rowA = ROW_A;
    unsigned int colA = COL_A;
    unsigned int rowB = ROW_B;
    unsigned int colB = COL_B;
    unsigned int rowC = ROW_C;
    unsigned int colC = COL_C;    
    
    // allocate host memory for matrices A and B
    unsigned int sizeA = rowA * colA;
    memSizeA = sizeof(float) * sizeA;
    ptrA = (float*)malloc(memSizeA);
    
    unsigned int sizeB = rowB * colB;
    memSizeB = sizeof(float) * sizeB;
    ptrB = (float*) malloc(memSizeB);

    // initialize host memory
    srand(2006);
    randomInit(ptrA, sizeA);
    randomInit(ptrB, sizeB);

    // allocate host memory for result
    unsigned int sizeC = rowC * colC;
    memSizeC = sizeof(float) * sizeC;
    ptrC = (float*) malloc(memSizeC);

    printMatrix(ptrA, sizeA, rowA, " A");
    printMatrix(ptrB, sizeB, rowB, " B");
    
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
                            printf(" row c %i, col c %i => i %i j %i k %i ii %i jj %i\n", row_c, col_c, i, j, k, ii, jj);
//                                 printf(" a[%i][%i] = %f ", row_a, col_a, ptrA[ row_a * ROW_A + col_a ]);
//                                 printf(" b[%i][%i] = %f \n", row_b, col_b, ptrB[ row_b * ROW_B + col_b ]);
                        }
                        ptrC[ row_c * ROW_A + col_c ] += ptrA[ row_a * ROW_A + col_a ] * ptrB[ row_b * ROW_B + col_b ];
                    }                    
                }
            }                    
        }        
    }
    
    printMatrix(ptrC, sizeC, rowC, " C");
}