// debug.hpp
#pragma once
#include <cstdio>

extern bool g_debug;
extern bool g_log;
extern bool g_output_print;

#define DBG_PRINT(stream, fmt, ...)              \
    do                                           \
    {                                            \
        if (g_debug)                             \
            fprintf(stream, fmt, ##__VA_ARGS__); \
    } while (0)

#define LOG_PRINT(stream, fmt, ...)              \
    do                                           \
    {                                            \
        if (g_log)                               \
            fprintf(stream, fmt, ##__VA_ARGS__); \
    } while (0)

#define OUTPUT_PRINT(stream, fmt, ...)           \
    do                                           \
    {                                            \
        if (g_output_print)                      \
            fprintf(stream, fmt, ##__VA_ARGS__); \
    } while (0)