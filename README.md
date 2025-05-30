# Processor Simulator

A detailed processor simulator that models the backend of a superscalar out-of-order processor with various functional units.
Computer Assignment Report can be found at CA2-Report.pdf

## Project Structure

### Core Components

- `processor.hpp/cpp`: Main processor class implementing the pipeline stages
- `procsim.hpp/cpp`: Processor simulator interface and configuration
- `procsim_driver.cpp`: Main driver program
- `dispatch_unit.hpp/cpp`: Instruction dispatch unit implementation
- `functional_unit.hpp/cpp`: Functional unit implementation
- `register_file.hpp/cpp`: Register file implementation
- `rob.hpp/cpp`: Reorder buffer implementation
- `schedule_queue.hpp/cpp`: Schedule queue implementation
- `update_state.hpp/cpp`: State update logic implementation

### Scripts

1. `run_all.sh`
   - Runs all traces with default configuration
   - Configuration: R=2, J=3, K=2, L=1, F=4
   - Compares outputs with golden outputs
   - Usage: `./run_all.sh`

2. `run_experiment.sh`
   - Tests multiple parameter configurations
   - Parameter ranges:
     - R: 2, 4, 8 (Result Busses)
     - J: 1, 2, 3 (k0)
     - K: 1, 2, 3 (k1)
     - L: 1, 2, 3 (k2)
     - F: 2, 4, 8 (fetch width)
   - Organizes results in two ways:
     - By configuration: `results/experiments/R2_J3_K2_L1_F4/`
     - By trace: `results/gcc/`, `results/gobmk/`, etc.
   - Usage: `./run_experiment.sh`

### Build System

- `Makefile`: Build configuration
  - Build: `make build`
  - Clean: `make clean`
  - Run: `make run`
  - Run Debug: `make run-debug`
  - Run Log: `make run-log`
  - Run Output: `make run-output`
  - Run a specific trace `make run TRACE=<trace_name>`

## Configuration Parameters

- `-r`: Number of Result Busses
- `-j`: Number of k0 Functional Units
- `-k`: Number of k1 Functional Units
- `-l`: Number of k2 Functional Units
- `-f`: Fetch Width
- `-v`: Enable Logging
- `-d`: Enable Debug
- `-o`: Enable Output


## Traces

The simulator supports the following traces:
- gcc
- gobmk
- hmmer
- mcf

## Output Organization

1. Configuration-based organization:
```
results/experiments/
└── R2_J3_K2_L1_F4/
    ├── gcc.output
    ├── gobmk.output
    ├── hmmer.output
    └── mcf.output
```

2. Trace-based organization:
```
results/
├── gcc/
│   ├── R2_J1_K1_L1_F2.output
│   ├── R2_J1_K1_L1_F4.output
│   └── ...
├── gobmk/
└── ...
```

## Building and Running

1. Build the simulator:
```bash
make
```

2. Run all traces with default configuration and check against golden:
```bash
./run_all.sh
```

3. Run experiments with different configurations:
```bash
./run_experiment.sh
```

4. Run a specific trace:
```bash
./procsim -r2 -f4 -j3 -k2 -l1 < traces/gcc.100k.trace > results/gcc.output
```

## Dependencies

- C++ compiler with C++11 support
- Make
- Bash shell

## Notes

- The simulator implements a superscalar out-of-order processor
- Supports multiple functional units and pipeline stages
- Includes reorder buffer for maintaining instruction order
- Implements register renaming and dynamic scheduling
- Provides detailed performance statistics and pipeline state information 
