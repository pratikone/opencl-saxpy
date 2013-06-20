//! Enable this for Double precision:
//#pragma OPENCL EXTENSION cl_khr_fp64: enable

#define ELEMENT_TYPE float


void saxpy(long N, ELEMENT_TYPE a, ELEMENT_TYPE *x, ELEMENT_TYPE *y)
{
  for (long i = 0; i < N; ++i)
    y[i*N] += a * x[i*N];
}


__kernel void mul(
  __global const ELEMENT_TYPE* A,
  __global const ELEMENT_TYPE* B,
  __global ELEMENT_TYPE* C)
{
  long i = get_global_id(0);
  long j = get_global_id(1);

  long N = get_global_size(0);
  // long M = get_global_size(1);

  ELEMENT_TYPE value = 0.0f;

  //saxpy(N, B[i*N+j], &A[i], &C[j]);
  for (long k = 0; k < N; ++k)
  {
    C[i*N+j] += B[i*N+j] * A[i*N+i];
    barrier(CLK_GLOBAL_MEM_FENCE);
  }
}
