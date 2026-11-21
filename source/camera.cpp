
#iinclude <stdio.h>
#include "th_config.h"

namespace THCamera
{
    struct Camera
    {
        float x;
        float y;
        float z;

        float pitch;
        float yaw;

        float fieldOfView;
        bool initialized;
    };

    Camera camera;

    void Init()
    {
        camera.x = 0.0f;
        camera.y = 1.7f;
        camera.z = 0.0f;

        camera.pitch = 0.0f;
        camera.yaw = 0.0f;

        camera.fieldOfView = 75.0f;
        camera.initialized = true;

        printf("[CAMERA] First person camera initialized.\n");
    }

    void Update()
    {
        if (!camera.initialized)
            return;

        // Camera rotation and player synchronization
        // will be implemented in the rendering engine.
    }

    void Shutdown()
    {
        camera.initialized = false;

        printf("[CAMERA] Camera shutdown.\n");
    }
}
