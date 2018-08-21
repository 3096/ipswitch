#include "menu.h"

void mainMenu() {
    StrList* main_menu_list = getStrList();
    addToStrList(main_menu_list, "Generate IPS by Patch Text");   //0
    //addToStrList(main_menu_list, "Toggle Patch Text Contents"); //1 (planned)
    //addToStrList(main_menu_list, "Toggle Existing IPS");        //2 (planned)
    //addToStrList(main_menu_list, "Create Patch Text with IPS"); //3 (planned)

    while(appletMainLoop())
    {
        int selection = 0;
        printf("\n%s\n", "Select an Operation or Press + to Quit:");
        u64 kDown = selectFromList(&selection, main_menu_list);

        if (kDown & KEY_A) {
            switch(selection) {
            case 0:
                kDown = patchTextSelect();
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

u64 patchTextSelect() {
    Result rc = 0;

    printf("Reading contents from %s\n", IPSWITCH_DIR);

    StrList* pchtxt_list = getStrList();

    char tid_str[17]; tid_str[16] = '\0';

    // read IPSWITCH_DIR folders
    DIR* dir; struct dirent* dir_ent;
    dir = opendir(IPSWITCH_DIR);
    if(dir==NULL) {
        printf("Failed to open %s.\n", IPSWITCH_DIR);
    } else {
        while ((dir_ent = readdir(dir))) {
            if(dir_ent->d_type != DT_DIR) {
                continue;
            }

            strcpysize(tid_str, dir_ent->d_name, 16);

            if(strlen(tid_str) != 16 || !isValidHexStr(tid_str)) {
                tid_str[0] = '\0';
            }

            char subdir_path[0x100];
            strcpy(subdir_path, IPSWITCH_DIR);
            strcat(subdir_path, dir_ent->d_name);
            strcat(subdir_path, "/");

            // search patch text in folder
            DIR* subdir; struct dirent* subdir_ent;
            subdir = opendir(subdir_path);
            if(subdir==NULL) {
                printf("Failed to open %s.\n", subdir_path);
            } else {
                while ((subdir_ent = readdir(subdir))) {
                    if(subdir_ent->d_type != DT_REG) {
                        continue;
                    }

                    char* ent_extention = strrchr(subdir_ent->d_name, '.');
                    if(strcmp(ent_extention, PATCH_TEXT_FORMAT) == 0) {
                        char pchtxt_path_buf[0x100];
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
        printf("\n%s\n",
            "Press: A to Select Patch Text | B to Go Back | PLUS(+) to quit:");
        kDown = selectFromList(&selection, pchtxt_list);
        if (kDown & KEY_PLUS || kDown & KEY_B) {
            break;
        }

        PatchTextTarget pchtxt_target;
        // tid_str
        strcpy(pchtxt_target.tid_str, tid_str);
        // patch_txt_path
        strcpy(pchtxt_target.patch_txt_path, IPSWITCH_DIR);
        strcat(pchtxt_target.patch_txt_path, pchtxt_list->str_list[selection]);
        // name
        getPatchTextName(pchtxt_target.name, pchtxt_target.patch_txt_path);
        // folder_name
        strcpy(pchtxt_target.folder_name, pchtxt_list->str_list[selection]);
        *strchr(pchtxt_target.folder_name, '/') = '\0';

        if (kDown & KEY_A) {
            PatchList* pchlist = initPatchList();
            pchlist->target = pchtxt_target;

            rc = parsePatchText(pchlist);
            if(!R_SUCCEEDED(rc)) {
                printf("\nFailed: %d\n", rc);
                continue;
            }
            
            rc = patchTarget(pchlist);

            free(pchlist);

            if(R_SUCCEEDED(rc))
                printf("\nAll Done\n");
            else
                printf("\nFailed: %d\n", rc);
        }
    }

    freeStrList(pchtxt_list);
    return kDown;
}


