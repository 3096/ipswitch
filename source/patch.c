#include "patch.h"

void getPatchTextName(char* name, const char* path) {
    size_t start_idx = strrchr(path, '/') - path + 1;
    size_t end_idx = strrchr(path, '.') - path;
    strcpysize(name, &path[start_idx], end_idx - start_idx);
}

int getPatchFromLine(char* line, Patch* patch) {
    char offset_str[9];
    offset_str[8] = '\0';
    memcpy(offset_str, line, 8);
    if (!isValidHexStr(offset_str)) return -1;
    patch->offset = (uint32_t)strtol(offset_str, NULL, 16);

    char* value_str = &line[9];
    if (value_str[0] == '\"') {
        char* str_end = value_str;
        while ((str_end = strchr(str_end + 1, '\"')) != NULL) {
            if (*(str_end - 1) != '\\') {
                break;
            }
        }
        if (str_end == NULL) {
            return -4;
        }
        u16 value_len = str_end - value_str;
        memcpy(patch->value, &value_str[1], value_len - 1);
        patch->value[value_len - 1] = '\0';

        size_t value_len_after_escape = 0;
        if (escapeString((char*)patch->value, &value_len_after_escape) != 0) {
            return -5;
        }
        patch->len = (u16)value_len_after_escape + 1;
        patch->type = PATCH_TYPE_STRING;
    } else {
        u16 value_str_len = strcspn(&line[9], LINE_IGNORE_CHARS);
        if (value_str_len % 2 != 0) {
            return -3;
        }

        value_str[value_str_len] = '\0';
        if (!isValidHexStr(value_str)) {
            return -2;
        }

        for (u16 i = 0; i < value_str_len / 2; i++) {
            char byte_str_buff[3] = {0};
            memcpy(byte_str_buff, &value_str[i * 2], 2);
            patch->value[i] = (u8)strtol(byte_str_buff, NULL, 16);
        }
        patch->len = value_str_len / 2;
        patch->type = PATCH_TYPE_BYTE;
    }

    return 0;
}

PatchList* initPatchList() {
    PatchList* pchlist = malloc(sizeof(PatchList));
    pchlist->size = 0;
    pchlist->total_value_len = 0;
    pchlist->first = NULL;
    pchlist->head = NULL;
    pchlist->nsobid[0] = '\0';
    pchlist->printing_values = false;
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
    if (pchlist->head == NULL) {
        pchlist->first = new_node;
        pchlist->head = new_node;
    } else {
        pchlist->head->next = new_node;
        pchlist->head = new_node;
    }
    pchlist->size++;
    pchlist->total_value_len += patch.len;

    return 0;
}

void freePatchList(PatchList* pchlist) {
    PatchListNode* node = pchlist->first;
    while (node != NULL) {
        node = node->next;
        free(node);
    }
    free(pchlist);
}

