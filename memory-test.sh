#!/bin/bash
make

echo ""
echo "!! Make CPU use Performance Governor for testing !!"
echo ""
echo "-- Press Enter to Begin..."
read

for ((i=2; i<12; i++))
do
    N=$((i*1000))
    if [ ! -z $1 ]; then
        N=$1
    fi
    T=30

    MEM=$(echo "$N*$N*4*3/1024/1024" | bc -l | awk '{printf "%08f\n", $0}')

    echo ""
    echo "==== Matrices size: ${N}x${N}"
    echo "---- Trip Count:    $T"
    echo "---- Memory Used:   $MEM MB"

    export CL_HELPER_NO_COMPILER_OUTPUT_NAG=1
    export HIDE_CHECK_RESULTS=1

    bin/memory $N $T
done
