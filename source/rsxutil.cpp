
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>

#include <ppu-types.h>
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

    static u32 width = 0;
    static u32 height = 0;
    static u32 pitch = 0;

    static u32 drawBuffer = 0;
    static bool initialized = false;

    static bool ConfigureVideo()
    {
        const u32 resolutions[] = {
            VIDEO_RESOLUTION_720,
            VIDEO_RESOLUTION_480,
            VIDEO_RESOLUTION_576
        };

        videoResolution resolution;
        s32 result = -1;
        u32 selectedResolution = 0;

        for (u32 i = 0;
             i < sizeof(resolutions) / sizeof(resolutions[0]);
             i++)
        {
            result = videoGetResolutionAvailability(
                VIDEO_PRIMARY,
                resolutions[i],
                VIDEO_ASPECT_AUTO,
                0
            );

            if (result != 1)
                continue;

            result = videoGetResolution(
                resolutions[i],
                &resolution
            );

            if (result == 0)
            {
                selectedResolution = resolutions[i];
                break;
            }
        }

        if (selectedResolution == 0)
        {
            printf("[RSX] No supported resolution.\n");
            return false;
        }

        videoConfiguration config = {};

        config.resolution = (u8)selectedResolution;
        config.format = VIDEO_BUFFER_FORMAT_XRGB;
        config.aspect = VIDEO_ASPECT_AUTO;
        config.pitch = resolution.width * 4;

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

        width = resolution.width;
        height = resolution.height;
        pitch = width * 4;

        printf("[RSX] Resolution: %ux%u\n", width, height);

        return true;
    }

    bool Init()
    {
        if (initialized)
            return true;

        printf("[RSX] Initializing...\n");

        ioBuffer = memalign(
            1024 * 1024,
            TH_RSX_IO_BUFFER_SIZE
        );

        if (!ioBuffer)
        {
            printf("[RSX] IO memory allocation failed.\n");
            return false;
        }

        memset(ioBuffer, 0, TH_RSX_IO_BUFFER_SIZE);

        if (rsxInit(
            &context,
            TH_RSX_COMMAND_BUFFER_SIZE,
            TH_RSX_IO_BUFFER_SIZE,
            ioBuffer
        ) != 0)
        {
            printf("[RSX] Context initialization failed.\n");
            free(ioBuffer);
            ioBuffer = NULL;
            context = NULL;
            return false;
        }

        if (!ConfigureVideo())
        {
            printf("[RSX] Video initialization failed.\n");
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
                printf("[RSX] Framebuffer allocation failed.\n");
                return false;
            }

            memset(frameBuffers[i], 0, height * pitch);

            if (rsxAddressToOffset(
                frameBuffers[i],
                &frameOffsets[i]
            ) != 0)
            {
                printf("[RSX] Framebuffer offset failed.\n");
                return false;
            }

            gcmSetDisplayBuffer(
                i,
                frameOffsets[i],
                pitch,
                width,
                height
            );
        }

        gcmResetFlipStatus();

        drawBuffer = 0;
        initialized = true;

        printf("[RSX] Initialization complete.\n");

        return true;
    }

    void Clear(u32 color)
    {
        if (!initialized)
            return;

        u32 *pixels = frameBuffers[drawBuffer];

        if (!pixels)
            return;

        u32 count = width * height;

        for (u32 i = 0; i < count; i++)
        {
            pixels[i] = color;
        }
    }

    bool Present()
    {
        if (!initialized || !context)
            return false;

        while (gcmGetFlipStatus() != 0)
        {
            usleep(200);
        }

        gcmSetFlip(context, drawBuffer);

        rsxFlushBuffer(context);

        gcmSetWaitFlip(context);

        drawBuffer ^= 1;

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

        /*
         * RSX allocations remain managed by the RSX
         * memory manager until process termination.
         */

        context = NULL;

        if (ioBuffer)
        {
            free(ioBuffer);
            ioBuffer = NULL;
        }

        width = 0;
        height = 0;
        pitch = 0;
        drawBuffer = 0;
    }
}
