#include "procsim.hpp"
#include "processor.hpp"
#include "debug.hpp"

// Global processor instance
static Processor *processor = nullptr;

/**
 * Subroutine for initializing the processor. You many add and initialize any global or heap
 * variables as needed.
 * XXX: You're responsible for completing this routine
 *
 * @r ROB size
 * @k0 Number of k0 FUs
 * @k1 Number of k1 FUs
 * @k2 Number of k2 FUs
 * @f Number of instructions to fetch
 */
void setup_proc(uint64_t r, uint64_t k0, uint64_t k1, uint64_t k2, uint64_t f)
{

    // init the processor
    // create the null ptr for instructions to fetch
    processor = new Processor(r, k0, k1, k2, f);
}

/**
 * Subroutine that simulates the processor.
 *   The processor should fetch instructions as appropriate, until all instructions have executed
 * XXX: You're responsible for completing this routine
 *
 * @p_stats Pointer to the statistics structure
 */
void run_proc(proc_stats_t *p_stats)
{
    // for number of instructions of instructions to fetch f
    // read instrcutions and put into pointers that are inside of processor
    // then when excuting the fetch instruction in the processor
    // move from the pointers to the dispatch queue
    // need to store the valid instuction bool also so that
    // when we get a false we stop running and clean up
    bool proc_running = true;
    // prefetch the first instructions instructions
    // prefetch_insts(proc_running);
    while (proc_running)
    {
        // tick the processor one cycle
        if (proc_running)
        {
            DBG_PRINT(stdout, "cycle: %llu proc_running: %d\n", processor->get_cycle_counter(), proc_running);
            processor->tick();
            p_stats->cycle_count++;
            prefetch_insts(proc_running);
            processor->incr_cycle_counter();
        }
    }
}

/**
 * Subroutine for cleaning up any outstanding instructions and calculating overall statistics
 * such as average IPC, average fire rate etc.
 * XXX: You're responsible for completing this routine
 *
 * @p_stats Pointer to the statistics structure
 */
void complete_proc(proc_stats_t *p_stats)
{
    // Run until all units are empty
    while (processor->has_pending_instructions())
    {
        DBG_PRINT(stdout, "cycle: %llu proc_running: %d\n", processor->get_cycle_counter(), processor->has_pending_instructions());
        processor->tick();
        processor->incr_cycle_counter();
        p_stats->cycle_count++;
    }
    // need to decrement the cycle count by one to account for the last cycle
    p_stats->cycle_count--;
    process_stats(p_stats);
    processor->clean_up();
}

void process_stats(proc_stats_t *p_stats)
{
    // cast all to floats before division
    double retired_insts = static_cast<double>(processor->get_retired_insts());
    double total_dispatch = static_cast<double>(processor->get_total_dispatch_size());
    double cycles = static_cast<double>(p_stats->cycle_count);
    p_stats->retired_instruction = processor->get_retired_insts();
    p_stats->avg_disp_size = total_dispatch / cycles;
    p_stats->max_disp_size = processor->get_max_dispatch_size();
    // insts fired equals retired is equal to total insts
    p_stats->avg_inst_fired = retired_insts / cycles;
    p_stats->avg_inst_retired = retired_insts / cycles;
}
// this should be a processor function ideally
void prefetch_insts(bool &proc_running)
{
    // loop through and fetch the number of instructions according to the rate
    for (int i = 0; i < processor->get_fetch_rate(); i++)
    {
        // get inst ptr according to the index
        _proc_inst_t *cur_inst_ptr = processor->get_inst_ptr(i);
        // read the instruction and set valid according to read inst
        // should be true for entire sim then once false all false
        processor->set_valid_inst(read_instruction(cur_inst_ptr));
        if (!processor->running())
        {
            proc_running = false;
            break;
        }
        // increment the inst counter
        processor->incr_inst_cntr();
        cur_inst_ptr->inst_number = processor->get_inst_cntr();
        cur_inst_ptr->fetch_cycle = processor->get_cycle_counter();
        LOG_PRINT(stdout, "%llu FETCHED %llu \n", processor->get_cycle_counter(), cur_inst_ptr->inst_number);
    }
}