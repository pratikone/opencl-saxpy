#include "timing.h"
#include "cl-helper.h"


void randomInit(double* data, int size)
{
  for (int i = 0; i < size; ++i)
    data[i] = rand() / (float)RAND_MAX;
}

void printMatrix(double* data, int n)
{
  for(int i = 0; i < n*n; i++)
  {
    printf(" %.4f", data[i]);
    if(((i + 1) % n) == 0)
      printf(";\n");
  }
  printf("\n");
}

int main(int argc, char **argv)
{
  if (argc != 3)
  {
    fprintf(stderr, "USO: %s  <dimension>  <vueltas>\n", argv[0]);
    abort();
  }

  const cl_long n = atol(argv[1]);
  const int ntrips = atoi(argv[2]);

  cl_context ctx;
  cl_command_queue queue;

  // create_context_on(CHOOSE_INTERACTIVELY, CHOOSE_INTERACTIVELY, 0, &ctx, &queue, 0);
  // print_device_info_from_queue(queue);

  print_platforms_devices();
  create_context_on(NULL, NULL, 0, &ctx, &queue, 0);

  // --------------------------------------------------------------------------
  // load kernels 
  // --------------------------------------------------------------------------
  char *knl_text = read_file("mat-mul.cl");
  cl_kernel knl = kernel_from_string(ctx, knl_text, "mul", NULL);
  free(knl_text);

  // --------------------------------------------------------------------------
  // allocate and initialize CPU memory
  // --------------------------------------------------------------------------

  cl_long sizeN = n*n;
  double *a = (double *) malloc(sizeof(double) * sizeN);
  if (!a) { perror("alloc x"); abort(); }
  double *b = (double *) malloc(sizeof(double) * sizeN);
  if (!b) { perror("alloc y"); abort(); }
  double *c = (double *) malloc(sizeof(double) * sizeN);
  if (!c) { perror("alloc z"); abort(); }

  srand(2006);
  randomInit(a, sizeN);
  randomInit(b, sizeN);

  // --------------------------------------------------------------------------
  // allocate device memory
  // --------------------------------------------------------------------------
  cl_int status;
  cl_mem buf_a = clCreateBuffer(ctx, CL_MEM_READ_WRITE, 
      sizeof(double) * sizeN, 0, &status);
  CHECK_CL_ERROR(status, "clCreateBuffer");

  cl_mem buf_b = clCreateBuffer(ctx, CL_MEM_READ_WRITE,
      sizeof(double) * sizeN, 0, &status);
  CHECK_CL_ERROR(status, "clCreateBuffer");

  cl_mem buf_c = clCreateBuffer(ctx, CL_MEM_READ_WRITE,
      sizeof(double) * sizeN, 0, &status);
  CHECK_CL_ERROR(status, "clCreateBuffer");

  // --------------------------------------------------------------------------
  // transfer to device
  // --------------------------------------------------------------------------
  CALL_CL_GUARDED(clEnqueueWriteBuffer, (
        queue, buf_a, /*blocking*/ CL_TRUE, /*offset*/ 0,
        sizeN * sizeof(double), a,
        0, NULL, NULL));

  CALL_CL_GUARDED(clEnqueueWriteBuffer, (
        queue, buf_b, /*blocking*/ CL_TRUE, /*offset*/ 0,
        sizeN * sizeof(double), b,
        0, NULL, NULL));

  // --------------------------------------------------------------------------
  // run code on device
  // --------------------------------------------------------------------------

  CALL_CL_GUARDED(clFinish, (queue));

  timestamp_type time1, time2;
  get_timestamp(&time1);

  for (int trip = 0; trip < ntrips; ++trip)
  {
    SET_4_KERNEL_ARGS(knl, buf_a, buf_b, buf_c, n);
    // size_t ldim[] = { 128 };
    // size_t gdim[] = { ((n + ldim[0] - 1)/ldim[0])*ldim[0] };
    
    size_t localWorkSize[]  = {n,n};
    size_t globalWorkSize[] = {n,n};
    
    CALL_CL_GUARDED(clEnqueueNDRangeKernel,
        (queue, knl,
         /*dimensions*/ 2, NULL, globalWorkSize, localWorkSize, //gdim, ldim,
         0, NULL, NULL));
  }

  CALL_CL_GUARDED(clFinish, (queue));

  get_timestamp(&time2);
  double elapsed = timestamp_diff_in_seconds(time1,time2)/ntrips;
  printf("%f s\n", elapsed);
  printf("%f GB/s\n",
      3*sizeN*sizeof(double)/1e9/elapsed);

  // --------------------------------------------------------------------------
  // transfer back & check
  // --------------------------------------------------------------------------
  CALL_CL_GUARDED(clEnqueueReadBuffer, (
        queue, buf_c, /*blocking*/ CL_TRUE, /*offset*/ 0,
        sizeN * sizeof(double), c,
        0, NULL, NULL));

  printf("\nMatrix A\n");
  printMatrix(a, n);
  
  printf("\nMatrix B\n");
  printMatrix(b, n);
  
  printf("\nMatrix C = A · B\n");
  printMatrix(c, n);

  // --------------------------------------------------------------------------
  // clean up
  // --------------------------------------------------------------------------
  CALL_CL_GUARDED(clReleaseMemObject, (buf_a));
  CALL_CL_GUARDED(clReleaseMemObject, (buf_b));
  CALL_CL_GUARDED(clReleaseMemObject, (buf_c));
  CALL_CL_GUARDED(clReleaseKernel, (knl));
  CALL_CL_GUARDED(clReleaseCommandQueue, (queue));
  CALL_CL_GUARDED(clReleaseContext, (ctx));

  return 0;
}