#include "patch.h"

void getPatchTextPath(const PatchTextTarget* p_target, char* path) {
    strcpy(path, p_target->patch_txt_dir);
    strcat(path, p_target->nso_name);
    strcat(path, PATCH_TEXT_FORMAT);
}

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

int parsePatchText(const PatchTextTarget* p_target) {
    printf("Reading patch text file:\n\n");

    FILE * patch_file;
    char patch_txt_path[0x100];
    getPatchTextPath(p_target, patch_txt_path);
    patch_file = fopen(patch_txt_path, "r");
    if (patch_file == NULL) {
        fclose(patch_file);
        printf("Cannot open %s, ", patch_txt_path);
        return -2;
    }

    // line buffers
    const size_t LINE_MAX_SIZE = 0x100;
    char line[LINE_MAX_SIZE]; line[LINE_MAX_SIZE] = '\0';
    char last_comment[LINE_MAX_SIZE]; last_comment[LINE_MAX_SIZE] = '\0';

    // line 1 Endianness 
    bool isLittleEndian = true;
    if(fgets(line, LINE_MAX_SIZE-1, patch_file) != NULL) {
        if(line[0] != '@') {
            printf("Error: Please specify Endianess in the first line using");
            printf("\n\"@little-endian\" or \"@big-endian\"\n");
            fclose(patch_file);
            return -3;
        } else {
            if(line[1] == 'b' || line[1] == 'B')
                isLittleEndian = false;
        }
    }
    bool isIPS = false; char nso_build_id[65]; nso_build_id[64] = '\0';
    if(fgets(line, LINE_MAX_SIZE-1, patch_file) != NULL) {
        // line 2 nso build id, nso_name doubles as pchtxt file name here
        char first_three[4]; strcpysize(first_three, p_target->nso_name, 3);
        if( (strcmp(first_three, "ips") == 0
            || strcmp(first_three, "IPS") == 0) && 
            (*(u64*)line == NSOBID_MAGIC_LOWER
            || *(u64*)line == NSOBID_MAGIC_UPPER) &&
            strlen(line) > 8 ) {

            isIPS = true;
            // parse nso build id
            for (int i = 0; i < 64; i++) {
                char buf[2]; buf[1] = '\0';
                buf[0] = line[i+8];
                if(isValidHexStr(buf)) {
                    nso_build_id[i] = buf[0];
                } else {
                    nso_build_id[i] = '\0';
                    break;
                }
            }

            fgets(line, LINE_MAX_SIZE-1, patch_file);
        }

        // parsing body
        bool enabled = false;
        do {
            switch(line[0]){
            case '#':
                printf("\n%s", line);
                continue;
            case '/':
                strcpy(last_comment, line);
                continue;
            case '@':
                if (line[1] == 's' || line[1] == 'S') {
                    break; // parsing reached "@stop"
                }
                else if (line[1] == 'e' || line[1] == 'E') {
                    enabled = true;
                    printf("\nPatching %s", last_comment);
                }
                else {
                    enabled = false;
                }
                continue;
            default:
                if(!enabled)
                    continue;
                if(strlen(line) < 17)
                    continue;

                Patch patch; int ret = 0;
                ret = getPatchFromLine(line, &patch, isLittleEndian);
                if(ret != 0)
                    continue;

                printf("%08X: ", patch.offset);
                printf("%08X\n", patch.value);

                //add patch to a list
            }
        } while (fgets(line, LINE_MAX_SIZE-1, patch_file) != NULL);
    }

    fclose(patch_file);
    return 0;
}

int patchTarget(PatchTarget target) {
    int ret;
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
            if (line[1] == 's') {
                break;
            }
            else if (line[1] == 'e') {
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
        ret = getPatchFromLine(line, &patch, isLittleEndian);
        if(ret != 0)
        	continue;

        printf("%08X: ", patch.offset);
        printf("%08X -> ", *(u32*)(elf+patch.offset));
        printf("%08X\n", patch.value);

        *(u32*)(elf+patch.offset) = patch.value;
    }

    fclose(patch_file);

    printf("\nDone\n\nCompressing into nso... ");

    char out_dir[0x100];
    strcpy(out_dir, ATMOS_TITLE_DIR);
    strcat(out_dir, target.tid_str);
    mkdir(out_dir, 0700);
    strcat(out_dir, "/exefs");
    mkdir(out_dir, 0700);
    strcat(out_dir, "/main");

    printf("\n%s\n", out_dir);
    printf("\n%s\n", out_dir);

    ret = elf2nso(elf, elf_len, out_dir);
    free(elf);
    return ret;
}
