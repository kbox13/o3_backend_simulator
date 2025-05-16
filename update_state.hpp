#ifndef UPDATE_STATE_HPP
#define UPDATE_STATE_HPP

#include "procsim.hpp"
#include "functional_unit.hpp"
#include "schedule_queue.hpp"
#include "register_file.hpp"
#include "rob.hpp"
#include <vector>
#include <cstdint>

class UpdateState
{
public:
    // Constructor takes the number of result buses
    UpdateState(uint64_t num_result_buses);

    // Process one cycle of state updates
    void tick_first_half(uint64_t cycle_count,
                         FunctionalUnit &k0_unit,
                         FunctionalUnit &k1_unit,
                         FunctionalUnit &k2_unit,
                         ScheduleQueue &schedule_queue,
                         RegisterFile &register_file,
                         Rob &rob);

    void tick_second_half(uint64_t cycle_count,
                          FunctionalUnit &k0_unit,
                          FunctionalUnit &k1_unit,
                          FunctionalUnit &k2_unit,
                          ScheduleQueue &schedule_queue,
                          RegisterFile &register_file,
                          Rob &rob);

    // Get the number of instructions retired in the last cycle
    uint64_t get_retired_count() const;

private:
    // Maximum number of instructions that can be updated per cycle
    uint64_t max_updates;

    // Number of instructions retired in the last cycle
    uint64_t retired_count_cycle;

    // total number of instructions retired
    uint64_t retired_count_global;

    // Vector to store completed instructions in order
    std::vector<_proc_inst_t *> completed_instructions;

    // Vector to store instructions to be removed from functional units
    std::vector<_proc_inst_t *> instructions_to_remove;

    // Helper function to check if an instruction is complete
    bool is_instruction_complete(const _proc_inst_t *inst) const;
};

#endif // UPDATE_STATE_HPP