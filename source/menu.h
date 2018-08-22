#ifndef MENU_H
#define MENU_H

#include <string.h>
#include <stdio.h>
#include <dirent.h>

#include <switch.h>

#include "util.h"
#include "console.h"
#include "patch.h"

#define IPSWITCH_DIR "/switch/ipswitch/"

u64 patchTextSelect();

void mainMenu();

u64 patchTextToIPSMenu();

u64 patchTextToggleMenu();

#endif
