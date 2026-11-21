
#include <stdio.h>
#include "th_config.h"

namespace THPlayer
{
    struct Player
    {
        float x;
        float y;
        float z;

        float rotationX;
        float rotationY;

        float speed;
        bool initialized;
    };

    Player player;

    void Init()
    {
        player.x = 0.0f;
        player.y = 0.0f;
        player.z = 0.0f;

        player.rotationX = 0.0f;
        player.rotationY = 0.0f;

        player.speed = TH_PLAYER_SPEED;
        player.initialized = true;

        printf("[PLAYER] Player initialized.\n");
    }

    void Update()
    {
        if (!player.initialized)
            return;

        // Movement and camera calculations
        // will be connected to the controller.
    }

    void Shutdown()
    {
        player.initialized = false;

        printf("[PLAYER] Player shutdown.\n");
    }
}
