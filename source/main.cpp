
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
    printf("================================\n");

    printf("[TH] Starting game engine...\n");

    if (!THGraphics::Init())
    {
        printf("[TH] Graphics initialization failed.\n");
        return 1;
    }

    printf("[TH] Graphics module ready.\n");

    bool padReady = (ioPadInit(7) == 0);

    if (!padReady)
        printf("[TH] Controller initialization failed.\n");

    sysUtilRegisterCallback(
        SYSUTIL_EVENT_SLOT0,
        THExitCallback,
        NULL
    );

    printf("[TH] Main loop started.\n");

    padInfo info = {};
    padData data = {};

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

        THGraphics::Clear(0.015f, 0.020f, 0.030f);
        THGraphics::Present();

        usleep(16000);
    }

    printf("[TH] Closing game...\n");

    sysUtilUnregisterCallback(SYSUTIL_EVENT_SLOT0);

    if (padReady)
        ioPadEnd();

    THGraphics::Shutdown();

    printf("[TH] Shutdown complete.\n");

    return 0;
}