int parsePatchText(PatchList* pchlist) {
    printf("\nReading patch text file:\n");

    FILE* pchtxt_file;
    pchtxt_file = fopen(pchlist->target.patch_txt_path, "r");
    if (pchtxt_file == NULL) {
        fclose(pchtxt_file);
        printf(CONSOLE_ESC(31m) "Cannot open %s, " CONSOLE_ESC(m),
               pchlist->target.patch_txt_path);
        return -2;
    }

    // some values used in parsing
    int offset_shift = 0;

    // line buffers
    char line[LINE_MAX_SIZE];
    line[LINE_MAX_SIZE - 1] = '\0';
    char last_comment[LINE_MAX_SIZE];
    last_comment[LINE_MAX_SIZE - 1] = '\0';

    // get line
    if (fgets(line, LINE_MAX_SIZE - 1, pchtxt_file) == NULL) goto stop;

    // check for old patch texts with endian specification
    // cuz it's no longer in use as Big-Endian is no longer supported.
    if (line[0] == '@') {
        switch (line[1]) {
            case 'b':
            case 'B':
                printf(CONSOLE_ESC(33m) "Big-Endian is no longer supported. "
                         "Proceeding as little-endian\n" CONSOLE_ESC(m));
            case 'l':
            case 'L':
                if (fgets(line, LINE_MAX_SIZE - 1, pchtxt_file) == NULL) {
                    goto stop;
                }
        }
    }

    pchlist->nsobid[64] = '\0';
    if ((*(u64*)line == NSOBID_MAGIC_LOWER ||
         *(u64*)line == NSOBID_MAGIC_UPPER) &&
        strlen(line) > 8) {
        // parse nso build id
        for (int i = 0; i < 64; i++) {
            char buf[2];
            buf[1] = '\0';
            buf[0] = line[i + 8];
            if (isValidHexStr(buf)) {
                pchlist->nsobid[i] = buf[0];
            } else {
                pchlist->nsobid[i] = '\0';
                break;
            }
        }
        if (fgets(line, LINE_MAX_SIZE - 1, pchtxt_file) == NULL) {
            goto stop;
        }
    }

    // parsing body
    bool enabled = false;
    do {
        switch (line[0]) {
            case '#':
                printf(CONSOLE_ESC(33;1m) "\n%s\n" CONSOLE_ESC(m), line);
                continue;
            case '/':
                strcpy(last_comment, line);
                continue;
            case '@':

                switch (line[1]) {
                    case 'e':
                    case 'E':
                        enabled = true;
                        printf(
                            CONSOLE_ESC(36m) "Patch read: %s" CONSOLE_ESC(m),
                            last_comment);
                        break;
                    case 'f':
                    case 'F':
                        if (strlen(line) < 7) break;
                        size_t flag_name_len =
                            strcspn(&line[6], LINE_IGNORE_CHARS);
                        char flag_name_buf[0x100] = {0};
                        strcpysize(flag_name_buf, &line[6], flag_name_len);
                        printf(CONSOLE_ESC(34;1m) "Flag: %s ", flag_name_buf);
                        strToLowerCase(flag_name_buf);

                        // flags with no need for a value goes here
                        if (strcmp(flag_name_buf, PRINT_VALUE_FLAG) == 0) {
                            pchlist->printing_values = true;
                        } else {
                            if (strlen(line) < strlen(flag_name_buf) + 8)
                                goto unknown_flag;
                            char flag_val_buf[0x100] = {0};
                            strcpysize(flag_val_buf,
                                       &line[6 + flag_name_len + 1],
                                       strcspn(&line[6 + flag_name_len + 1],
                                               LINE_IGNORE_CHARS));

                            // flags with need for a value goes here
                            if (strcmp(flag_name_buf, OFFSET_SHIFT_FLAG) ==
                                0) {
                                offset_shift =
                                    (int)strtol(flag_val_buf, NULL, 0);
                                printf("0x%X", offset_shift);
                            } else {
                            unknown_flag:
                                printf(CONSOLE_ESC(0;33m) "Unknown Flag");
                            }
                        }
                        printf(CONSOLE_ESC(m) "\n");

                        break;
                    case 's':
                    case 'S':
                        goto stop;  // parsing reached "@stop"
                    default:
                        enabled = false;
                }
                continue;
            default:
                if (!enabled) continue;
                if (strlen(line) < PATCH_LINE_MIN_SIZE) continue;

                Patch patch = {};
                int ret = 0;
                ret = getPatchFromLine(line, &patch);
                if (ret != 0) continue;

                patch.offset += offset_shift;
                addPatchToList(pchlist, patch);
        }
    } while (fgets(line, LINE_MAX_SIZE - 1, pchtxt_file) != NULL);
stop:
    fclose(pchtxt_file);
    return 0;
}

