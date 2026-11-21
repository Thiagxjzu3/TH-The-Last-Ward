
#include <stdio.h>
#include <rsx/rsx.h>
#include <video/video.h>

namespace THGraphics
{
    bool initialized = false;

    bool Init()
    {
        printf("[GRAPHICS] Initializing RSX...\n");

        videoState state;

        if (videoGetState(0, 0, &state) != 0)
        {
            printf("[GRAPHICS] Video state error.\n");
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

        printf("[GRAPHICS] Shutdown.\n");
        initialized = false;
    }
}
