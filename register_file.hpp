#ifndef REGISTER_FILE_HPP
#define REGISTER_FILE_HPP

#include <vector>
#include <cstdint>

class RegisterFile
{
public:
    // Constructor initializes all registers as ready
    RegisterFile();

    // Mark a register as busy with the instruction number that's using it
    void mark_busy(uint64_t reg_num, uint64_t inst_number);

    // Mark a register as ready by removing the instruction number
    void mark_ready(uint64_t reg_num, uint64_t inst_number);

    // Get the total number of registers
    uint64_t get_num_registers() const;

    // Get the most recent instruction number that is producing the value
    uint64_t get_recent_dependency(uint64_t reg_num) const;

private:
    // Vector to track register status - each element is a vector of instruction numbers using that register
    std::vector<uint64_t> register_status;

    // Total number of registers 64 to be safe
    static const uint64_t NUM_REGISTERS = 64;
};

#endif // REGISTER_FILE_HPP