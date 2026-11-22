
#include <stdio.h>
#include <unistd.h>

#include <sysutil/sysutil.h>

static bool running = true;

extern "C"
{
    static void THExitCallback(u64 status, u64 param, void *userdata)
    {
        (void)param;
        (void)userdata;

        if (status == SYSUTIL_EXIT_GAME)
            running = false;
    }
}

int main()
{
    printf("================================\n");
    printf("      TH: THE LAST WARD\n");
    printf("      STARTUP TEST\n");
    printf("================================\n");

    sysUtilRegisterCallback(
        SYSUTIL_EVENT_SLOT0,
        THExitCallback,
        NULL
    );

    printf("[TH] Startup test running.\n");

    while (running)
    {
        sysUtilCheckCallback();
        usleep(16000);
    }

    sysUtilUnregisterCallback(SYSUTIL_EVENT_SLOT0);

    printf("[TH] Startup test finished.\n");

    return 0;
}
