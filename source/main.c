#include <switch.h>

#include "menu.h"

int main(int argc, char **argv)
{
    gfxInitDefault();
    consoleInit(NULL);

    printf("Welcome to IPSwitch. Do no evil.\n\n");

    titleSelect();

    gfxExit();
    return 0;
}
