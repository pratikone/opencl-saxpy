#!/bin/bash
N=400
T=10

MEM=$(echo "$N*$N*4*3/1024/1024" | bc -l | awk '{printf "%08f\n", $0}')

make

echo ""
echo "==== Matrices size: ${N}x${N}"
echo "---- Trip Count:    $T"
echo "---- Memory Used:   $MEM MB"
echo ""
echo "!! Make CPU use Performance Governor for testing !!"
echo ""
echo "-- Press Enter to Begin..."
read

export CL_HELPER_NO_COMPILER_OUTPUT_NAG=1

echo ""
echo "----- 1) Traditional Multiplication:"
bin/cl-mul0 $N $T

echo ""
echo "----- 2) Multiplication using SAXPY:"
bin/cl-mul0-saxpy $N $T

echo ""
echo "----- 3) Multiplication using Blocks:"
echo "TODO"
#bin/cl-mul0-blocks $N $T

echo ""
echo "----- 4) OpenCL Traditional Multiplication:"
bin/cl-mul1 $N $T

echo ""
echo "----- 5) OpenCL Multiplication using SAXPY:"
echo "TODO"
#bin/cl-mul2 $N $T

echo ""
echo "----- 6) OpenCL Multiplication using Blocks:"
bin/cl-mul3-blocks $N $T
