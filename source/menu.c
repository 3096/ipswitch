#include "menu.h"

void mainMenu() {
    StrList* main_menu_list = getStrList();
    addToStrList(main_menu_list, "Generate IPS by Patch Text");   //0
    addToStrList(main_menu_list, "Toggle Patch Text Contents");   //1
    //addToStrList(main_menu_list, "Toggle Existing IPS");        //2 (planned)
    //addToStrList(main_menu_list, "Create Patch Text with IPS"); //3 (planned)

    while(appletMainLoop())
    {
        int selection = 0;
        printf("\n%s\n", CONSOLE_ESC(1m)
            "Select an Operation or Press + to Quit:" CONSOLE_ESC(m));
        u64 kDown = selectFromList(&selection, main_menu_list);

        if (kDown & KEY_A) {
            switch(selection) {
            case 0:
                kDown = patchTextToIPSMenu();
                break;
            case 1:
                kDown = patchTextToggleMenu();
                break;
            default:
                break;
            }
        }

        if (kDown & KEY_PLUS)
            break;
    }

    freeStrList(main_menu_list);
}

u64 patchTextSelect(PatchTextTarget* pchtxt_target) {
    printf("\nReading contents from %s\n", IPSWITCH_DIR);

    StrList* pchtxt_list = getStrList();

    char tid_str[17]; tid_str[16] = '\0';

    // read IPSWITCH_DIR folders
    DIR* dir; struct dirent* dir_ent;
    dir = opendir(IPSWITCH_DIR);
    if(dir==NULL) {
        printf(CONSOLE_ESC(31m) "Failed to open %s.\n" CONSOLE_ESC(m),
            IPSWITCH_DIR);
    } else {
        while ((dir_ent = readdir(dir))) {
            if(dir_ent->d_type != DT_DIR) {
                continue;
            }

            strcpysize(tid_str, dir_ent->d_name, 16);

            if(strlen(tid_str) != 16 || !isValidHexStr(tid_str)) {
                tid_str[0] = '\0';
            }

            char subdir_path[0x100] = { 0 };
            strcpy(subdir_path, IPSWITCH_DIR);
            strcat(subdir_path, dir_ent->d_name);
            strcat(subdir_path, "/");

            // search patch text in folder
            DIR* subdir; struct dirent* subdir_ent;
            subdir = opendir(subdir_path);
            if(subdir==NULL) {
                printf(CONSOLE_ESC(31m) "Failed to open %s.\n" CONSOLE_ESC(m),
                    subdir_path);
            } else {
                while ((subdir_ent = readdir(subdir))) {
                    if(subdir_ent->d_type != DT_REG) {
                        continue;
                    }

                    char* ent_extention = strrchr(subdir_ent->d_name, '.');
                    if(strcmp(ent_extention, PATCH_TEXT_FORMAT) == 0) {
                        char pchtxt_path_buf[0x100] = { 0 };
                        strcpy(pchtxt_path_buf,
                            &subdir_path[strlen(IPSWITCH_DIR)]);
                        strcat(pchtxt_path_buf, subdir_ent->d_name);
                        addToStrList(pchtxt_list, pchtxt_path_buf);
                    }
                }
                closedir(subdir);
            }
        }
        closedir(dir);
    }

    u64 kDown = 0; // returns this later

    while(appletMainLoop())
    {
        int selection = 0;
        printf("\n%s\n", CONSOLE_ESC(1m)
            "Press: A to Select Patch Text | B to Go Back | PLUS(+) to quit:"
            CONSOLE_ESC(m));
        kDown = selectFromList(&selection, pchtxt_list);

        if (kDown & KEY_PLUS || kDown & KEY_B) {
            break;
        }

        if (kDown & KEY_A) {
            // patch_txt_path
            strcpy(pchtxt_target->patch_txt_path, IPSWITCH_DIR);
            strcat(pchtxt_target->patch_txt_path,
                pchtxt_list->str_list[selection]);
            // tid_str
            strcpysize(pchtxt_target->tid_str,
                pchtxt_list->str_list[selection], 16);
            // name
            getPatchTextName(pchtxt_target->name,
                pchtxt_target->patch_txt_path);
            // folder_name
            strcpy(pchtxt_target->folder_name,
                pchtxt_list->str_list[selection]);
            *strchr(pchtxt_target->folder_name, '/') = '\0';
            break;
        }
    }

    freeStrList(pchtxt_list);
    return kDown;
}

u64 patchTextToIPSMenu() {
    u64 kDown = 0;
    while(appletMainLoop()) {
        PatchTextTarget pchtxt_target;
        kDown = patchTextSelect(&pchtxt_target);

        if (kDown & KEY_PLUS || kDown & KEY_B)
            break;

        if (kDown & KEY_A) {
            int rc = 0;

            rc = patchTextToIPS(&pchtxt_target);

            if(R_SUCCEEDED(rc))
                printf(CONSOLE_ESC(32m) "\nAll Done\n" CONSOLE_ESC(m));
            else
                printf(CONSOLE_ESC(31m) "\nFailed: %d\n" CONSOLE_ESC(m), rc);
        }

    }
    return kDown;
}

u64 patchTextToggleMenu() {
    u64 kDown = 0;
    PatchTextTarget pchtxt_target;
    kDown = patchTextSelect(&pchtxt_target);

    if (kDown & KEY_A) {

        printf(CONSOLE_ESC(1m) 
            "Press: A to Toggle a Patch            | B to Save and Go Back\n"
            "       X to Abort and Discard Changes | "
            "Y to Save and Apply Changes to IPS\n"
            CONSOLE_ESC(0;32m) "\n\nGreen = Enabled "
            CONSOLE_ESC(31m) "Red = Disabled\n" CONSOLE_ESC(m));

        StrList* pchtxt = getStrList();
        StrList* patch_str_list = getStrList();

        Result rc = 0;
        rc = readPchtxtIntoStrList(&pchtxt_target, pchtxt, patch_str_list);
        if(!R_SUCCEEDED(rc)) {
            printf(CONSOLE_ESC(31m)
                "\nFailed to read patch text: %d\n" CONSOLE_ESC(m), rc);
            goto end;
        }
        
        int selection = 0; // dummy here
        kDown = selectFromList(&selection, patch_str_list);

        if (kDown & KEY_B || kDown & KEY_Y) {
            rc = writePchtxtFromStrList(&pchtxt_target,
                pchtxt, patch_str_list);

            if(R_SUCCEEDED(rc)) {
                printf(CONSOLE_ESC(32m) "Changes applied.\n" CONSOLE_ESC(m));
            } else {
                printf(CONSOLE_ESC(31m) "\nFailed: %d\n" CONSOLE_ESC(m), rc);
                goto end;
            }

            if(kDown & KEY_Y) {
                rc = patchTextToIPS(&pchtxt_target);

                if(R_SUCCEEDED(rc))
                    printf(CONSOLE_ESC(32m) "\nAll Done\n" CONSOLE_ESC(m));
                else
                    printf(CONSOLE_ESC(31m) "\nFailed: %d\n" CONSOLE_ESC(m), rc);
            }
        } else {
            printf(CONSOLE_ESC(33m) "Changes discarded.\n" CONSOLE_ESC(m));
        }
    end:
        freeStrList(pchtxt);
        freeStrList(patch_str_list);
    }
    return kDown;
}
