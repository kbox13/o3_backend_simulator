#!/bin/bash

# Define parameter ranges to test
R_VALUES=(2 3 4 5 6)
J_VALUES=(1 2)
K_VALUES=(1 2)
L_VALUES=(1 2)
F_VALUES=(4 8)

# List of traces to run
TRACES=("gcc" "gobmk" "hmmer" "mcf")

# Create results directories
mkdir -p results/experiments
for TRACE in "${TRACES[@]}"; do
    mkdir -p "results/$TRACE"
done

echo "----------------------------------------"
# Function to run a single configuration
run_config() {
    local r=$1
    local j=$2
    local k=$3
    local l=$4
    local f=$5
    
    echo "Testing configuration: R=$r, J=$j, K=$k, L=$l, F=$f"
    
    # Create configuration directory
    local config_dir="results/experiments/R${r}_J${j}_K${k}_L${l}_F${f}"
    mkdir -p "$config_dir"
    
    # Run each trace with this configuration
    for TRACE in "${TRACES[@]}"; do
        echo "Running $TRACE..."
        # Run the experiment once and save to both locations
        ./procsim -r$r -f$f -j$j -k$k -l$l < traces/$TRACE.100k.trace > "$config_dir/$TRACE.output"
        # Copy to trace-specific directory
        cp "$config_dir/$TRACE.output" "results/$TRACE/R${r}_J${j}_K${k}_L${l}_F${f}.output"
    done
    
    echo "----------------------------------------"
}

# Run all combinations
for r in "${R_VALUES[@]}"; do
    for j in "${J_VALUES[@]}"; do
        for k in "${K_VALUES[@]}"; do
            for l in "${L_VALUES[@]}"; do
                for f in "${F_VALUES[@]}"; do
                    run_config $r $j $k $l $f
                done
            done
        done
    done
done
