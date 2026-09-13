#pragma once

#include <cstddef>
#include <cstdint>

// Writes a WPA2-compatible, per-session password without dynamic allocation.
bool generateTransferPassword(char* output, size_t output_size, uint32_t random_value);
