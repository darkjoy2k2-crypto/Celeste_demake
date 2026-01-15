#pragma once

#include <genesis.h>

void FADE_init();
void FADE_in(u16 num_frames);
void FADE_out(u16 num_frames);
void PAL_set_colors(u16 pal_num, u16 start_index, const u16* colors, u16 count);
void PAL_restore_direct(u16 pal_num, u16 start_index, u16 count);