#include <cstdio>
#include <cinttypes>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include "procsim.hpp"
#include "debug.hpp"

FILE *inFile = stdin;
bool g_debug = false;
bool g_log = false;
bool g_output_print = false;

void print_help_and_exit(void)
{
    printf("procsim [OPTIONS]\n");
    printf("  -j k0\t\tNumber of k0 FUs\n");
    printf("  -k k1\t\tNumber of k1 FUs\n");
    printf("  -l k2\t\tNumber of k2 FUs\n");
    printf("  -f N\t\tNumber of instructions to fetch\n");
    printf("  -r R\t\tNumber of result buses\n");
    printf("  -i traces/file.trace\n");
    printf("  -h\t\tThis helpful output\n");
    printf("  -d\t\tEnable debug output\n");
    printf("  -v\t\tEnable logging output\n");
    printf("  -o\t\tEnable output print mode\n");
    exit(0);
}

//
// read_instruction
//
//  returns true if an instruction was read successfully
//
bool read_instruction(proc_inst_t *p_inst)
{
    int ret;

    if (p_inst == NULL)
    {
        fprintf(stderr, "Fetch requires a valid pointer to populate\n");
        return false;
    }

    ret = fscanf(stdin, "%x %d %d %d %d\n", &p_inst->instruction_address,
                 &p_inst->op_code, &p_inst->dest_reg, &p_inst->src_reg[0], &p_inst->src_reg[1]);
    if (ret != 5)
    {
        return false;
    }

    return true;
}

void print_statistics(proc_stats_t *p_stats);

int main(int argc, char *argv[])
{
    int opt;
    uint64_t f = DEFAULT_F;
    uint64_t k0 = DEFAULT_K0;
    uint64_t k1 = DEFAULT_K1;
    uint64_t k2 = DEFAULT_K2;
    uint64_t r = DEFAULT_R;

    /* Read arguments */
    while (-1 != (opt = getopt(argc, argv, "r:i:j:k:l:f:h:dvo")))
    {
        switch (opt)
        {
        case 'r':
            r = atoi(optarg);
            break;
        case 'j':
            k0 = atoi(optarg);
            break;
        case 'k':
            k1 = atoi(optarg);
            break;
        case 'l':
            k2 = atoi(optarg);
            break;
        case 'f':
            f = atoi(optarg);
            break;
        case 'i':
            inFile = fopen(optarg, "r");
            if (inFile == NULL)
            {
                fprintf(stderr, "Failed to open %s for reading\n", optarg);
                print_help_and_exit();
            }
            break;
        case 'h':
            /* Fall through */
        case 'd':
            g_debug = true;
            break;
        case 'v':
            g_log = true;
            break;
        case 'o':
            g_output_print = true;
            break;
        default:
            print_help_and_exit();
            break;
        }
    }

    printf("Processor Settings\n");
    printf("R: %" PRIu64 "\n", r);
    printf("k0: %" PRIu64 "\n", k0);
    printf("k1: %" PRIu64 "\n", k1);
    printf("k2: %" PRIu64 "\n", k2);
    printf("F: %" PRIu64 "\n", f);
    printf("\n");

    /* Setup the processor */
    setup_proc(r, k0, k1, k2, f);

    /* Setup statistics */
    proc_stats_t stats;
    memset(&stats, 0, sizeof(proc_stats_t));

    /* Run the processor */
    run_proc(&stats);

    /* Finalize stats */
    complete_proc(&stats);

    print_statistics(&stats);

    return 0;
}

void print_statistics(proc_stats_t *p_stats)
{
    printf("\n");
    printf("Processor stats:\n");
    printf("Total instructions: %lu\n", p_stats->retired_instruction);
    printf("Avg Dispatch queue size: %f\n", p_stats->avg_disp_size);
    printf("Maximum Dispatch queue size: %lu\n", p_stats->max_disp_size);
    printf("Avg inst fired per cycle: %f\n", p_stats->avg_inst_fired);
    printf("Avg inst retired per cycle: %f\n", p_stats->avg_inst_retired);
    printf("Total run time (cycles): %lu\n", p_stats->cycle_count);
}
