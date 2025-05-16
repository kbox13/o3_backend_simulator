#include "update_state.hpp"
#include "debug.hpp"

UpdateState::UpdateState(uint64_t num_result_buses)
    : max_updates(num_result_buses), retired_count_global(0) {}

void UpdateState::tick_first_half(uint64_t cycle_count,
                                  FunctionalUnit &k0_unit,
                                  FunctionalUnit &k1_unit,
                                  FunctionalUnit &k2_unit,
                                  ScheduleQueue &schedule_queue,
                                  RegisterFile &register_file,
                                  Rob &rob)
{
    retired_count_cycle = 0;
    // save the old size of the completed instructions vector for later sorting
    size_t old_size = completed_instructions.size();
    // Check each functional unit for completed instructions and add them to the vector
    for (size_t i = 0; i < k0_unit.get_current_instructions(); i++)
    {
        _proc_inst_t *inst = k0_unit.get_instruction(i);
        if (is_instruction_complete(inst))
        {
            completed_instructions.push_back(inst);
        }
    }

    for (size_t i = 0; i < k1_unit.get_current_instructions(); i++)
    {
        _proc_inst_t *inst = k1_unit.get_instruction(i);
        if (is_instruction_complete(inst))
        {
            completed_instructions.push_back(inst);
        }
    }

    for (size_t i = 0; i < k2_unit.get_current_instructions(); i++)
    {
        _proc_inst_t *inst = k2_unit.get_instruction(i);
        if (is_instruction_complete(inst))
        {
            completed_instructions.push_back(inst);
        }
    }

    // sort only newly added instructions by instruction number
    std::sort(completed_instructions.begin() + old_size, completed_instructions.end(),
              [](const _proc_inst_t *a, const _proc_inst_t *b)
              {
                  return a->inst_number < b->inst_number;
              });

    while (retired_count_cycle < max_updates && !completed_instructions.empty())
    {
        _proc_inst_t *inst = completed_instructions.front();
        completed_instructions.erase(completed_instructions.begin());

        // Update the instruction's state cycle
        inst->state_cycle = cycle_count;
        LOG_PRINT(stdout, "%llu STATE UPDATE %llu \n", cycle_count, inst->inst_number);

        // Update schedule queue states
        if (inst->dest_reg >= 0)
        {
            DBG_PRINT(stdout, "instruction %llu src updating src reg \n", inst->inst_number);
            schedule_queue.make_src_reg_ready(inst->dest_reg, inst->inst_number);
            DBG_PRINT(stdout, "inst: %llu marking %d ready\n", inst->inst_number, inst->dest_reg);
            register_file.mark_ready(inst->dest_reg, inst->inst_number);
        }

        // Remove instruction from functional unit
        switch (std::abs(inst->op_code))
        {
        case 0:
            k0_unit.remove_instruction(inst);
            break;
        case 1:
            k1_unit.remove_instruction(inst);
            break;
        case 2:
            k2_unit.remove_instruction(inst);
            break;
        }

        // Remove instruction from schedule queue
        instructions_to_remove.push_back(inst);
        // increment the number of instructions retired in the last cycle
        retired_count_cycle++;
    }
}

void UpdateState::tick_second_half(uint64_t cycle_count,
                                   FunctionalUnit &k0_unit,
                                   FunctionalUnit &k1_unit,
                                   FunctionalUnit &k2_unit,
                                   ScheduleQueue &schedule_queue,
                                   RegisterFile &register_file,
                                   Rob &rob)
{
    // Process completed instructions up to max_updates
    for (auto inst : instructions_to_remove)
    {
        schedule_queue.remove_instruction(inst);
        retired_count_global++;

        // connect the instruction to the rob
        DBG_PRINT(stdout, "inst: %llu added to rob \n", inst->inst_number);
        rob.add_inst(inst);
    }
    instructions_to_remove.clear();
}

bool UpdateState::is_instruction_complete(const _proc_inst_t *inst) const
{
    // Check if instruction has executed and hasn't been processed yet
    // Also check if it's not already in the completed_instructions vector
    return inst->execute_cycle > 0 &&
           inst->state_cycle == 0 &&
           std::find(completed_instructions.begin(), completed_instructions.end(), inst) == completed_instructions.end();
}

uint64_t UpdateState::get_retired_count() const
{
    return retired_count_cycle;
}