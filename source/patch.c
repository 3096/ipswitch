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

PatchList* initPatchList() {
    PatchList* pchlist = malloc(sizeof(PatchList));
    pchlist->size = 0;
    pchlist->first = NULL;
    pchlist->head = NULL;
    pchlist->nsobid[0] = '\0';
    return pchlist;
}

PatchListNode* getPatchNode(Patch patch) {
    PatchListNode* new_node = malloc(sizeof(PatchListNode));
    new_node->patch = patch;
    new_node->next = NULL;
    return new_node;
}

int addPatchToList(PatchList* pchlist, Patch patch) {
    PatchListNode* new_node = getPatchNode(patch);
    if(pchlist->head == NULL) {
        pchlist->first = new_node;
        pchlist->head = new_node;
    } else {
        pchlist->head->next = new_node;
        pchlist->head = new_node;
    }
    pchlist->size++;

    return 0;
}

void freePatchList(PatchList* pchlist) {
    PatchListNode* node = pchlist->first;
    while(node != NULL) {
        node = node->next;
        free(node);
    }
    free(pchlist);
}

int parsePatchText(PatchList* pchlist) {
    printf("\nReading patch text file:\n");

    FILE * patch_file;
    char patch_txt_path[0x100];
    getPatchTextPath(&pchlist->target, patch_txt_path);
    patch_file = fopen(patch_txt_path, "r");
    if (patch_file == NULL) {
        fclose(patch_file);
        printf("Cannot open %s, ", patch_txt_path);
        return -2;
    }

    // some values used in parsing
    int offset_shift = 0;

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
    pchlist->nsobid[64] = '\0';
    if(fgets(line, LINE_MAX_SIZE-1, patch_file) != NULL) {
        // line 2 nso build id, nso_name doubles as pchtxt file name here
        char first_three[4];
        strcpysize(first_three, pchlist->target.nso_name, 3);
        strToLowerCase(first_three);
        if( strcmp(first_three, "ips") == 0 && 
            (*(u64*)line == NSOBID_MAGIC_LOWER
            || *(u64*)line == NSOBID_MAGIC_UPPER) &&
            strlen(line) > 8 ) {
            // parse nso build id
            for (int i = 0; i < 64; i++) {
                char buf[2]; buf[1] = '\0';
                buf[0] = line[i+8];
                if(isValidHexStr(buf)) {
                    pchlist->nsobid[i] = buf[0];
                } else {
                    pchlist->nsobid[i] = '\0';
                    break;
                }
            }

            if(fgets(line, LINE_MAX_SIZE-1, patch_file) == NULL) {
                goto stop;
            }
        }

        // parsing body
        bool enabled = false;
        do {
            switch(line[0]){
            case '#':
                printf("\n%s\n", line);
                continue;
            case '/':
                strcpy(last_comment, line);
                continue;
            case '@':

                switch(line[1]) {
                case 'e':
                case 'E':
                    enabled = true;
                    printf("Patch read: %s", last_comment);
                    break;
                case 'f':
                case 'F':
                    if(strlen(line) < 7)
                        break;
                    size_t flag_name_len = strcspn(&line[6], " /\n");
                    char flag_name_buf[0x100];
                    strcpysize(flag_name_buf, &line[6], flag_name_len);
                    strToLowerCase(flag_name_buf);

                    if(strcmp(flag_name_buf, OFFSET_SHIFT_FLAG) == 0) {
                        if(strlen(line) < 20)
                            break;
                        char flag_val_buf[0x100];
                        strcpysize(flag_val_buf, &line[6+flag_name_len+1],
                            strcspn(&line[6+flag_name_len+1], " /\n"));
                        offset_shift = (int)strtol(flag_val_buf, NULL, 0);
                        printf("Flag: %s 0x%X\n", 
                            flag_name_buf, offset_shift);
                    }
                    break;
                case 's':
                case 'S':
                    goto stop; // parsing reached "@stop"
                default:
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

                patch.offset += offset_shift;
                addPatchToList(pchlist, patch);
            }
        } while (fgets(line, LINE_MAX_SIZE-1, patch_file) != NULL);
    }
stop:
    fclose(patch_file);
    return 0;
}

int patchTarget(const PatchList* pchlist) {
    int ret;

    uint8_t* out_file_buf = NULL;
    size_t out_file_buf_size = 0;

    PatchMode mode = PATCH_MODE_IPS;
    const size_t IPS_HEAD_LEN = strlen(IPS32_HEAD_MAGIC);
    const size_t IPS_FOOT_LEN = strlen(IPS32_FOOT_MAGIC);
    if(strlen(pchlist->nsobid) == 0) {
        mode = PATCH_MODE_ELF2NSO;
        printInProgress("\nReading elf");
        char elf_dir[0x100];
        strcpy(elf_dir, pchlist->target.patch_txt_dir);
        strcat(elf_dir, pchlist->target.nso_name);
        strcat(elf_dir, ".elf");
        out_file_buf = ReadEntireFile(elf_dir, &out_file_buf_size);
        if (out_file_buf == NULL) {
            printf("Failed to open %s, ", elf_dir);
            return -1;
        }
        printDone();
    } else {
        out_file_buf_size = IPS_HEAD_LEN
            + IPS32_4BYTE_PATCHBLOCK_SIZE * pchlist->size + IPS_FOOT_LEN;
        out_file_buf = malloc(out_file_buf_size);
        memcpy(out_file_buf, IPS32_HEAD_MAGIC, IPS_HEAD_LEN);
    }

    printf("\nApplying patches:\n\n");

    PatchListNode* node = pchlist->first;
    for(int i = 0; node != NULL; i++) {
        printf("%08X: ", node->patch.offset);

        if(mode == PATCH_MODE_ELF2NSO) {
            printf("%08X ->", *(u32*)&out_file_buf[node->patch.offset]);
            *(u32*)&out_file_buf[node->patch.offset] = node->patch.value;
        } else {
            size_t cur_block_offset = 
                IPS_HEAD_LEN + IPS32_4BYTE_PATCHBLOCK_SIZE * i;

            u32 offset_BE = node->patch.offset;
            bytesEndianSwap((u8*)&offset_BE, sizeof(offset_BE));
            *(u32*)&out_file_buf[cur_block_offset] = offset_BE;

            u16 len_BE = 0x400; // as 4 in big endian
            *(u32*)&out_file_buf[cur_block_offset + 4] = len_BE;

            u32 value_BE = node->patch.value;
            *(u32*)&out_file_buf[cur_block_offset + 6] = value_BE;
        }
        printf("%08X\n", node->patch.value);
        
        node = node->next;
    }

    char out_file_path[0x100]; FILE* out;
    if(mode == PATCH_MODE_ELF2NSO) {
        strcpy(out_file_path, ATMOS_TITLE_DIR);
        strcat(out_file_path, pchlist->target.tid_str);
        mkdir(out_file_path, 0700);
        strcat(out_file_path, "/exefs/");
        mkdir(out_file_path, 0700);
        strcat(out_file_path, pchlist->target.nso_name);

        printInProgress("\nCompressing into nso");
        out = fopen(out_file_path, "wb");
        ret = elf2nso(out_file_buf, out_file_buf_size, out);
        printDone();

        printf("\nNSO output to %s\n", out_file_path);
        
    } else {
        memcpy(out_file_buf+out_file_buf_size-IPS_FOOT_LEN,
            IPS32_FOOT_MAGIC, IPS_FOOT_LEN);

        strcpy(out_file_path, ATMOS_EXEPCH_DIR);
        mkdir(out_file_path, 0700);
        strcat(out_file_path, pchlist->target.tid_str);
        mkdir(out_file_path, 0700);
        strcat(out_file_path, "/");
        strcat(out_file_path, pchlist->nsobid);
        strcat(out_file_path, ".ips");
        
        printInProgress("\nWriting .ips file");
        out = fopen(out_file_path, "wb");
        ret = fwrite(out_file_buf, out_file_buf_size, 1, out);
        if(ret == 1) ret = 0;
        printDone();

        printf("\nIPS output to %s\n", out_file_path);
    }

    fclose(out);
    free(out_file_buf);

    return ret;
}
