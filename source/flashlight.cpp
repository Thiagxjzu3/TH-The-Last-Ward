
#include <stdio.h>
#include "th_config.h"

namespace THFlashlight
{
    struct Flashlight
    {
        bool enabled;
        float battery;
        float intensity;
        bool initialized;
    };

    Flashlight flashlight;

    void Init()
    {
        flashlight.enabled = true;
        flashlight.battery = 100.0f;
        flashlight.intensity = 1.0f;
        flashlight.initialized = true;

        printf("[FLASHLIGHT] Flashlight initialized.\n");
    }

    void Toggle()
    {
        if (!flashlight.initialized)
            return;

        flashlight.enabled = !flashlight.enabled;

        printf("[FLASHLIGHT] %s\n",
            flashlight.enabled ? "ON" : "OFF");
    }

    void Update(float deltaTime)
    {
        if (!flashlight.initialized)
            return;

        if (flashlight.enabled && flashlight.battery > 0.0f)
        {
            flashlight.battery -= 0.5f * deltaTime;

            if (flashlight.battery <= 0.0f)
            {
                flashlight.battery = 0.0f;
                flashlight.enabled = false;

                printf("[FLASHLIGHT] Battery depleted.\n");
            }
        }
    }

    void Shutdown()
    {
        flashlight.initialized = false;

        printf("[FLASHLIGHT] Shutdown.\n");
    }
}
