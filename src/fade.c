#include "fade.h"
#include "title.h"
#include "level.h"

static u16 target_palettes[64];

void FADE_init() {
    memcpy(&target_palettes[0], pal_bg.data, 16 * 2);
    memcpy(&target_palettes[16], player_sprite.palette->data, 16 * 2);
    memcpy(&target_palettes[32], pal_font.data, 16 * 2);
    memcpy(&target_palettes[48], pal_layer_1.data, 16 * 2);

    PAL_setColors(0, palette_black, 64, DMA);
}

void FADE_in(u16 num_frames) {
    PAL_fadeTo(0, 63, target_palettes, num_frames, TRUE);
}

void FADE_out(u16 num_frames) {
    PAL_fadeTo(0, 63, palette_black, num_frames, TRUE);
}

void PAL_set_colors(u16 pal_num, u16 start_index, const u16* colors, u16 count) {
    u16 base = (pal_num << 4) + start_index;
    for (u16 i = 0; i < count; i++) {
        PAL_setColor(base + i, colors[i]);
    }
}

void PAL_restore_direct(u16 pal_num, u16 start_index, u16 count) {
    u16 base = (pal_num << 4) + start_index;
    for (u16 i = 0; i < count; i++) {
        PAL_setColor(base + i, target_palettes[base + i]);
    }
}