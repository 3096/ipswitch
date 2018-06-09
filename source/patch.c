#include "patch.h"

int getPatchFromLine(char* line, Patch* patch, bool isLittleEndian) {
    char offset_str[9];
    offset_str[8] = '\0';
    char value_str[9];
    value_str[8] = '\0';

    memcpy(offset_str, line, 8);
    memcpy(value_str, &line[9], 8);
    if(!isValidHexStr(offset_str) || !isValidHexStr(value_str))
        return -1;

    if(isLittleEndian) {
        strEndianSwap(value_str);
    }

    patch->offset = (uint32_t)strtol(offset_str, NULL, 16);
    patch->value = (uint32_t)strtol(value_str, NULL, 16);

    return 0;
}

int patchTarget(PatchTarget target) {
    printf("Reading elf... ");

    size_t elf_len;
    uint8_t* elf = ReadEntireFile(target.elf_dir, &elf_len);
    if (elf == NULL) {
        printf("Failed to open %s, ", target.elf_dir);
        return -1;
    }

    printf("Done\n\nApplying patches...\n");

    FILE * patch_file;
    int len = 0x99;
    char line[len+1];
    line[len] = '\0';
    char last_comment[len+1];

    patch_file = fopen(target.patch_dir, "r");
    if (patch_file == NULL) {
        printf("Cannot open %s, ", target.patch_dir);
        return -2;
    }

    bool isLittleEndian = true;
    if(fgets(line, len, patch_file) != NULL) {
        if(line[0] != '@') {
            printf("Error: Please specify Endianess in the first line using\n");
            printf("\"@little-endian\" or \"@big-endian\"\n");
            return -3;
        } else {
            if(line[1] == 'b' || line[1] == 'B')
                isLittleEndian = false;
        }
    }

    bool enabled = false;
    while (fgets(line, len, patch_file) != NULL) {
        if(line[0] == '/') {
            strcpy(last_comment, line);
            continue;
        }
        if(line[0] == '@') {
            if(line[1] == 'e') {
                enabled = true;
                printf("\nPatching %s", last_comment);
            }
            else
                enabled = false;
            continue;
        }
        if(!enabled)
        	continue;
        if(strlen(line) < 17)
            continue;

        Patch patch;
        int ret = getPatchFromLine(line, &patch, isLittleEndian);
        if(ret != 0)
        	continue;

        printf("%08X ", patch.offset);
        printf("%08X ", *(u32*)(elf+patch.offset));
        printf("%08X\n", patch.value);
        *(u32*)(elf+patch.offset) = patch.value;
    }

    fclose(patch_file);

    char out_dir[0x100];
    strcpy(out_dir, ATMOS_TITLE_DIR);
    strcat(out_dir, target.tid_str);
    strcat(out_dir, "/exefs/main");

    elf2nso(elf, elf_len, out_dir);

	return 0;
}
