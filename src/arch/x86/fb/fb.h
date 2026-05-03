#pragma once
#include "vbe/vbe_mode_info.h"
#include <arch/noarch/fb/fb.h>

/**
 * @brief Gets the current framebuffer information from the VBE mode info block.
 * @return Pointer to the current `VbeModeInfo` structure.
 * @note For internal use only. Do not use directly.
 * @internal
 */
extern "C" const VbeModeInfo *vbe_block();
