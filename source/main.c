#include <string.h>
#include <stdio.h>

#include <switch.h>

#include "elf2nso.h"

int main(int argc, char **argv)
{
    gfxInitDefault();
    consoleInit(NULL);

    printf("Lolz!\n");

    // Main loop
    while(appletMainLoop())
    {
        //Scan all the inputs. This should be done once for each frame
        hidScanInput();

        // Your code goes here

        //hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_A) {
            int n_argc = 3;
            char* n_argv[] = { "elf2nso", "/main.elf", "/atmosphere/titles/01003BC0000A0000/exefs/main" };
            printf("Yolo\n");
            int ret = elf2nso(n_argc, n_argv);
            printf("Done: %d\n", ret);
        }

        if (kDown & KEY_PLUS) break; // break in order to return to hbmenu

        gfxFlushBuffers();
        gfxSwapBuffers();
        gfxWaitForVsync();
    }

    gfxExit();
    return 0;
}
