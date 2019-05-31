#include "util.h"

StrList* getStrList() {
    StrList* list_ptr = malloc(sizeof(StrList));
    list_ptr->size = 0;
    list_ptr->size_max = 0;
    list_ptr->str_list = NULL;
    return list_ptr;
}

void addToStrList(StrList* list_ptr, const char* str) {
    if (list_ptr->size >= list_ptr->size_max) {
        list_ptr->size_max = list_ptr->size + 4;
        list_ptr->str_list =
            realloc(list_ptr->str_list, list_ptr->size_max * sizeof(String));
    }
    strcpy((char*)&list_ptr->str_list[list_ptr->size], str);
    list_ptr->size++;
}

void addBytesToStrList(StrList* list_ptr, const char* bytes) {
    if (list_ptr->size >= list_ptr->size_max) {
        list_ptr->size_max = list_ptr->size + 4;
        list_ptr->str_list =
            realloc(list_ptr->str_list, list_ptr->size_max * sizeof(String));
    }
    memcpy((char*)&list_ptr->str_list[list_ptr->size], bytes, sizeof(String));
    list_ptr->size++;
}

void freeStrList(StrList* list_ptr) {
    free(list_ptr->str_list);
    free(list_ptr);
}

int isValidHexStr(const char* str) {
    return strlen(str) == strspn(str, VALID_HEX_CHAR);
}

int strToLowerCase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
    return 0;
}

int strcpysize(char* dest, const char* src, size_t size) {
    size_t i = 0;
    while (i < size && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return i;
}

int isDirectory(const char* path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0) return 0;
    return S_ISDIR(statbuf.st_mode);
}

int bytesEndianSwap(u8* bytes, size_t size) {
    char* buffer = malloc(size);

    for (int i = 0; i < size; i++) {
        buffer[size - 1 - i] = bytes[i];
    }
    memcpy(bytes, buffer, size);

    free(buffer);
    return 0;
}

int strEndianSwap(char* str) {
    size_t size = strlen(str);
    if (size % 2 != 0) return -1;
    char* buffer = malloc(size);

    for (int i = size - 2; i >= 0; i -= 2) {
        buffer[size - i - 2] = str[i];
        buffer[size - i - 1] = str[i + 1];
    }
    memcpy(str, buffer, size);

    free(buffer);
    return 0;
}

int escapeString(char* str, size_t* new_len) {
    char* cur_char = str;
    char* head_char = str;
    *new_len = 0;

    while (*head_char) {
        if (*head_char == '\\') {
            head_char++;

            switch (*head_char) {
                case 'a':
                    *cur_char = '\a';
                    break;
                case 'b':
                    *cur_char = '\b';
                    break;
                case 'f':
                    *cur_char = '\f';
                    break;
                case 'n':
                    *cur_char = '\n';
                    break;
                case 'r':
                    *cur_char = '\r';
                    break;
                case 't':
                    *cur_char = '\t';
                    break;
                case 'v':
                    *cur_char = '\v';
                    break;
                default:
                    *cur_char = *head_char;
            }
        } else {
            *cur_char = *head_char;
        }
        head_char++;
        cur_char++;
        (*new_len)++;
    }

    *cur_char = '\0';
    return 0;
}
