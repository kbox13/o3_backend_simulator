#include "rob.hpp"
#include "debug.hpp"

Rob::Rob() : inst_to_retire_count(1) {}

void Rob::tick()
{
    // Sort the rob_insts vector by instruction number
    std::sort(rob_insts.begin(), rob_insts.end(),
              [](const _proc_inst_t *a, const _proc_inst_t *b)
              {
                  return a->inst_number < b->inst_number;
              });

    // Create a vector to store instructions to remove
    std::vector<_proc_inst_t *> to_remove;

    // Check all instructions in order
    for (auto inst : rob_insts)
    {
        DBG_PRINT(stdout, "inst in rob: %llu, inst_to_retire_count: %llu\n", inst->inst_number, inst_to_retire_count);
        if (inst->inst_number == inst_to_retire_count)
        {
            print_inst(inst);
            inst_to_retire_count++;
            to_remove.push_back(inst);
        }
        else
        {
            // If we find an instruction that's not ready to retire, stop checking
            break;
        }
    }

    // Remove all instructions that were marked for removal
    for (auto inst : to_remove)
    {
        rob_insts.erase(std::remove(rob_insts.begin(), rob_insts.end(), inst), rob_insts.end());
    }
}

void Rob::add_inst(_proc_inst_t *inst)
{
    rob_insts.push_back(inst);
}

void Rob::print_inst(_proc_inst_t *inst)
{
    OUTPUT_PRINT(stdout, "%llu\t%llu\t%llu\t%llu\t%llu\t%llu\n",
                 inst->inst_number, inst->fetch_cycle, inst->disp_cycle, inst->schedule_cycle,
                 inst->execute_cycle, inst->state_cycle);
}

uint64_t Rob::get_size() const
{
    return rob_insts.size();
}