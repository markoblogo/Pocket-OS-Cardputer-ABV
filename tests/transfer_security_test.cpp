#include "transfer_security.h"

#include <cassert>
#include <cstring>
#include <cstdio>

int main()
{
    char password[16] = {};
    assert(generateTransferPassword(password, sizeof(password), 0x12ab34cd));
    assert(std::strcmp(password, "abvx12ab34cd") == 0);

    char too_small[12] = {};
    assert(!generateTransferPassword(too_small, sizeof(too_small), 1));
    assert(too_small[0] == '\0');

    assert(!generateTransferPassword(nullptr, 0, 1));
    std::puts("Transfer security: OK");
}
