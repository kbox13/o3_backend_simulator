#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP
#include <cstdint>
#include <cstdio>
#include <vector>
#include "procsim.hpp"
#include "functional_unit.hpp"
#include "dispatch_unit.hpp"
#include "register_file.hpp"
#include "schedule_queue.hpp"
#include "update_state.hpp"

class Processor
{
public:
    Processor(uint64_t r, uint64_t k0, uint64_t k1, uint64_t k2, uint64_t f);
    void output_header() const;
    uint64_t get_fetch_rate() const;
    _proc_inst_t *get_inst_ptr(int index) const;
    void set_valid_inst(bool inst_valid);
    void incr_inst_cntr();
    uint64_t get_inst_cntr() const;
    bool running() const;
    void tick();
    void clean_up();
    uint64_t get_cycle_counter() const;
    uint64_t get_retired_insts() const;
    void incr_cycle_counter();
    bool has_pending_instructions() const;
    // Getters for stats
    uint64_t get_total_dispatch_size() const;
    uint64_t get_max_dispatch_size() const;

private:
    uint64_t fetch_rate;
    uint64_t inst_counter;
    uint64_t cycle_counter;
    uint64_t retired_insts;
    // vector to prefetch instructions in procsim using read instrcutions
    std::vector<_proc_inst_t *> pre_fetched_insts;
    bool valid_insts;

    // stats
    uint64_t total_dispatch_size;
    uint64_t total_insts_fired;
    uint64_t total_insts_retired;

    // Dispatch unit
    DispatchUnit dispatch_unit;

    // Register file
    RegisterFile register_file;

    // Schedule queue
    ScheduleQueue schedule_queue;

    // Functional units
    FunctionalUnit k0_unit;
    FunctionalUnit k1_unit;
    FunctionalUnit k2_unit;

    // Update state unit
    UpdateState update_state;

    // Reorder buffer
    Rob rob;
};

#endif