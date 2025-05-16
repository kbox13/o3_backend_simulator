#ifndef SCHEDULE_QUEUE_HPP
#define SCHEDULE_QUEUE_HPP

#include "procsim.hpp"
#include "functional_unit.hpp"
#include "dispatch_unit.hpp"
#include "register_file.hpp"
#include <vector>
#include <cstdint>

class ScheduleQueue
{
public:
    // Constructor takes the number of functional units for each type
    ScheduleQueue(uint64_t k0_fus, uint64_t k1_fus, uint64_t k2_fus);

    // Process one cycle of scheduling
    void tick(uint64_t cycle_count, DispatchUnit &dispatch_unit,
              FunctionalUnit &k0_unit, FunctionalUnit &k1_unit, FunctionalUnit &k2_unit, RegisterFile &register_file);

    // Add an instruction to the schedule queue
    bool add_instruction(_proc_inst_t *inst, RegisterFile &register_file);

    // Check if the schedule queue is full
    bool is_full() const;

    // Get current number of instructions in the queue
    uint64_t get_size() const;

    // Get maximum size the queue has reached
    uint64_t get_max_size() const;

    // Update source register ready states when an instruction completes
    void make_src_reg_ready(int32_t reg_num, uint64_t inst_number);

    // Remove an instruction from the schedule queue
    void remove_instruction(_proc_inst_t *inst);

    // Get the ready queue
    std::vector<_proc_inst_t *> &get_ready_queue();

    // reserve slots in the schedule queue
    void reserve_slots(uint64_t num_slots);

private:
    // Maximum size of the schedule queue
    uint64_t max_size;

    // Current size of the queue
    uint64_t current_size;

    // Vector to store instructions in the schedule queue
    std::vector<_proc_inst_t *> schedule_queue;

    // Vector to store instructions that are ready to be executed
    std::vector<_proc_inst_t *> ready_queue;

    // Reserved slots in the schedule queue
    uint64_t reserved_slots;

    // Helper function to check if an instruction's source registers are ready
    bool are_src_regs_ready(const _proc_inst_t *inst) const;

    // Helper function to update source register ready states for an instruction
    void update_instruction_src_regs(_proc_inst_t *inst, const RegisterFile &register_file);

    // Helper function to dispatch ready instructions to functional units
    void schedule_ready_instructions(uint64_t cycle_count);
};

#endif // SCHEDULE_QUEUE_HPP