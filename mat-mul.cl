//! Enable this for Double precision:
//#pragma OPENCL EXTENSION cl_khr_fp64: enable

#define ELEMENT_TYPE float


__kernel void mul(
    __global const ELEMENT_TYPE* A,
    __global const ELEMENT_TYPE* B,
    __global ELEMENT_TYPE* C,
    long N)
{
   int tx = get_local_id(0);
   int ty = get_local_id(1);
 
   ELEMENT_TYPE value = 0;
   for (long k = 0; k < N; ++k)
   {
      ELEMENT_TYPE elementA = A[ty * N + k];
      ELEMENT_TYPE elementB = B[k * N + tx];
      value += elementA * elementB;
   }
 
   C[ty * N + tx] = value;
}
