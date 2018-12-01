#ifndef PATCH_H
#define PATCH_H

#include <stdio.h>

#include <switch.h>

#include "config.h"
#include "util.h"
#include "console.h"
#include "elf2nso.h"

#define PATCH_TEXT_FORMAT ".pchtxt"

#define ATMOS_DIR        "/atmosphere/"
#define ATMOS_TITLE_DIR  "/atmosphere/titles/"
#define ATMOS_EXEPCH_DIR "/atmosphere/exefs_patches/"

#define NSOBID_MAGIC_LOWER 0x2D6469626F736E40 // "@nsobid-"
#define NSOBID_MAGIC_UPPER 0x2D4449424F534E40 // "@NSOBID-"

#define LINE_IGNORE_CHARS " /\r\n"
// a line should at least have 2 chars as value
// `DEADBEEF ""` or `DEADBEEF 00`
#define PATCH_LINE_MIN_SIZE 11 

#define ENABLED_FLAG    "@enabled\n"
#define DISABLED_FLAG   "@disabled\n"

#define OFFSET_SHIFT_FLAG "offset_shift"
#define PRINT_VALUE_FLAG "print_values"

#define IPS32_HEAD_MAGIC "IPS32"
#define IPS32_FOOT_MAGIC "EEOF"

#define IPS32_OFFSET_AND_LEN_SIZE 0x6

typedef enum {
    PATCH_MODE_ELF2NSO = 0,
    PATCH_MODE_IPS,
} PatchMode;

typedef struct {
    char tid_str[17];
    char patch_txt_path[0x100];
    char name[0x100];  // used as nso name
    char folder_name[0x100];
} PatchTextTarget;

void getPatchTextName(char* name, const char* path);

typedef enum {
    PATCH_TYPE_BYTE = 0,
    PATCH_TYPE_STRING,
} PatchType;

typedef struct {
    u32 offset;
    u16 len;
    PatchType type;
    u8 value[LINE_MAX_SIZE];
} Patch;
typedef struct PatchListNode {
    Patch patch;
    struct PatchListNode* next;
} PatchListNode;
typedef struct {
    size_t size;
    size_t total_value_len;
    PatchListNode* first;
    PatchListNode* head;
    PatchTextTarget target;
    char nsobid[65];
    bool printing_values;
} PatchList;

PatchList* initPatchList();
int addPatchToList(PatchList* pchlist, Patch patch);
void freePatchList(PatchList* pchlist);

int parsePatchText(PatchList* pchlist);

int patchTarget(const PatchList* pchlist);

int patchTextToIPS(PatchTextTarget* pchtxt_target);

int readPchtxtIntoStrList(PatchTextTarget* pchtxt_target, StrList* pchtxt,
                          StrList* patch_str_list);
int writePchtxtFromStrList(PatchTextTarget* pchtxt_target, StrList* pchtxt,
                           StrList* patch_str_list);

#endif
