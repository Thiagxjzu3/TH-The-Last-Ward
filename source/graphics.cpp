
#include <stdio.h>
#include <rsx/rsx.h>
#include <sysutil/video.h>

namespace THGraphics
{
    bool initialized = false;

    bool Init()
    {
        printf("[GRAPHICS] Initializing video system...\n");

        videoState state = {};

        if (videoGetState(VIDEO_PRIMARY, 0, &state) != 0)
        {
            printf("[GRAPHICS] Video state error.\n");
            return false;
        }

        if (state.state != VIDEO_STATE_ENABLED)
        {
            printf("[GRAPHICS] Video output is not enabled.\n");
            return false;
        }

        printf("[GRAPHICS] Video system detected.\n");

        initialized = true;
        return true;
    }

    void Shutdown()
    {
        if (!initialized)
            return;

        printf("[GRAPHICS] Graphics module stopped.\n");
        initialized = false;
    }
}
