__kernel void mul(
    __global const double* A,
    __global const double* B,
    __global double* C,
    long N)
{
   int tx = get_local_id(0);
   int ty = get_local_id(1);
 
   double value = 0;
   for (long k = 0; k < N; ++k)
   {
      double elementA = A[ty * N + k];
      double elementB = B[k * N + tx];
      value += elementA * elementB;
   }
 
   C[ty * N + tx] = value;
}
