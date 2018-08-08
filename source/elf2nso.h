#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <lz4.h>
#include "sha256.h"
#include "elf64.h"

#ifndef ELF2NSO_H
#define ELF2NSO_H

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint8_t u8;

typedef struct {
    u32 FileOff;
    u32 DstOff;
    u32 DecompSz;
    u32 AlignOrTotalSz;
} NsoSegment;

typedef u8 Sha2Hash[0x20];

typedef struct {
    u8  Magic[4];
    u32 Unk1;
    u32 Unk2;
    u32 Unk3;
    NsoSegment Segments[3];
    u8  BuildId[0x20];
    u32 CompSz[3];
    u8  Padding[0x24];
    u64 Unk4;
    u64 Unk5;
    Sha2Hash Hashes[3];    
} NsoHeader;

uint8_t* ReadEntireFile(const char* fn, size_t* len_out);

int elf2nso(uint8_t* elf, size_t elf_len, FILE* out);

#endif
