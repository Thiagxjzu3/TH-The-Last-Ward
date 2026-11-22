
#include <stdio.h>
#include <unistd.h>

#include <sysutil/sysutil.h>

static volatile bool running = true;
static FILE* logFile = NULL;

extern "C"
{
    static void THExitCallback(u64 status, u64 param, void *userdata)
    {
        (void)param;
        (void)userdata;

        if (status == SYSUTIL_EXIT_GAME)
        {
            if (logFile)
            {
                fprintf(logFile, "EXIT_EVENT_RECEIVED\n");
                fflush(logFile);
            }

            running = false;
        }
    }
}

int main()
{
    logFile = fopen("/dev_hdd0/tmp/THLW_startup.log", "w");

    if (logFile)
    {
        fprintf(logFile, "MAIN_ENTERED\n");
        fflush(logFile);
    }

    s32 result = sysUtilRegisterCallback(
        SYSUTIL_EVENT_SLOT0,
        THExitCallback,
        NULL
    );

    if (logFile)
    {
        fprintf(logFile, "CALLBACK_RESULT=%d\n", (int)result);
        fflush(logFile);
    }

    if (result != SYSUTIL_OK)
    {
        if (logFile)
        {
            fprintf(logFile, "CALLBACK_REGISTRATION_FAILED\n");
            fclose(logFile);
        }

        return 1;
    }

    if (logFile)
    {
        fprintf(logFile, "STARTUP_SUCCESS\n");
        fflush(logFile);
    }

    unsigned int counter = 0;

    while (running)
    {
        sysUtilCheckCallback();

        if (++counter >= 60)
        {
            counter = 0;

            if (logFile)
            {
                fprintf(logFile, "GAME_LOOP_ALIVE\n");
                fflush(logFile);
            }
        }

        usleep(16000);
    }

    sysUtilUnregisterCallback(SYSUTIL_EVENT_SLOT0);

    if (logFile)
    {
        fprintf(logFile, "GAME_EXITING\n");
        fclose(logFile);
        logFile = NULL;
    }

    return 0;
}
