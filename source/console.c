#include "console.h"

u16 getStringTailU16(char* str) {
    return *(u16*)&str[0xFE];
}

void setStringTailU16(char* str, u16 val) {
    *(u16*)&str[0xFE] = val;
}

void catColoredStrByTail(char* out, char* in) {
    if(strlen(in) >= CONSOLE_WIDTH) {
        strcpy(&in[CONSOLE_WIDTH-4], EXCEED_SYMBOL);
    }
    u16 str_tail = getStringTailU16(in);
    switch(str_tail) {
    case TOGGLE_ENABLED_CHANGED:
        setStringTailU16(in, TOGGLE_ENABLED);
    case TOGGLE_ENABLED:
        strcat(out, CONSOLE_ESC(32m));
        strcat(out, in);
        break;
    case TOGGLE_DISABLED_CHANGED:
        setStringTailU16(in, TOGGLE_DISABLED);
    case TOGGLE_DISABLED:
        strcat(out, CONSOLE_ESC(31m));
        strcat(out, in);
        break;
    default:
        strcat(out, in);
    }
    strcat(out, CONSOLE_ESC(m));
}

void selectIndex(int* selection, StrList* str_list, int change) {
    int selection_priv = *selection;
    *selection += change;

    if (*selection < 0) {
        *selection = abs(*selection) % str_list->size;
        *selection = str_list->size - *selection;
    }
    if ((u32)*selection >= str_list->size) {
        *selection %= str_list->size;
    }

    const size_t MENU_PAGE_SIZE_MAX = 20;
    char menu_buffer[0x1000]; menu_buffer[0] = '\0';

    size_t menu_page_size;
    if(str_list->size > MENU_PAGE_SIZE_MAX) {
        menu_page_size = MENU_PAGE_SIZE_MAX;
    } else {
        menu_page_size = str_list->size;
    }

    bool page_changed = 
        *selection/menu_page_size != selection_priv/menu_page_size;
    u16 cur_str_tail = getStringTailU16(str_list->str_list[*selection]);
    bool toggle_changed =
        cur_str_tail == TOGGLE_ENABLED_CHANGED || 
        cur_str_tail == TOGGLE_DISABLED_CHANGED;

    // assume first print is always change == 0, and otherwise re-print
    if((change != 0 && !page_changed) || toggle_changed) {
        size_t move_up = menu_page_size - selection_priv % menu_page_size + 1;
        for(int i = 0; i < move_up; i++)
            strcat(menu_buffer, CONSOLE_ESC(1A));
        strcat(menu_buffer, "\r");
        catColoredStrByTail(menu_buffer, str_list->str_list[selection_priv]);
        for(int i = 0; i < move_up; i++)
            strcat(menu_buffer, CONSOLE_ESC(1B));

        move_up = menu_page_size - *selection % menu_page_size + 1;
        for(int i = 0; i < move_up; i++)
            strcat(menu_buffer, CONSOLE_ESC(1A));
        strcat(menu_buffer, "\r" CONSOLE_ESC(7m));
        catColoredStrByTail(menu_buffer, str_list->str_list[*selection]);
        strcat(menu_buffer, CONSOLE_ESC(0m));
        for(int i = 0; i < move_up; i++)
            strcat(menu_buffer, CONSOLE_ESC(1B));
    } else {
        if(page_changed) {
            size_t move_up = menu_page_size + 2;
            for(int i = 0; i < move_up; i++)
                strcat(menu_buffer, CONSOLE_ESC(1A));
            strcat(menu_buffer, "\r" CONSOLE_ESC(0J));
        }

        // determine menu start and end
        size_t menu_start = *selection/menu_page_size * menu_page_size;
        size_t menu_end = (*selection/menu_page_size + 1) * menu_page_size;

        // catting menu
        if(menu_start > 0) {
            strcat(menu_buffer, "...\n");
        } else {
            strcat(menu_buffer, "\n");
        }

        for(int i = menu_start; i < *selection; i++) {
            catColoredStrByTail(menu_buffer, str_list->str_list[i]);
            strcat(menu_buffer, "\n");
        }

        strcat(menu_buffer, CONSOLE_ESC(7m));
        catColoredStrByTail(menu_buffer, str_list->str_list[*selection]);
        strcat(menu_buffer, CONSOLE_ESC(0m));
        strcat(menu_buffer, "\n");

        for(int i = *selection+1; i < menu_end; i++) {
            if(i < str_list->size)
                catColoredStrByTail(menu_buffer, str_list->str_list[i]);
            strcat(menu_buffer, "\n");
        }
        if(menu_end < str_list->size) {
            strcat(menu_buffer, "...\n");
        } else {
            strcat(menu_buffer, "\n");
        }
    }

    printf(menu_buffer);
}

u64 selectFromList(int* selection, StrList* str_list) {
    if(str_list->size <= 0) {
        if(userConfirm("Error: list is empty"))
            return 0;
        else
            return KEY_PLUS;
    }

    selectIndex(selection, str_list, 0);

    u64 kDownPrevious = 0xFFFFFFFFFFFFFFFF;
    while(appletMainLoop()) {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        kDown = kDown&(0xFFFFFFFFFFFFFFFF-KEY_TOUCH); // "can't touch this"

        if (kDown & KEY_UP)
            selectIndex(selection, str_list, -1);
        else if (kDown & KEY_DOWN)
            selectIndex(selection, str_list, 1);
        else if (kDown & KEY_LEFT)
            selectIndex(selection, str_list, -5);
        else if (kDown & KEY_RIGHT)
            selectIndex(selection, str_list, 5);
        else if (kDown & KEY_A) {
            char* cur_str = str_list->str_list[*selection];
            u16 str_tail = getStringTailU16(cur_str);
            if(str_tail == TOGGLE_ENABLED) {
                setStringTailU16(cur_str, TOGGLE_DISABLED_CHANGED);
                selectIndex(selection, str_list, 0);
            } else if (str_tail == TOGGLE_DISABLED) {
                setStringTailU16(cur_str, TOGGLE_ENABLED_CHANGED);
                selectIndex(selection, str_list, 0);
            } else {
                printf("\n");
                return kDown;
            }
        }
        else if(kDown > kDownPrevious) {
            printf("\n");
            return kDown;
        }
        kDownPrevious = kDown;
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
    printf(CONSOLE_ESC(32m) "Done\n" CONSOLE_ESC(m));
}

void printBytesAsHex(const u8* bytes, size_t size) {
    for(size_t i = 0; i < size; i++) {
        printf("%02X", bytes[i]);
    }
}
