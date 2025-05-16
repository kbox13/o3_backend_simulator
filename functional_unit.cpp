#include "functional_unit.hpp"
#include "debug.hpp"
#include <algorithm>
#include <cmath>

FunctionalUnit::FunctionalUnit(uint64_t num_instructions, uint64_t latency, uint8_t opcode)
    : capacity(num_instructions), inst_latency(latency), opcode(opcode)
{
    // Reserve space in vectors to avoid reallocations
    current_inst_count = 0;
    executing_instructions.reserve(capacity);
    remaining_cycles.reserve(capacity);
}

bool FunctionalUnit::add_instruction(_proc_inst_t *inst)
{
    if (is_full())
    {
        return false;
        DBG_PRINT(stdout, "functional unit is full \n");
    }

    executing_instructions.push_back(inst);
    remaining_cycles.push_back(inst_latency);
    current_inst_count++;
    DBG_PRINT(stdout, "inst: %llu added to functional unit \n", inst->inst_number);
    return true;
}

bool FunctionalUnit::is_full() const
{
    DBG_PRINT(stdout, "current_inst_count: %llu capacity: %llu \n", current_inst_count, capacity);
    return current_inst_count >= capacity;
}

void FunctionalUnit::tick(uint64_t cycle_count, std::vector<_proc_inst_t *> &ready_queue)
{
    DBG_PRINT(stdout, "current_inst_count: %llu on cycle: %llu\n", current_inst_count, cycle_count);
    // process ready queue to add instructions to the functional unit from the schedule queue
    process_ready_queue(ready_queue);

    // process instructions
    for (auto it = executing_instructions.begin(); it != executing_instructions.end(); ++it)
    {
        size_t index = std::distance(executing_instructions.begin(), it);
        DBG_PRINT(stdout, "%llu inst: %llu remaining cycles: %d op_code: %d \n",
                  cycle_count, (*it)->inst_number, remaining_cycles[index], (*it)->op_code);

        if (remaining_cycles[index] > 0)
        {
            remaining_cycles[index]--;
        }
        // If instruction is complete, mark execution as executed
        if (remaining_cycles[index] == 0 && (*it)->execute_cycle == 0)
        {
            (*it)->execute_cycle = cycle_count;
            LOG_PRINT(stdout, "%llu EXECUTED %llu \n", cycle_count, (*it)->inst_number);
        }
    }
}

uint64_t FunctionalUnit::get_current_instructions() const
{
    return current_inst_count;
}

uint64_t FunctionalUnit::get_capacity() const
{
    return capacity;
}

_proc_inst_t *FunctionalUnit::get_instruction(size_t index) const
{
    if (index < executing_instructions.size())
    {
        return executing_instructions[index];
    }
    return nullptr;
}

void FunctionalUnit::remove_instruction(_proc_inst_t *inst)
{
    auto it = std::find(executing_instructions.begin(), executing_instructions.end(), inst);
    if (it != executing_instructions.end())
    {
        size_t index = std::distance(executing_instructions.begin(), it);
        executing_instructions.erase(it);
        remaining_cycles.erase(remaining_cycles.begin() + index);
        current_inst_count--;
        DBG_PRINT(stdout, "inst: %llu removed from functional unit \n", inst->inst_number);
    }
}

void FunctionalUnit::process_ready_queue(std::vector<_proc_inst_t *> &ready_queue)
{
    DBG_PRINT(stdout, "ready queue size: %zu \n", ready_queue.size());
    // Iterate through ready queue
    for (auto inst : ready_queue)
    {
        // Check if instruction matches this FU's opcode and FU is not full
        if (std::abs(inst->op_code) == opcode && !is_full() && !inst->fired)
        {
            if (add_instruction(inst))
            {
                DBG_PRINT(stdout, "inst: %llu added to functional unit %d\n",
                          inst->inst_number, opcode);
                inst->fired = true;
            }
        }
    }
}
