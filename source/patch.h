#include <stdio.h>

#include <switch.h>

#include "util.h"
#include "elf2nso.h"

#ifndef PATCH_H
#define PATCH_H

#define PATCH_TEXT_FORMAT ".pchtxt"

#define ATMOS_TITLE_DIR "/atmosphere/titles/"

#define NSOBID_MAGIC_LOWER 0x2D6469626F736E40 // "@nsobid-"
#define NSOBID_MAGIC_UPPER 0x2D4449424F534E40 // "@NSOBID-"

typedef struct {
	char tid_str[17];
    char elf_dir[0x100];
    char patch_dir[0x100];
} PatchTarget;

typedef struct {
	char tid_str[17];
    char patch_txt_dir[0x100];
    char nso_name[0x100];
} PatchTextTarget;

void getPatchTextPath(const PatchTextTarget* target, char* path);

typedef struct {
    u32 offset;
    u32 value;
} Patch;
typedef struct PatchListNode {
	Patch patch;
	struct PatchListNode* next;
} PatchListNode;
typedef struct {
	size_t size;
    PatchListNode* first;
    PatchListNode* head;
    char nsobid[65];
} PatchList;

PatchList* initPatchList();
int addPatchToList(PatchList* pchlist, Patch patch);
void freePatchList(PatchList* pchlist);

int getPatchFromLine(char* line, Patch* patch, bool isLittleEndian);

int parsePatchText(const PatchTextTarget* p_target, PatchList* pchlist);

int patchTarget(PatchTarget target);

#endif
