#ifndef DISPATCH_UNIT_HPP
#define DISPATCH_UNIT_HPP

#include "procsim.hpp"
#include <queue>
#include <cstdint>

class DispatchUnit
{
public:
    DispatchUnit();

    // Add an instruction to the dispatch queue
    void add_instruction(_proc_inst_t *inst, uint64_t cycle_count);

    // Check if the dispatch queue is empty
    bool is_empty() const;

    // Get the next instruction from the queue without removing it
    _proc_inst_t *peek_next() const;

    // Remove and return the next instruction from the queue
    _proc_inst_t *get_next();

    // Get the current size of the dispatch queue
    uint64_t get_size() const;

    // Get the maximum size the dispatch queue has reached
    uint64_t get_max_size() const;

private:
    // Queue to store instructions
    std::queue<_proc_inst_t *> dispatch_queue;

    // Track maximum queue size for statistics
    uint64_t max_size;
};

#endif // DISPATCH_UNIT_HPP