#include "transfer_security.h"

#include <cstdio>

bool generateTransferPassword(char* output, size_t output_size, uint32_t random_value)
{
    constexpr size_t required_size = 13; // "abvx" + 8 hex digits + NUL
    if (!output || output_size < required_size) {
        if (output && output_size) output[0] = '\0';
        return false;
    }
    return std::snprintf(output, output_size, "abvx%08lx",
                         static_cast<unsigned long>(random_value)) == 12;
}
