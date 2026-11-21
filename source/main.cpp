
#include <stdio.h>
#include "th_config.h"

namespace THGraphics
{
    bool Init();
    void Shutdown();
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

    THGraphics::Shutdown();

    printf("[TH] Shutdown complete.\n");

    return 0;
}
