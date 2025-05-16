#include "processor.hpp"
#include <cstdint>
#include <cstdio>
#include <iostream>
#include "debug.hpp"
Processor::Processor(uint64_t r, uint64_t k0, uint64_t k1, uint64_t k2, uint64_t f)
    : fetch_rate(f), dispatch_unit(), register_file(), schedule_queue(k0, k1, k2),
      k0_unit(k0, 1, 0), k1_unit(k1, 1, 1), k2_unit(k2, 1, 2),
      update_state(r), rob()
{

    // resize the prefetch vector
    pre_fetched_insts.resize(fetch_rate);
    for (size_t i = 0; i < fetch_rate; ++i)
    {
        pre_fetched_insts[i] = new _proc_inst_t;
    }
    valid_insts = true;
    inst_counter = 0;
    cycle_counter = 1; // start at ticks once to allow for prefetch.
    retired_insts = 0;
    total_dispatch_size = 0;
    total_insts_fired = 0;
    total_insts_retired = 0;
    output_header();
}

void Processor::output_header() const
{
    OUTPUT_PRINT(stdout, "INST	FETCH	DISP	SCHED	EXEC	STATE\n");
}

bool Processor::running() const
{
    // stop running if any of the insts are not valid
    return valid_insts;
}

uint64_t Processor::get_fetch_rate() const
{
    return fetch_rate;
}

_proc_inst_t *Processor::get_inst_ptr(int index) const
{
    return pre_fetched_insts[index];
}

void Processor::set_valid_inst(bool inst_valid)
{
    valid_insts = inst_valid;
}

void Processor::incr_inst_cntr()
{
    inst_counter++;
}

uint64_t Processor::get_inst_cntr() const
{
    return inst_counter;
}

void Processor::tick()
{
    // Process reorder buffer
    rob.tick();

    // Process update state unit first to free up functional units
    update_state.tick_first_half(cycle_counter, k0_unit, k1_unit, k2_unit, schedule_queue, register_file, rob);
    retired_insts += update_state.get_retired_count();

    // update state fetch from fus and write to rf

    // Process each functional unit
    k0_unit.tick(cycle_counter, schedule_queue.get_ready_queue());
    k1_unit.tick(cycle_counter, schedule_queue.get_ready_queue());
    k2_unit.tick(cycle_counter, schedule_queue.get_ready_queue());

    // Process schedule queue
    schedule_queue.tick(cycle_counter, dispatch_unit, k0_unit, k1_unit, k2_unit, register_file);

    // dispatch unit tick
    // Move instructions from prefetched to dispatch queue
    for (int i = 0; i < fetch_rate; i++)
    {
        if (pre_fetched_insts[i] != nullptr && pre_fetched_insts[i]->inst_number != 0)
        {
            dispatch_unit.add_instruction(pre_fetched_insts[i], cycle_counter);
            // DBG_PRINT(stdout, "%llu %x\n", pre_fetched_insts[i]->inst_number, pre_fetched_insts[i]->instruction_address);
            pre_fetched_insts[i] = new _proc_inst_t;
        }
    }
    // try to reserve slots in the schedule queue for the enitre dispatch unit
    schedule_queue.reserve_slots(dispatch_unit.get_size());

    // update state delete from everything
    update_state.tick_second_half(cycle_counter, k0_unit, k1_unit, k2_unit, schedule_queue, register_file, rob);
    // stats
    total_dispatch_size += dispatch_unit.get_size();
}

uint64_t Processor::get_cycle_counter() const
{
    return cycle_counter;
}

void Processor::incr_cycle_counter()
{
    cycle_counter++;
}

uint64_t Processor::get_retired_insts() const
{
    return retired_insts;
}

void Processor::clean_up()
{
    // for now nothing to do
    // use ai to write this to clean up
    return;
}

bool Processor::has_pending_instructions() const
{
    // Check if any functional units have instructions
    if (k0_unit.get_current_instructions() > 0 ||
        k1_unit.get_current_instructions() > 0 ||
        k2_unit.get_current_instructions() > 0)
    {
        return true;
    }

    // Check if schedule queue has instructions
    if (schedule_queue.get_size() > 0)
    {
        return true;
    }

    // Check if dispatch unit has instructions
    if (!dispatch_unit.is_empty())
    {
        return true;
    }

    // check if rob has any instructions
    if (rob.get_size() > 0)
    {
        return true;
    }

    return false;
}

uint64_t Processor::get_total_dispatch_size() const
{
    return total_dispatch_size;
}

uint64_t Processor::get_max_dispatch_size() const
{
    return dispatch_unit.get_max_size();
}
