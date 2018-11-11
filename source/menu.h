#ifndef MENU_H
#define MENU_H

#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include <switch.h>

#include "console.h"
#include "patch.h"
#include "util.h"

#define IPSWITCH_DIR "/switch/ipswitch/"

u64 patchTextSelect();

void mainMenu();

u64 patchTextToIPSMenu();

u64 patchTextToggleMenu();

#endif
