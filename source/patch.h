#ifndef PATCH_H
#define PATCH_H

#include <stdio.h>

#include <switch.h>

#include "util.h"
#include "console.h"
#include "elf2nso.h"

#define PATCH_TEXT_FORMAT ".pchtxt"

#define ATMOS_TITLE_DIR	 "/atmosphere/titles/"
#define ATMOS_EXEPCH_DIR "/atmosphere/exefs_patches/"

#define NSOBID_MAGIC_LOWER 0x2D6469626F736E40 // "@nsobid-"
#define NSOBID_MAGIC_UPPER 0x2D4449424F534E40 // "@NSOBID-"

#define ENABLED_FLAG	"@enabled\n"
#define DISABLED_FLAG	"@disabled\n"

#define OFFSET_SHIFT_FLAG "offset_shift"

#define IPS32_HEAD_MAGIC "IPS32"
#define IPS32_FOOT_MAGIC "EEOF"

#define IPS32_4BYTE_PATCHBLOCK_SIZE 0xA

typedef enum {
	PATCH_MODE_ELF2NSO = 0,
	PATCH_MODE_IPS,
} PatchMode;

typedef struct {
	char tid_str[17];
    char patch_txt_path[0x100];
    char name[0x100]; // used as nso name
    char folder_name[0x100];
} PatchTextTarget;

void getPatchTextName(char* name, const char* path);

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
    PatchTextTarget target;
    char nsobid[65];
} PatchList;

PatchList* initPatchList();
int addPatchToList(PatchList* pchlist, Patch patch);
void freePatchList(PatchList* pchlist);

int getPatchFromLine(char* line, Patch* patch, bool isLittleEndian);

int parsePatchText(PatchList* pchlist);

int patchTarget(const PatchList* pchlist);

int readPchtxtIntoStrList(PatchTextTarget* pchtxt_target,
    StrList* pchtxt, StrList* patch_str_list);
int writePchtxtFromStrList(PatchTextTarget* pchtxt_target,
    StrList* pchtxt, StrList* patch_str_list);

#endif
