#include "schedule_queue.hpp"
#include "debug.hpp"

ScheduleQueue::ScheduleQueue(uint64_t k0_fus, uint64_t k1_fus, uint64_t k2_fus)
    : max_size(2 * (k0_fus + k1_fus + k2_fus)), current_size(0)
{
    schedule_queue.reserve(max_size);
    ready_queue.reserve(max_size);
}

void ScheduleQueue::tick(uint64_t cycle_count, DispatchUnit &dispatch_unit,
                         FunctionalUnit &k0_unit, FunctionalUnit &k1_unit, FunctionalUnit &k2_unit, RegisterFile &register_file)
{

    uint64_t num_slots_filled = 0;
    while (!dispatch_unit.is_empty() && !is_full() && num_slots_filled < reserved_slots)
    {
        DBG_PRINT(stdout, "num_slots_filled: %llu, reserved_slots: %llu \n", num_slots_filled, reserved_slots);
        DBG_PRINT(stdout, "schedule queue is full ? %llu, %llu \n", current_size, max_size);
        _proc_inst_t *inst = dispatch_unit.peek_next();
        DBG_PRINT(stdout, "inst: %llu peeked \n", inst->inst_number);
        if (add_instruction(inst, register_file))
        {
            dispatch_unit.get_next(); // Remove the instruction from dispatch queue
            inst->schedule_cycle = cycle_count;
            LOG_PRINT(stdout, "%llu SCHEDULED %llu \n", cycle_count, inst->inst_number);
            DBG_PRINT(stdout, "inst: %llu added to schedule queue \n", inst->inst_number);
        }
        else
        {
            DBG_PRINT(stdout, "inst: %llu not added to schedule queue \n", inst->inst_number);
            break; // Can't add more instructions
        }
        num_slots_filled++;
    }

    // schedule ready instructions
    schedule_ready_instructions(cycle_count);
}

bool ScheduleQueue::add_instruction(_proc_inst_t *inst, RegisterFile &register_file)
{
    if (is_full())
    {
        return false;
        DBG_PRINT(stdout, "schedule queue is full \n");
    }

    // Update source register ready states
    update_instruction_src_regs(inst, register_file);

    // Mark destination register as busy
    if (inst->dest_reg >= 0)
    {
        register_file.mark_busy(inst->dest_reg, inst->inst_number);
    }

    // Add instruction to schedule queue
    schedule_queue.push_back(inst);
    DBG_PRINT(stdout, "inst: %llu added to schedule queue \n", inst->inst_number);
    current_size++;

    return true;
}

void ScheduleQueue::update_instruction_src_regs(_proc_inst_t *inst, const RegisterFile &register_file)
{
    // Get the instruction numbers using each source register
    if (inst->src_reg[0] >= 0)
    {
        inst->src_reg_ready[0] = register_file.get_recent_dependency(inst->src_reg[0]);
        DBG_PRINT(stdout, "inst: %llu src reg 0: %llu \n", inst->inst_number, inst->src_reg_ready[0]);
    }
    if (inst->src_reg[1] >= 0)
    {
        inst->src_reg_ready[1] = register_file.get_recent_dependency(inst->src_reg[1]);
        DBG_PRINT(stdout, "inst: %llu src reg 1: %llu \n", inst->inst_number, inst->src_reg_ready[1]);
    }
}

bool ScheduleQueue::is_full() const
{
    DBG_PRINT(stdout, "schedule queue is full %d \n", current_size >= max_size);
    return current_size >= max_size;
}

uint64_t ScheduleQueue::get_size() const
{
    return current_size;
}

uint64_t ScheduleQueue::get_max_size() const
{
    return max_size;
}

void ScheduleQueue::make_src_reg_ready(int32_t reg_num, uint64_t inst_number)
{
    // Update source register ready states for all instructions in the queue
    DBG_PRINT(stdout, "updating src reg %d for inst %llu\n", reg_num, inst_number);
    for (auto inst : schedule_queue)
    {
        if (inst->src_reg[0] == reg_num && inst->src_reg_ready[0] == inst_number)
        {
            inst->src_reg_ready[0] = 0;
        }
        if (inst->src_reg[1] == reg_num && inst->src_reg_ready[1] == inst_number)
        {
            inst->src_reg_ready[1] = 0;
        }
    }
}

void ScheduleQueue::remove_instruction(_proc_inst_t *inst)
{
    schedule_queue.erase(std::remove(schedule_queue.begin(), schedule_queue.end(), inst), schedule_queue.end());
    ready_queue.erase(std::remove(ready_queue.begin(), ready_queue.end(), inst), ready_queue.end());
    current_size--;
}

bool ScheduleQueue::are_src_regs_ready(const _proc_inst_t *inst) const
{
    // Check if both source registers are ready for this instruction
    bool src0_ready = inst->src_reg_ready[0] == 0;
    // || inst->inst_number < inst->src_reg_ready[0];
    bool src1_ready = inst->src_reg_ready[1] == 0;
    // || inst->inst_number < inst->src_reg_ready[1];
    return src0_ready && src1_ready;
}

void ScheduleQueue::schedule_ready_instructions(uint64_t cycle_count)
{

    // Create a temporary vector to store instructions to be moved
    std::vector<_proc_inst_t *> ready_insts;

    // First, identify all ready instructions
    for (auto inst : schedule_queue)
    {
        DBG_PRINT(stdout, "inst: %llu are src regs ready? %llu %llu \n", inst->inst_number, inst->src_reg_ready[0], inst->src_reg_ready[1]);
        if (are_src_regs_ready(inst))
        {
            DBG_PRINT(stdout, "ready \n");
            ready_insts.push_back(inst);
        }
    }

    // Move ready instructions to ready queue and remove from schedule queue
    for (auto inst : ready_insts)
    {
        DBG_PRINT(stdout, "inst: %llu added to ready queue \n", inst->inst_number);
        ready_queue.push_back(inst);
        schedule_queue.erase(std::remove(schedule_queue.begin(), schedule_queue.end(), inst), schedule_queue.end());
    }
    // Sort ready queue by tag order
    std::sort(ready_queue.begin(), ready_queue.end(), [](const _proc_inst_t *a, const _proc_inst_t *b)
              { return a->inst_number < b->inst_number; });

    DBG_PRINT(stdout, "Schedule Queue Contents:\n");
    for (const auto &inst : schedule_queue)
    {
        DBG_PRINT(stdout, "Instruction %llu: op_code=%d, src_regs=[%d,%d], src_ready=[%llu,%llu]\n",
                  inst->inst_number,
                  inst->op_code,
                  inst->src_reg[0],
                  inst->src_reg[1],
                  inst->src_reg_ready[0],
                  inst->src_reg_ready[1]);
    }
    DBG_PRINT(stdout, "Ready Queue Contents:\n");
    for (const auto &inst : ready_queue)
    {
        DBG_PRINT(stdout, "Ready Instruction %llu: op_code=%d, src_regs=[%d,%d], src_ready=[%llu,%llu]\n",
                  inst->inst_number,
                  inst->op_code,
                  inst->src_reg[0],
                  inst->src_reg[1],
                  inst->src_reg_ready[0],
                  inst->src_reg_ready[1]);
    }
}

std::vector<_proc_inst_t *> &ScheduleQueue::get_ready_queue()
{
    return ready_queue;
}
// reserve slots that is the lesser of the number requested and the number of slots available
void ScheduleQueue::reserve_slots(uint64_t num_requested_slots)
{
    reserved_slots = std::min(num_requested_slots, max_size - current_size);
    DBG_PRINT(stdout, "reserved slots: %llu \n", reserved_slots);
}