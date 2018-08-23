#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <switch.h>

#include "util.h"

#define CONSOLE_WIDTH 80

#define EXCEED_SYMBOL "..."

#define TOGGLE_ENABLED			0x6500 // "\0e"
#define TOGGLE_DISABLED 		0x6400 // "\0d"
#define TOGGLE_ENABLED_CHANGED	0x4500 // "\0E"
#define TOGGLE_DISABLED_CHANGED	0x4400 // "\0D"

u16 getStringTailU16(char* str);

u64 selectFromList(int* selection, StrList* str_list);

bool userConfirm(const char* msg);

void printInProgress(const char* msg);
void printDone();

#endif
