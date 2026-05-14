#pragma once
#include <stdint.h>


uint32_t sys_read(uint32_t arg1);

struct ReadArguments {
    uint32_t count;
    uint32_t fd;
    void* buf;
};