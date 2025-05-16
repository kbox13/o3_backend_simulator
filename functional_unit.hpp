#ifndef FUNCTIONAL_UNIT_HPP
#define FUNCTIONAL_UNIT_HPP

#include "procsim.hpp"
#include <vector>
#include <cstdint>

class FunctionalUnit
{
public:
    // Constructor takes the number of instructions this FU can handle simultaneously
    FunctionalUnit(uint64_t num_instructions, uint64_t latency, uint8_t opcode);

    // Add an instruction to the functional unit
    bool add_instruction(_proc_inst_t *inst);

    // Check if the functional unit is full
    bool is_full() const;

    // Process one cycle of execution
    void tick(uint64_t cycle_count, std::vector<_proc_inst_t *> &ready_queue);

    // Get the number of instructions currently in the FU
    uint64_t get_current_instructions() const;

    // Get the number of instructions this FU can handle
    uint64_t get_capacity() const;

    // Get an instruction at a specific index
    _proc_inst_t *get_instruction(size_t index) const;

    // Remove an instruction from the functional unit
    void remove_instruction(_proc_inst_t *inst);

    // Process instructions from the ready queue that match this FU's opcode
    void process_ready_queue(std::vector<_proc_inst_t *> &ready_queue);

private:
    // Maximum number of instructions this FU can handle
    uint64_t capacity;

    // Number of instructions currently in the FU
    uint64_t current_inst_count;

    // Time it takes to execute an instruction
    uint64_t inst_latency;

    // opcode of the functional unit
    uint8_t opcode;

    // Vector to store instructions currently being executed
    std::vector<_proc_inst_t *> executing_instructions;

    // Vector to track remaining cycles for each instruction
    std::vector<int8_t> remaining_cycles;
};

#endif // FUNCTIONAL_UNIT_HPP
