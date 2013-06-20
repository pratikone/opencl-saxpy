#!/bin/bash
N=50
T=10

export CL_HELPER_NO_COMPILER_OUTPUT_NAG=1

echo ""
echo "----- Traditional Multiplication:"
bin/cl-mul0 $N $T
echo ""
echo "----- Multiplication with SAXPY:"
bin/cl-mul0-saxpy $N $T
echo ""
echo "----- OpenCL Traditional Multiplication:"
bin/cl-mul1 $N $T
echo ""
echo "----- OpenCL SAXPY Multiplication:"
bin/cl-mul2 $N $T
