
#ifndef TH_GRAPHICS_H
#define TH_GRAPHICS_H

namespace THGraphics
{
    bool Init();

    void Clear(
        float red,
        float green,
        float blue
    );

    void Present();

    void Shutdown();
}

#endif