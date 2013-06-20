#!/bin/bash
N=500
T=10

export CL_HELPER_NO_COMPILER_OUTPUT_NAG=1

bin/cl-mul0 $N $T
bin/cl-mul0-saxpy $N $T
bin/cl-mul1 $N $T
