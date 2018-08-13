#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <switch.h>

#ifndef UTIL_H
#define UTIL_H

#define VALID_HEX_CHAR "0123456789abcdefABCDEF"

typedef char String[0x100];

typedef struct {
	size_t size;
    size_t size_max;
    String* str_list;
    char** str_list_ptr;
} StrList;

StrList* getStrList();
void addToStrList(StrList* list_ptr, const char* str);
void freeStrList(StrList* list_ptr);

int isValidHexStr(const char* str);

int isDirectory(const char* path);

int bytesEndianSwap(u8* buf, size_t size);

// Does endian swap on str representation of hex
// e.g. "EFBEADDE" -> "DEADBEEF"
int strEndianSwap(char* str);

int strToLowerCase(char* str);

int strcpysize(char* dest, const char* src, size_t size);

void selectIndex(int* selection, char* list[], int size, int change);
u64 selectFromList(int* selection, char* list[], int size);

bool userConfirm(const char* msg);

void printInProgress(const char * msg);
void printDone();

#endif
