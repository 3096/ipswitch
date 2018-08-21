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

void mainMenu();

u64 patchTextSelect();

#endif
