#include "dispatch_unit.hpp"
#include "debug.hpp"

DispatchUnit::DispatchUnit() : max_size(0) {}

void DispatchUnit::add_instruction(_proc_inst_t *inst, uint64_t cycle_count)
{
    dispatch_queue.push(inst);
    inst->disp_cycle = cycle_count;
    LOG_PRINT(stdout, "%llu DISPATCHED %llu \n", cycle_count, inst->inst_number);
    DBG_PRINT(stdout, "inst: %llu added to dispatch queue \n", inst->inst_number);

    // Update max size if current size is larger
    uint64_t current_size = dispatch_queue.size();
    if (current_size > max_size)
    {
        max_size = current_size;
    }
}

bool DispatchUnit::is_empty() const
{
    return dispatch_queue.empty();
}

_proc_inst_t *DispatchUnit::peek_next() const
{
    if (dispatch_queue.empty())
    {
        return nullptr;
    }
    return dispatch_queue.front();
}

_proc_inst_t *DispatchUnit::get_next()
{
    if (dispatch_queue.empty())
    {
        return nullptr;
    }

    _proc_inst_t *inst = dispatch_queue.front();
    dispatch_queue.pop();
    return inst;
}

uint64_t DispatchUnit::get_size() const
{
    return dispatch_queue.size();
}

uint64_t DispatchUnit::get_max_size() const
{
    return max_size;
}
