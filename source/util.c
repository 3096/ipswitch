#include "util.h"

StrList* getStrList() {
    StrList* list_ptr = malloc(sizeof(StrList));
    list_ptr->size = 0;
    list_ptr->size_max = 0;
    list_ptr->str_list = NULL;
    list_ptr->str_list_ptr = NULL;
    return list_ptr;
}

void addToStrList(StrList* list_ptr, const char* str) {
    if ( list_ptr->size >= list_ptr->size_max ){
        list_ptr->size_max = list_ptr->size + 4;
        list_ptr->str_list = realloc(list_ptr->str_list, list_ptr->size_max * sizeof(String));
        list_ptr->str_list_ptr = realloc(list_ptr->str_list_ptr, list_ptr->size_max * sizeof(char*));
    }
    strcpy((char*)&list_ptr->str_list[list_ptr->size], str);
    list_ptr->str_list_ptr[list_ptr->size] = list_ptr->str_list[list_ptr->size];
    list_ptr->size++;
}

void freeStrList(StrList* list_ptr) {
    free(list_ptr->str_list_ptr);
    free(list_ptr->str_list);
    free(list_ptr);
}

int isValidHexStr(const char* str)
    { return strlen(str) == strspn(str, VALID_HEX_CHAR); }

int strcpysize(char* dest, const char* src, size_t size) {
    size_t i = 0;
    while(i < size && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return i;
}

int isDirectory(const char* path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return 0;
    return S_ISDIR(statbuf.st_mode);
}

int strEndianSwap(char* str) {
    size_t size = strlen(str);
    if(size%2 != 0)
        return -1;
    char * buffer = malloc(size);

    for(int i = size-2; i >= 0; i-=2) {
        buffer[size-i-2] = str[i];
        buffer[size-i-1] = str[i+1];
    }

    memcpy(str, buffer, size);
    free(buffer);
    return 0;
}

void selectIndex(int* selection, char* list[], int size, int change) {
    *selection += change;

    if (*selection < 0) {
        *selection = abs(*selection) % size;
        *selection = size - *selection;
    }
    if ((u32)*selection >= size) {
        *selection %= size;
    }
    printf("\r                                                               "
        "                ");
    printf("\rUsing: %s", list[*selection]);
}

u64 selectFromList(int* selection, char* list[], int size) {
    if(size <= 0) {
        if(userConfirm("Error: list is empty"))
            return 0;
        else
            return KEY_PLUS;
    }

    selectIndex(selection, list, size, 0);

    u64 kDownPrevious = hidKeysDown(CONTROLLER_P1_AUTO);
    while(appletMainLoop()) {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_UP)
            selectIndex(selection, list, size, -1);
        else if (kDown & KEY_DOWN)
            selectIndex(selection, list, size, 1);
        else if (kDown & KEY_LEFT)
            selectIndex(selection, list, size, -5);
        else if (kDown & KEY_RIGHT)
            selectIndex(selection, list, size, 5);
        else if(kDown > kDownPrevious) {
            printf("\n");
            return kDown;
        }
        kDownPrevious = kDown;

        gfxFlushBuffers();
        gfxSwapBuffers();
        gfxWaitForVsync();
    }

    return 0;
}

bool userConfirm(const char * msg) {
    printf("\n%s\nPress A to confirm, any other button to cancel\n", msg);

    u64 kDownPrevious = hidKeysDown(CONTROLLER_P1_AUTO);
    while(appletMainLoop())
    {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if(kDown > kDownPrevious) {
            if (kDown & KEY_A)
                return true;
            else {
                printf("Canceled\n");
                return false;
            }
        }

        kDownPrevious = kDown;
    }
    return false;
}

void printInProgress(const char * msg) {
    printf("%s... ", msg);
}

void printDone() {
    printf("Done\n");
}
