#!/bin/bash

# Check if two integer arguments are provided
if [ $# -ne 2 ]; then
    echo "Usage: $0 n p"
    exit 1
fi

# Read the two integer arguments
n=$1
p=$2

# Calculate the array size and number of processes
arraySize=$((2**n))
nprocs=$((arraySize/2**p))

echo "Array size: $arraySize"
echo "Number of processes: $nprocs"

# Compile the C code with MPI support
mpicc q2.c -o q2 -lm

# Run the MPI program with calculated arguments
mpirun -n $nprocs ./q2 $arraySize
