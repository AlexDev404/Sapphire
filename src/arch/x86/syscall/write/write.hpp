#pragma once
#include <stdint.h>


uint32_t sys_write(uint32_t arg1);

struct WriteArguments {
    uint32_t count;
    uint32_t fd;
    void* buf;
};