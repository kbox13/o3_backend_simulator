#include "register_file.hpp"
#include "debug.hpp"

RegisterFile::RegisterFile()
{
    register_status.resize(NUM_REGISTERS, 0);
}

void RegisterFile::mark_busy(uint64_t reg_num, uint64_t inst_number)
{
    DBG_PRINT(stdout, "marking reg %llu as busy with inst %llu \n", reg_num, inst_number);
    if (reg_num < NUM_REGISTERS && inst_number >= 0)
    {
        register_status[reg_num] = inst_number;
    }
}

void RegisterFile::mark_ready(uint64_t reg_num, uint64_t inst_number)
{
    if (reg_num < NUM_REGISTERS && inst_number >= 0 && register_status[reg_num] == inst_number)
    {
        register_status[reg_num] = 0;
    }
}

uint64_t RegisterFile::get_num_registers() const
{
    return NUM_REGISTERS;
}

uint64_t RegisterFile::get_recent_dependency(uint64_t reg_num) const
{
    if (reg_num < NUM_REGISTERS && register_status[reg_num] >= 0)
    {
        return register_status[reg_num];
    }
    return 0;
}