
#ifndef TH_RSXUTIL_H
#define TH_RSXUTIL_H

#include <ppu-types.h>
#include <rsx/rsx.h>

#define DEFAULT_CB_SIZE 0x80000
#define FRAME_BUFFER_COUNT 2

extern gcmContextData *context;
extern u32 curr_fb;
extern u32 display_width;
extern u32 display_height;

extern u32 color_pitch;
extern u32 color_offset[FRAME_BUFFER_COUNT];
extern u32 *color_buffer[FRAME_BUFFER_COUNT];

void init_screen(void *host_addr, u32 size);
void setRenderTarget(u32 index);
void flip();

#endif
