#!/bin/bash

# Configuration from Makefile
R=2
J=3
K=2
L=1
F=4

# Create results directory if it doesn't exist
mkdir -p results

# List of traces to run
TRACES=("gcc" "gobmk" "hmmer" "mcf")

# Run each trace
for TRACE in "${TRACES[@]}"; do
    echo "Running $TRACE..."
    ./procsim -o -r$R -f$F -j$J -k$K -l$L < traces/$TRACE.100k.trace > results/$TRACE.output
done

echo "All traces completed!" 
echo "Testing differences with golden outputs"

# Check all diffs and track if any failed
failed=0
for TRACE in "${TRACES[@]}"; do
    if ! diff -q ./results/$TRACE.output ./output1.1/$TRACE.output > /dev/null; then
        echo "$TRACE output differs from golden output"
        failed=1
    else
        echo "$TRACE output matches golden output"
    fi
done

if [ $failed -eq 0 ]; then
    echo "All outputs match golden outputs!"
else
    echo "Some outputs differ from golden outputs!"
fi

