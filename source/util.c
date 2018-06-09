#include "util.h"

int isValidHexStr(const char* str)
    { return strlen(str) == strspn(str, VALID_HEX_CHAR); }

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

int selectFromList(int* selection, char* list[], int size) {
    if(size <= 0) {
        if(userConfirm("Error: list is empty"))
            return -1;
        else
            return 1;
    }

    selectIndex(selection, list, size, 0);

    while(appletMainLoop()) {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_UP)
            selectIndex(selection, list, size, -1);

        if (kDown & KEY_DOWN)
            selectIndex(selection, list, size, 1);

        if (kDown & KEY_LEFT)
            selectIndex(selection, list, size, -5);

        if (kDown & KEY_RIGHT)
            selectIndex(selection, list, size, 5);

        if (kDown & KEY_A) {
            printf("\n");
            return 0;
        }

        if (kDown & KEY_PLUS) {
            printf("\n");
            return 1;
        }

        gfxFlushBuffers();
        gfxSwapBuffers();
        gfxWaitForVsync();
    }

    return -1;
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