int patchTarget(const PatchList* pchlist) {
    int ret;

    uint8_t* out_file_buf = NULL;
    size_t out_file_buf_size = 0;

    PatchMode mode = PATCH_MODE_IPS;
    const size_t IPS_HEAD_LEN = strlen(IPS32_HEAD_MAGIC);
    const size_t IPS_FOOT_LEN = strlen(IPS32_FOOT_MAGIC);
    if (strlen(pchlist->nsobid) == 0) {
        mode = PATCH_MODE_ELF2NSO;
        if (strlen(pchlist->target.tid_str) == 0) {
            printf(CONSOLE_ESC(31m)
                "Failed to find output target." CONSOLE_ESC(m));
            return -2;
        }

        printInProgress("\nReading elf");
        char elf_dir[0x100] = {0};
        strcpysize(elf_dir, pchlist->target.patch_txt_path,
                   strlen(pchlist->target.patch_txt_path) -
                       strlen(PATCH_TEXT_FORMAT));
        strcat(elf_dir, ".elf");
        out_file_buf = ReadEntireFile(elf_dir, &out_file_buf_size);
        if (out_file_buf == NULL) {
            printf(CONSOLE_ESC(31m) "Failed to open %s, " CONSOLE_ESC(m),
                   elf_dir);
            return -1;
        }
        printDone();
    } else {
        out_file_buf_size = IPS_HEAD_LEN + pchlist->total_value_len +
                            IPS32_OFFSET_AND_LEN_SIZE * pchlist->size +
                            IPS_FOOT_LEN;
        out_file_buf = malloc(out_file_buf_size);
        memcpy(out_file_buf, IPS32_HEAD_MAGIC, IPS_HEAD_LEN);
    }

    if (pchlist->printing_values)
        printf("\nApplying patches:\n\n");
    else
        printInProgress("\nApplying patches");

    PatchListNode* node = pchlist->first;
    size_t cur_ips_buf_offset = IPS_HEAD_LEN;
    for (int i = 0; node != NULL; i++) {
        if (pchlist->printing_values)
            printf(CONSOLE_ESC(36m) "%08X: ", node->patch.offset);

        if (mode == PATCH_MODE_ELF2NSO) {
            if (pchlist->printing_values) {
                if (node->patch.type == PATCH_TYPE_BYTE) {
                    printBytesAsHex(&out_file_buf[node->patch.offset],
                                    node->patch.len);
                } else {
                    printf((char*)&out_file_buf[node->patch.offset]);
                }
                printf(" -> ");
            }
            memcpy(&out_file_buf[node->patch.offset], node->patch.value,
                   node->patch.len);
        } else {
            u32 offset_BE = node->patch.offset;
            bytesEndianSwap((u8*)&offset_BE, sizeof(offset_BE));
            *(u32*)&out_file_buf[cur_ips_buf_offset] = offset_BE;
            cur_ips_buf_offset += sizeof(offset_BE);

            u16 len_BE = node->patch.len;
            bytesEndianSwap((u8*)&len_BE, sizeof(len_BE));
            *(u16*)&out_file_buf[cur_ips_buf_offset] = len_BE;
            cur_ips_buf_offset += sizeof(len_BE);

            memcpy(&out_file_buf[cur_ips_buf_offset], node->patch.value,
                   node->patch.len);
            cur_ips_buf_offset += node->patch.len;
        }

        if (pchlist->printing_values) {
            if (node->patch.type == PATCH_TYPE_BYTE) {
                printBytesAsHex(node->patch.value, node->patch.len);
            } else {
                printf((char*)node->patch.value);
            }

            printf(CONSOLE_ESC(m) "\n");
        }

        node = node->next;
    }

    if (!pchlist->printing_values) printDone();

    char out_file_path[0x100] = {0};
    FILE* out;
    if (mode == PATCH_MODE_ELF2NSO) {
        strcpy(out_file_path, ATMOS_TITLE_DIR);
        strcat(out_file_path, pchlist->target.tid_str);
        mkdir(out_file_path, 0700);
        strcat(out_file_path, "/exefs/");
        mkdir(out_file_path, 0700);
        strcat(out_file_path, pchlist->target.name);

        printInProgress("\nCompressing into nso");
        out = fopen(out_file_path, "wb");
        ret = elf2nso(out_file_buf, out_file_buf_size, out);
        printDone();

        printf("\nNSO output to\n%s\n", out_file_path);

    } else {
        memcpy(out_file_buf + out_file_buf_size - IPS_FOOT_LEN,
               IPS32_FOOT_MAGIC, IPS_FOOT_LEN);

        strcpy(out_file_path, ATMOS_EXEPCH_DIR);
        mkdir(out_file_path, 0700);
        strcat(out_file_path, pchlist->target.folder_name);
        mkdir(out_file_path, 0700);
        strcat(out_file_path, "/");
        strcat(out_file_path, pchlist->nsobid);
        strcat(out_file_path, ".ips");

        printInProgress("\nWriting .ips file");
        out = fopen(out_file_path, "wb");
        ret = fwrite(out_file_buf, out_file_buf_size, 1, out);
        if (ret == 1) ret = 0;
        printDone();

        printf("\nIPS output to\n%s\n", out_file_path);
    }

    fclose(out);
    free(out_file_buf);

    return ret;
}

