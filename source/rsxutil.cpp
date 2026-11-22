
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>

#include <ppu-types.h>
#include <rsx/gcm_sys.h>
#include <rsx/rsx.h>
#include <sysutil/video.h>

#include "rsxutil.h"

namespace THRSX
{
    static gcmContextData *context = NULL;
    static void *ioBuffer = NULL;

    static u32 *frameBuffers[TH_RSX_FRAMEBUFFER_COUNT] = {
        NULL, NULL
    };

    static u32 frameOffsets[TH_RSX_FRAMEBUFFER_COUNT] = {
        0, 0
    };

    static u32 *depthBuffer = NULL;
    static u32 depthOffset = 0;

    static u32 width = 0;
    static u32 height = 0;
    static u32 pitch = 0;

    static u32 drawBuffer = 0;
    static bool initialized = false;

    static bool ConfigureVideo()
    {
        videoState state = {};
        videoResolution resolution = {};

        if (videoGetState(VIDEO_PRIMARY, 0, &state) != 0)
        {
            printf("[RSX] Could not read video state.\n");
            return false;
        }

        if (state.state != 0)
        {
            printf("[RSX] Video output is disabled.\n");
            return false;
        }

        if (videoGetResolution(
            state.displayMode.resolution,
            &resolution
        ) != 0)
        {
            printf("[RSX] Could not read current resolution.\n");
            return false;
        }

        videoConfiguration config = {};
        config.resolution = state.displayMode.resolution;
        config.format = VIDEO_BUFFER_FORMAT_XRGB;
        config.aspect = state.displayMode.aspect;
        config.pitch = (u32)resolution.width * sizeof(u32);

        if (videoConfigure(
            VIDEO_PRIMARY,
            &config,
            NULL,
            0
        ) != 0)
        {
            printf("[RSX] Video configuration failed.\n");
            return false;
        }

        width = resolution.width;
        height = resolution.height;
        pitch = width * sizeof(u32);

        printf("[RSX] Display: %ux%u\n", width, height);
        return true;
    }

    static bool SetRenderTarget(u32 index)
    {
        if (!context || index >= TH_RSX_FRAMEBUFFER_COUNT)
            return false;

        gcmSurface surface = {};

        surface.type = GCM_SURFACE_TYPE_LINEAR;
        surface.antiAlias = GCM_SURFACE_CENTER_1;
        surface.colorFormat = GCM_SURFACE_X8R8G8B8;
        surface.colorTarget = GCM_SURFACE_TARGET_0;

        surface.colorLocation[0] = GCM_LOCATION_RSX;
        surface.colorOffset[0] = frameOffsets[index];
        surface.colorPitch[0] = pitch;

        surface.colorLocation[1] = GCM_LOCATION_RSX;
        surface.colorLocation[2] = GCM_LOCATION_RSX;
        surface.colorLocation[3] = GCM_LOCATION_RSX;

        surface.colorOffset[1] = 0;
        surface.colorOffset[2] = 0;
        surface.colorOffset[3] = 0;

        surface.colorPitch[1] = 64;
        surface.colorPitch[2] = 64;
        surface.colorPitch[3] = 64;

        surface.depthFormat = GCM_SURFACE_ZETA_Z16;
        surface.depthLocation = GCM_LOCATION_RSX;
        surface.depthOffset = depthOffset;
        surface.depthPitch = pitch;

        surface.width = width;
        surface.height = height;
        surface.x = 0;
        surface.y = 0;

        rsxSetSurface(context, &surface);
        return true;
    }

    bool Init()
    {
        if (initialized)
            return true;

        printf("[RSX] Initializing RSX context...\n");

        ioBuffer = memalign(
            1024 * 1024,
            TH_RSX_IO_BUFFER_SIZE
        );

        if (!ioBuffer)
        {
            printf("[RSX] IO buffer allocation failed.\n");
            return false;
        }

        memset(ioBuffer, 0, TH_RSX_IO_BUFFER_SIZE);

        s32 rsxResult = rsxInit(
            &context,
            TH_RSX_COMMAND_BUFFER_SIZE,
            TH_RSX_IO_BUFFER_SIZE,
            ioBuffer
        );

        if (rsxResult != 0 || context == NULL)
        {
            printf("[RSX] Context initialization failed (%d).\n", rsxResult);
            free(ioBuffer);
            ioBuffer = NULL;
            context = NULL;
            return false;
        }

        if (!ConfigureVideo())
        {
            printf("[RSX] Video setup failed.\n");
            return false;
        }

        gcmSetFlipMode(GCM_FLIP_VSYNC);

        for (u32 i = 0; i < TH_RSX_FRAMEBUFFER_COUNT; i++)
        {
            frameBuffers[i] = (u32*)rsxMemalign(
                64,
                height * pitch
            );

            if (!frameBuffers[i])
            {
                printf("[RSX] Framebuffer %u allocation failed.\n", i);
                return false;
            }

            memset(frameBuffers[i], 0, height * pitch);

            if (rsxAddressToOffset(
                frameBuffers[i],
                &frameOffsets[i]
            ) != 0)
            {
                printf("[RSX] Framebuffer %u offset conversion failed.\n", i);
                return false;
            }

            if (gcmSetDisplayBuffer(
                i,
                frameOffsets[i],
                pitch,
                width,
                height
            ) != 0)
            {
                printf("[RSX] Display buffer %u registration failed.\n", i);
                return false;
            }
        }

        depthBuffer = (u32*)rsxMemalign(
            64,
            height * pitch
        );

        if (!depthBuffer)
        {
            printf("[RSX] Depth buffer allocation failed.\n");
            return false;
        }

        memset(depthBuffer, 0, height * pitch);

        if (rsxAddressToOffset(
            depthBuffer,
            &depthOffset
        ) != 0)
        {
            printf("[RSX] Depth buffer offset conversion failed.\n");
            return false;
        }

        gcmResetFlipStatus();

        drawBuffer = 0;

        if (!SetRenderTarget(drawBuffer))
        {
            printf("[RSX] Could not set initial render target.\n");
            return false;
        }

        initialized = true;
        printf("[RSX] Initialization complete.\n");
        return true;
    }

    void Clear(u32 color)
    {
        if (!initialized || !frameBuffers[drawBuffer])
            return;

        u32 pixelCount = width * height;

        for (u32 i = 0; i < pixelCount; i++)
            frameBuffers[drawBuffer][i] = color;
    }

    bool Present()
    {
        if (!initialized || !context)
            return false;

        while (gcmGetFlipStatus() != 0)
            usleep(200);

        gcmResetFlipStatus();

        if (gcmSetFlip(context, drawBuffer) != 0)
        {
            printf("[RSX] Flip command failed.\n");
            return false;
        }

        rsxFlushBuffer(context);
        gcmSetWaitFlip(context);

        drawBuffer ^= 1;

        if (!SetRenderTarget(drawBuffer))
        {
            printf("[RSX] Could not switch render target.\n");
            return false;
        }

        return true;
    }

    u32 GetWidth()
    {
        return width;
    }

    u32 GetHeight()
    {
        return height;
    }

    void Shutdown()
    {
        if (!initialized)
            return;

        printf("[RSX] Shutting down.\n");

        initialized = false;
        context = NULL;
        ioBuffer = NULL;
        depthBuffer = NULL;

        frameBuffers[0] = NULL;
        frameBuffers[1] = NULL;

        frameOffsets[0] = 0;
        frameOffsets[1] = 0;
        depthOffset = 0;

        width = 0;
        height = 0;
        pitch = 0;
        drawBuffer = 0;
    }
}
