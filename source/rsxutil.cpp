
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ppu-types.h>
#include <sysutil/video.h>
#include <rsx/rsx.h>

#include "rsxutil.h"

#define GCM_LABEL_INDEX 255

videoResolution vResolution;

gcmContextData *context = NULL;

u32 curr_fb = 0;
static u32 first_fb = 1;

u32 display_width = 0;
u32 display_height = 0;

u32 depth_pitch = 0;
u32 depth_offset = 0;
u32 *depth_buffer = NULL;

u32 color_pitch = 0;
u32 color_offset[FRAME_BUFFER_COUNT];
u32 *color_buffer[FRAME_BUFFER_COUNT];

f32 aspect_ratio = 16.0f / 9.0f;

static u32 resolution_ids[] = {
    VIDEO_RESOLUTION_720,
    VIDEO_RESOLUTION_480,
    VIDEO_RESOLUTION_576
};

static u32 label_value = 1;

static void waitFinish()
{
    rsxSetWriteBackendLabel(
        context,
        GCM_LABEL_INDEX,
        label_value
    );

    rsxFlushBuffer(context);

    while (*(volatile u32*)gcmGetLabelAddress(GCM_LABEL_INDEX)
           != label_value)
    {
        usleep(30);
    }

    label_value++;
}

static void waitRSXIdle()
{
    rsxSetWriteBackendLabel(
        context,
        GCM_LABEL_INDEX,
        label_value
    );

    rsxSetWaitLabel(
        context,
        GCM_LABEL_INDEX,
        label_value
    );

    label_value++;

    waitFinish();
}

static bool initVideoConfiguration()
{
    s32 result = -1;
    u32 resolution_id = 0;

    for (u32 i = 0;
         i < sizeof(resolution_ids) / sizeof(resolution_ids[0]);
         i++)
    {
        result = videoGetResolutionAvailability(
            VIDEO_PRIMARY,
            resolution_ids[i],
            VIDEO_ASPECT_AUTO,
            0
        );

        if (result != 1)
            continue;

        resolution_id = resolution_ids[i];

        result = videoGetResolution(
            resolution_id,
            &vResolution
        );

        if (result == 0)
            break;
    }

    if (result != 0)
    {
        printf("[RSX] No usable resolution found.\n");
        return false;
    }

    videoConfiguration config = {
        (u8)resolution_id,
        VIDEO_BUFFER_FORMAT_XRGB,
        VIDEO_ASPECT_AUTO,
        {0,0,0,0,0,0,0,0,0},
        (u32)vResolution.width * 4
    };

    result = videoConfigure(
        VIDEO_PRIMARY,
        &config,
        NULL,
        0
    );

    if (result != 0)
    {
        printf("[RSX] Video configuration failed.\n");
        return false;
    }

    videoState state;

    result = videoGetState(
        VIDEO_PRIMARY,
        0,
        &state
    );

    if (result != 0)
        return false;

    if (state.displayMode.aspect == VIDEO_ASPECT_4_3)
        aspect_ratio = 4.0f / 3.0f;
    else
        aspect_ratio = 16.0f / 9.0f;

    display_width = vResolution.width;
    display_height = vResolution.height;

    return true;
}

void setRenderTarget(u32 index)
{
    gcmSurface surface = {};

    surface.colorFormat = GCM_SURFACE_X8R8G8B8;
    surface.colorTarget = GCM_SURFACE_TARGET_0;

    surface.colorLocation[0] = GCM_LOCATION_RSX;
    surface.colorOffset[0] = color_offset[index];
    surface.colorPitch[0] = color_pitch;

    surface.colorLocation[1] = GCM_LOCATION_RSX;
    surface.colorLocation[2] = GCM_LOCATION_RSX;
    surface.colorLocation[3] = GCM_LOCATION_RSX;

    surface.colorOffset[1] = 0;
    surface.colorOffset[2] = 0;
    surface.colorOffset[3] = 0;

    surface.colorPitch[1] = 64;
    surface.colorPitch[2] = 64;
    surface.colorPitch[3] = 64;

    surface.depthFormat = GCM_SURFACE_ZETA_Z24S8;
    surface.depthLocation = GCM_LOCATION_RSX;
    surface.depthOffset = depth_offset;
    surface.depthPitch = depth_pitch;

    surface.type = GCM_SURFACE_TYPE_LINEAR;
    surface.antiAlias = GCM_SURFACE_CENTER_1;

    surface.width = display_width;
    surface.height = display_height;

    surface.x = 0;
    surface.y = 0;

    rsxSetSurface(context, &surface);
}

void init_screen(void *host_addr, u32 size)
{
    const u32 color_depth = 4;
    const u32 depth_size = 4;

    if (rsxInit(
        &context,
        DEFAULT_CB_SIZE,
        size,
        host_addr
    ) != 0)
    {
        printf("[RSX] Context initialization failed.\n");
        exit(1);
    }

    if (!initVideoConfiguration())
        exit(1);

    waitRSXIdle();

    gcmSetFlipMode(GCM_FLIP_VSYNC);

    color_pitch = display_width * color_depth;
    depth_pitch = display_width * depth_size;

    for (u32 i = 0; i < FRAME_BUFFER_COUNT; i++)
    {
        color_buffer[i] = (u32*)rsxMemalign(
            64,
            display_height * color_pitch
        );

        if (!color_buffer[i])
            exit(1);

        rsxAddressToOffset(
            color_buffer[i],
            &color_offset[i]
        );

        gcmSetDisplayBuffer(
            i,
            color_offset[i],
            color_pitch,
            display_width,
            display_height
        );
    }

    depth_buffer = (u32*)rsxMemalign(
        64,
        display_height * depth_pitch
    );

    if (!depth_buffer)
        exit(1);

    rsxAddressToOffset(
        depth_buffer,
        &depth_offset
    );

    printf("[RSX] Screen initialized: %ux%u\n",
           display_width,
           display_height);
}

static void waitflip()
{
    while (gcmGetFlipStatus() != 0)
        usleep(200);

    gcmResetFlipStatus();
}

void flip()
{
    if (!first_fb)
        waitflip();
    else
        gcmResetFlipStatus();

    gcmSetFlip(context, curr_fb);

    rsxFlushBuffer(context);
    gcmSetWaitFlip(context);

    curr_fb ^= 1;

    setRenderTarget(curr_fb);

    first_fb = 0;
}
