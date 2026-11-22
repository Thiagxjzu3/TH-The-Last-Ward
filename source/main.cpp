
#include <stdio.h>
#include <unistd.h>

#include <io/pad.h>
#include <sysutil/sysutil.h>

#include "th_config.h"
#include "th_graphics.h"

static bool running = true;

extern "C"
{
    static void THExitCallback(u64 status, u64 param, void *userdata)
    {
        (void)param;
        (void)userdata;

        if (status == SYSUTIL_EXIT_GAME)
            running = false;
    }
}

int main()
{
    printf("================================\n");
    printf("      TH: THE LAST WARD\n");
    printf("      GRAPHICS TEST\n");
    printf("================================\n");

    if (!THGraphics::Init())
    {
        printf("[TH] Graphics initialization failed.\n");
        return 1;
    }

    bool padReady = (ioPadInit(7) == 0);

    sysUtilRegisterCallback(
        SYSUTIL_EVENT_SLOT0,
        THExitCallback,
        NULL
    );

    padInfo info = {};
    padData data = {};

    unsigned int frame = 0;
    unsigned int colorIndex = 0;

    const float colors[4][3] = {
        {0.08f, 0.02f, 0.02f},
        {0.02f, 0.12f, 0.04f},
        {0.02f, 0.05f, 0.16f},
        {0.015f, 0.020f, 0.030f}
    };

    printf("[TH] Graphics test started.\n");

    while (running)
    {
        sysUtilCheckCallback();

        if (padReady && ioPadGetInfo(&info) == 0)
        {
            for (u32 i = 0; i < MAX_PADS; i++)
            {
                if (!info.status[i])
                    continue;

                data = {};

                if (ioPadGetData(i, &data) == 0)
                {
                    if (data.BTN_START)
                        running = false;
                }
            }
        }

        THGraphics::Clear(
            colors[colorIndex][0],
            colors[colorIndex][1],
            colors[colorIndex][2]
        );

        THGraphics::Present();

        frame++;

        if (frame >= 180)
        {
            frame = 0;
            colorIndex = (colorIndex + 1) % 4;
        }

        usleep(16000);
    }

    sysUtilUnregisterCallback(SYSUTIL_EVENT_SLOT0);

    if (padReady)
        ioPadEnd();

    THGraphics::Shutdown();

    printf("[TH] Graphics test finished.\n");

    return 0;
}
