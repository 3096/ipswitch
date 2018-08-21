#include <switch.h>

#include "menu.h"

int main(int argc, char **argv)
{
    gfxInitDefault();
    consoleInit(NULL);

    printf(CONSOLE_ESC(35;1m) "Welcome to IPSwitch. Do no evil.\n\n"
        CONSOLE_ESC(m));

    mainMenu();

    gfxExit();
    return 0;
}
