
#include <stdio.h>

#include "th_graphics.h"
#include "rsxutil.h"

namespace THGraphics
{
    static bool initialized = false;

    static unsigned int ConvertColor(float value)
    {
        if (value < 0.0f)
            value = 0.0f;

        if (value > 1.0f)
            value = 1.0f;

        return (unsigned int)(value * 255.0f + 0.5f);
    }

    bool Init()
    {
        if (initialized)
            return true;

        printf("[GRAPHICS] Initializing RSX...\n");

        if (!THRSX::Init())
        {
            printf("[GRAPHICS] RSX initialization failed.\n");
            return false;
        }

        initialized = true;

        printf("[GRAPHICS] RSX initialized.\n");

        return true;
    }

    void Clear(float red, float green, float blue)
    {
        if (!initialized)
            return;

        unsigned int r = ConvertColor(red);
        unsigned int g = ConvertColor(green);
        unsigned int b = ConvertColor(blue);

        unsigned int color =
            (r << 16) |
            (g << 8) |
            b;

        THRSX::Clear(color);
    }

    void Present()
    {
        if (!initialized)
            return;

        if (!THRSX::Present())
        {
            printf("[GRAPHICS] Frame presentation failed.\n");
        }
    }

    void Shutdown()
    {
        if (!initialized)
            return;

        THRSX::Shutdown();

        initialized = false;

        printf("[GRAPHICS] Graphics stopped.\n");
    }
}
