//! Enable this for Double precision:
//#pragma OPENCL EXTENSION cl_khr_fp64: enable

#define ELEMENT_TYPE float


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

  for (long k = 0; k < N; ++k)
  {
    ELEMENT_TYPE elementA = A[i * N + k];
    ELEMENT_TYPE elementB = B[k * N + j];
    value += elementA * elementB;
  }

  C[i * N + j] = value;
}
