#define ELEMENT_TYPE float

__kernel void sum(
    __global const ELEMENT_TYPE *A,
    __global const ELEMENT_TYPE *B,
    __global ELEMENT_TYPE *C)
{
  long i = get_global_id(0);
  long j = get_global_id(1);
  long N = get_global_size(0);
  C[i * N + j] = A[i * N + j] + B[i * N + j];
}
