#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <switch.h>

#include "util.h"

u64 selectFromList(int* selection, StrList* str_list);

bool userConfirm(const char* msg);

void printInProgress(const char* msg);
void printDone();

#endif
