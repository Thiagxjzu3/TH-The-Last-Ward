
#ifndef TH_RSXUTIL_H
#define TH_RSXUTIL_H

#include <ppu-types.h>

#define TH_RSX_COMMAND_BUFFER_SIZE 0x100000
#define TH_RSX_IO_BUFFER_SIZE      0x100000
#define TH_RSX_FRAMEBUFFER_COUNT   2

namespace THRSX
{
    bool Init();
    void Clear(u32 color);
    bool Present();
    void Shutdown();

    u32 GetWidth();
    u32 GetHeight();
}

#endif
