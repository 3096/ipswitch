#include <stdio.h>

#include <switch.h>

#include "util.h"
#include "elf2nso.h"

#ifndef PATCH_H
#define PATCH_H

#define ATMOS_TITLE_DIR "/atmosphere/titles/"

typedef struct {
	char tid_str[17];
    char elf_dir[0x100];
    char patch_dir[0x100];
} PatchTarget;

typedef struct {
    u32 offset;
    u32 value;
} Patch;

int getPatchFromLine(char* line, Patch* patch, bool isLittleEndian);

int patchTarget(PatchTarget target);

#endif