int patchTextToIPS(PatchTextTarget* pchtxt_target) {
    int rc = 0;

    PatchList* pchlist = initPatchList();
    pchlist->target = *pchtxt_target;
    rc = parsePatchText(pchlist);
    if (!R_SUCCEEDED(rc)) {
        return rc;
    }
    rc = patchTarget(pchlist);
    free(pchlist);

    // for elf fail and people just didn't put nsobid for ips
    if(rc == -1 || rc == -2) {
        printf("Did you forget to include nsobid?\n");
    }

    return rc;
}

int addPatchCommentToToggleList(StrList* patch_str_list, char* comment,
                                u32 line_idx, u16 stringTail) {
    char* new_line_char = strchr(comment, '\n');
    if (new_line_char != NULL) *new_line_char = '\0';
    comment[0xF9] = '\0';
    *(u32*)&comment[0xFA] = line_idx;
    *(u16*)&comment[0xFE] = stringTail;
    addBytesToStrList(patch_str_list, comment);

    return 0;
}

int readPchtxtIntoStrList(PatchTextTarget* pchtxt_target, StrList* pchtxt,
                          StrList* patch_str_list) {
    FILE* pchtxt_file;
    pchtxt_file = fopen(pchtxt_target->patch_txt_path, "r");
    if (pchtxt_file == NULL) {
        fclose(pchtxt_file);
        printf(CONSOLE_ESC(31m) "Cannot open %s, " CONSOLE_ESC(m),
               pchtxt_target->patch_txt_path);
        return -1;
    }

    // line buffers
    char line[LINE_MAX_SIZE];
    line[LINE_MAX_SIZE - 1] = '\0';
    char last_comment[LINE_MAX_SIZE];
    last_comment[LINE_MAX_SIZE - 1] = '\0';

    bool stopped = false;
    u32 line_idx = 0;
    while (fgets(line, LINE_MAX_SIZE - 7, pchtxt_file) != NULL) {
        if (!stopped) {
            switch (line[0]) {
                case '/':
                    strcpy(last_comment, line);
                case '@':
                    switch (line[1]) {
                        case 's':
                        case 'S':
                            stopped = true;
                            break;
                        case 'e':
                        case 'E':
                            addPatchCommentToToggleList(patch_str_list,
                                                        last_comment, line_idx,
                                                        TOGGLE_ENABLED);
                            break;
                        case 'd':
                        case 'D':
                            addPatchCommentToToggleList(patch_str_list,
                                                        last_comment, line_idx,
                                                        TOGGLE_DISABLED);
                            break;
                    }
            }
        }
        addToStrList(pchtxt, line);
        line_idx++;
    }

    fclose(pchtxt_file);
    return 0;
}

int writePchtxtFromStrList(PatchTextTarget* pchtxt_target, StrList* pchtxt,
                           StrList* patch_str_list) {
    FILE* pchtxt_file;
    pchtxt_file = fopen(pchtxt_target->patch_txt_path, "wb");
    if (pchtxt_file == NULL) {
        fclose(pchtxt_file);
        printf(CONSOLE_ESC(31m) "Cannot open %s, " CONSOLE_ESC(m),
               pchtxt_target->patch_txt_path);
        return -1;
    }

    for (int i = 0; i < patch_str_list->size; i++) {
        u32 flag_idx = *(u32*)&patch_str_list->str_list[i][0xFA];
        switch (getStringTailU16(patch_str_list->str_list[i])) {
            case TOGGLE_ENABLED:
            case TOGGLE_ENABLED_CHANGED:
                strcpy(pchtxt->str_list[flag_idx], ENABLED_FLAG);
                break;
            case TOGGLE_DISABLED:
            case TOGGLE_DISABLED_CHANGED:
                strcpy(pchtxt->str_list[flag_idx], DISABLED_FLAG);
                break;
        }
    }

    for (int i = 0; i < pchtxt->size; i++) {
        fputs(pchtxt->str_list[i], pchtxt_file);
    }

    fclose(pchtxt_file);

    return 0;
}
