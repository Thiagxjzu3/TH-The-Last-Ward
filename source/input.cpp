
#include <stdio.h>
#include "th_config.h"

namespace THInput
{
    bool initialized = false;

    bool Init()
    {
        printf("[INPUT] Initializing controller system...\n");

        initialized = true;

        printf("[INPUT] Controller system initialized.\n");

        return true;
    }

    void Update()
    {
        if (!initialized)
            return;

        // Controller polling will be implemented
        // using the PS3 pad library.
    }

    void Shutdown()
    {
        initialized = false;

        printf("[INPUT] Controller system stopped.\n");
    }
}
