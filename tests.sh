#!/bin/bash
N=500
T=10

MEM=$(echo "$N*$N*4*3/1024/1024" | bc -l | awk '{printf "%08f\n", $0}')
echo ""
echo "==== Matrix size: ${N}x${N}"
echo "---- Trip Count:  $T"
echo "---- Memory:      $MEM MB"
echo ""
echo "!! Make CPU use Performance Governor !!"
echo "-- Press Enter to Begin..."
read

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
