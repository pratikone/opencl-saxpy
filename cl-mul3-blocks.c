#include "timing.h"
#include "main-helper.h"
#include "cl-helper.h"


//#define BLOCK_SIZE 4

unsigned int uiWA, uiHA, uiWB, uiHB, uiWC, uiHC;


int main(int argc, char **argv)
{
  if (argc != 4)
  {
    fprintf(stderr, "USO: %s  <dimension>  <vueltas>  <bloque>\n", argv[0]);
    abort();
  }

  const cl_long n = atol(argv[1]);
  const int ntrips = atoi(argv[2]);
  const int BLOCK_SIZE = atoi(argv[3]);

  cl_context ctx;
  cl_command_queue queue;

  // create_context_on(CHOOSE_INTERACTIVELY, CHOOSE_INTERACTIVELY, 0, &ctx, &queue, 0);
  // print_device_info_from_queue(queue);

  print_platforms_devices();
  create_context_on(NULL, NULL, 0, &ctx, &queue, 0);

  // --------------------------------------------------------------------------
  // load kernels 
  // --------------------------------------------------------------------------
  char *knl_text = read_file("mat-mul-blocks.cl");
  cl_kernel knl = kernel_from_string(ctx, knl_text, "mul", NULL);
  free(knl_text);

  // --------------------------------------------------------------------------
  // allocate and initialize CPU memory
  // --------------------------------------------------------------------------

  cl_long sizeN = n*n;
  ELEMENT_TYPE *h_A_data = (ELEMENT_TYPE *) malloc(sizeof(ELEMENT_TYPE) * sizeN);
  if (!h_A_data) { perror("alloc x"); abort(); }
  ELEMENT_TYPE *h_B_data = (ELEMENT_TYPE *) malloc(sizeof(ELEMENT_TYPE) * sizeN);
  if (!h_B_data) { perror("alloc y"); abort(); }
  ELEMENT_TYPE *h_C = (ELEMENT_TYPE *) malloc(sizeof(ELEMENT_TYPE) * sizeN);
  if (!h_C) { perror("alloc z"); abort(); }

  srand(2006);
  randomInit(h_A_data, sizeN);
  randomInit(h_B_data, sizeN);
  zeroInit(h_C, sizeN);

  // --------------------------------------------------------------------------
  // allocate device memory
  // --------------------------------------------------------------------------
  cl_int status;
  cl_mem h_A = clCreateBuffer(ctx, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 
      sizeof(ELEMENT_TYPE) * sizeN, h_A_data, &status);
  CHECK_CL_ERROR(status, "clCreateBuffer");

  // cl_mem h_B = clCreateBuffer(ctx, CL_MEM_READ_WRITE,
  //     sizeof(ELEMENT_TYPE) * sizeN, 0, &status);
  // CHECK_CL_ERROR(status, "clCreateBuffer");

  // cl_mem h_C = clCreateBuffer(ctx, CL_MEM_READ_WRITE,
  //     sizeof(ELEMENT_TYPE) * sizeN, 0, &status);
  // CHECK_CL_ERROR(status, "clCreateBuffer");

  // --------------------------------------------------------------------------
  // transfer to device
  // --------------------------------------------------------------------------
  
  // CALL_CL_GUARDED(clEnqueueWriteBuffer, (
  //       queue, buf_a, /*blocking*/ CL_TRUE, /*offset*/ 0,
  //       sizeN * sizeof(ELEMENT_TYPE), a,
  //       0, NULL, NULL));

  // CALL_CL_GUARDED(clEnqueueWriteBuffer, (
  //       queue, buf_b, /*blocking*/ CL_TRUE, /*offset*/ 0,
  //       sizeN * sizeof(ELEMENT_TYPE), b,
  //       0, NULL, NULL));

  // --------------------------------------------------------------------------
  // run code on device
  // --------------------------------------------------------------------------

  //! Globals for size of matrices (uiWA, uiHA, uiWB, uiHB, uiWC, uiHC)
  uiWA = uiHA = uiWB = uiHB = uiWC = uiHC = n;

  int worksize = uiHA;
  int workOffset = 0;

  cl_mem d_A = clCreateBuffer(ctx, CL_MEM_READ_ONLY, worksize * sizeof(ELEMENT_TYPE) * uiWA, NULL,NULL);

  // Copy only assigned rows from host to device
  CALL_CL_GUARDED(clEnqueueCopyBuffer,
      (queue, h_A, d_A, workOffset * sizeof(ELEMENT_TYPE) * uiWA, 
      0, worksize * sizeof(ELEMENT_TYPE) * uiWA, 0, NULL, NULL));
  
  // create OpenCL buffer on device that will be initiatilize
  // from the host memory on first use on device
  cl_mem d_B = clCreateBuffer(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                          sizeof(ELEMENT_TYPE) * sizeN, h_B_data, NULL);

  // Output buffer
  cl_mem d_C = clCreateBuffer(ctx, CL_MEM_WRITE_ONLY, worksize * n * sizeof(ELEMENT_TYPE), NULL, NULL);

  // SET_8_KERNEL_ARGS(knl, d_C, d_A, d_B, 0, 0, uiWA, uiWB, worksize);
  clSetKernelArg(knl, 0, sizeof(cl_mem), (void *) &d_C);
  clSetKernelArg(knl, 1, sizeof(cl_mem), (void *) &d_A);
  clSetKernelArg(knl, 2, sizeof(cl_mem), (void *) &d_B);
  clSetKernelArg(knl, 3, sizeof(ELEMENT_TYPE) * BLOCK_SIZE *BLOCK_SIZE, 0 );
  clSetKernelArg(knl, 4, sizeof(ELEMENT_TYPE) * BLOCK_SIZE *BLOCK_SIZE, 0 );
  clSetKernelArg(knl, 5, sizeof(cl_int), (void *) &BLOCK_SIZE);
  clSetKernelArg(knl, 6, sizeof(cl_int), (void *) &uiWA);
  clSetKernelArg(knl, 7, sizeof(cl_int), (void *) &uiWB);
  clSetKernelArg(knl, 8, sizeof(cl_int), (void *) &worksize);

    
  // Launch kernels on devices
  size_t localWorkSize[] = {BLOCK_SIZE, BLOCK_SIZE};
  size_t globalWorkSize[] = {shrRoundUp(BLOCK_SIZE, uiWC), shrRoundUp(BLOCK_SIZE, worksize)};
  globalWorkSize[1] = shrRoundUp(BLOCK_SIZE, worksize);

  CALL_CL_GUARDED(clFinish, (queue));

  timestamp_type time1, time2;
  get_timestamp(&time1);

  for (int trip = 0; trip < ntrips; ++trip)
  {
    CALL_CL_GUARDED(clEnqueueNDRangeKernel,
        (queue, knl,
         /*dimensions*/ 2, NULL, globalWorkSize, localWorkSize,
         0, NULL, NULL));
    
    CALL_CL_GUARDED(clFlush, (queue));
  }

  CALL_CL_GUARDED(clFinish, (queue));

  get_timestamp(&time2);
  printStatistics(time1, time2, ntrips, n);

  // --------------------------------------------------------------------------
  // transfer back & check
  // --------------------------------------------------------------------------

  if (! getenv("HIDE_CHECK_RESULTS"))
  {
    CALL_CL_GUARDED(clEnqueueReadBuffer, (
          queue, d_C, /*blocking*/ CL_TRUE, /*offset*/ 0,
          uiWC * sizeof(ELEMENT_TYPE) * worksize,
          h_C + workOffset * uiWC,
          0, NULL, NULL));

    // CALL_CL_GUARDED(clEnqueueReadBuffer, (
    //       queue, buf_c, /*blocking*/ CL_TRUE, /*offset*/ 0,
    //       sizeN * sizeof(ELEMENT_TYPE), c,
    //       0, NULL, NULL));

    printCheckResults(h_A_data,h_B_data,h_C,n);
  }

  // --------------------------------------------------------------------------
  // clean up
  // --------------------------------------------------------------------------
  CALL_CL_GUARDED(clReleaseMemObject, (d_A));
  CALL_CL_GUARDED(clReleaseMemObject, (d_B));
  CALL_CL_GUARDED(clReleaseMemObject, (d_C));
  CALL_CL_GUARDED(clReleaseKernel, (knl));
  CALL_CL_GUARDED(clReleaseCommandQueue, (queue));
  CALL_CL_GUARDED(clReleaseContext, (ctx));

  free(h_A_data);
  free(h_B_data);
  free(h_C);
  return 0;
}
