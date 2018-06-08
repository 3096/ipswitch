#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <switch.h>

#ifndef UTIL_H
#define UTIL_H

#define VALID_HEX_CHAR "0123456789abcdefABCDEF"

int isValidHexStr(const char* str);

int isDirectory(const char* path);

void selectIndex(int* selection, char* list[], int size, int change);

int selectFromList(int* selection, char* list[], int size);

bool userConfirm(const char* msg);

#endif
