//! Enable this for Double precision:
//#pragma OPENCL EXTENSION cl_khr_fp64: enable

#define ELEMENT_TYPE float


__kernel void mul(
  __global ELEMENT_TYPE* A,
  __global ELEMENT_TYPE* B,
  __global ELEMENT_TYPE* C,
  int sizeN )
{
  long i = get_global_id(0);
  long j = get_global_id(1);

  ELEMENT_TYPE value = 0.0f;

  for (int k = 0; k < sizeN; ++k)
  {
    ELEMENT_TYPE elementA = A[j * sizeN + k];
    ELEMENT_TYPE elementB = B[k * sizeN + i];
    value += elementA * elementB;
  }

  C[j * sizeN + i] = value;
}
