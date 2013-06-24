//! Enable this for Double precision:
//#pragma OPENCL EXTENSION cl_khr_fp64: enable

#define ELEMENT_TYPE float


__kernel void zero(
  __global ELEMENT_TYPE* C)
{
  long i = get_global_id(0);
  long j = get_global_id(1);
  long N = get_global_size(0);

  C[i * N + j] = 0;
}


__kernel void mul(
  __global const ELEMENT_TYPE* A,
  __global const ELEMENT_TYPE* B,
  __global ELEMENT_TYPE* C)
{
  long k = get_global_id(0);
  long j = get_global_id(1);

  long N = get_global_size(0);
  // long M = get_global_size(1);

  for (long i = 0; i < N; ++i)
  {
    ELEMENT_TYPE elementA = A[i * N + k];
    ELEMENT_TYPE elementB = B[k * N + j];
    C[i * N + j] += elementA * elementB;
  }

}
