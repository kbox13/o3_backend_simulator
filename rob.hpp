#ifndef UPDATE_ROB_HPP
#define UPDATE_ROB_HPP

#include "procsim.hpp"
#include <vector>
#include <cstdint>
#include <iostream>

class Rob
{
public:
    // Constructor takes the number of result buses
    Rob();

    // Process one cycle of state updates
    void tick();

    // Get the number of instructions retired in the last cycle
    void add_inst(_proc_inst_t *inst);

    // Get the current number of instructions in the ROB
    uint64_t get_size() const;

private:
    // Vector to store completed instructions in order
    std::vector<_proc_inst_t *> rob_insts;

    // current instruction to retire count
    uint64_t inst_to_retire_count;

    // print instruction stats for output file
    void print_inst(_proc_inst_t *inst);
};

#endif // UPDATE_ROB_HPP