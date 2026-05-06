#pragma once

#ifdef __GNUC__
#define PACKED __attribute__((packed))
#else
#define PACKED
#endif