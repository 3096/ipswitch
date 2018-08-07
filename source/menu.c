#include "menu.h"

void titleSelect() {
    Result rc = 0;

    printf("Reading contents from %s\n", IPSWITCH_DIR);
    StrList* title_list = getStrList();

    char tid_str[17];
    tid_str[16] = '\0';

    DIR* dir;
    struct dirent* ent;
    dir = opendir(IPSWITCH_DIR);
    if(dir==NULL) {
        printf("Failed to open dir.\n");
    } else {
        while ((ent = readdir(dir))) {
            if(ent->d_type != DT_DIR) {
                continue;
            }
            if(strlen(ent->d_name) < 16) {
                continue;
            }
            memcpy(tid_str, ent->d_name, 16);
            printf("tid: %s\n", tid_str);
            if(!isValidHexStr(tid_str)) {
                printf("!isValidHexStr: %s\n", tid_str);
                continue;
            }

            addToStrList(title_list, ent->d_name);
        }
        closedir(dir);
    }

    while(appletMainLoop())
    {
        int selection;
        printf("\n%s\n", "Select Title or Press + to quit:");
        u64 kDown = selectFromList(&selection, title_list->str_list_ptr, title_list->size);
        if (kDown & KEY_PLUS)
            break;

        // I'll clean up this part later ofc
        PatchTarget target;

        memcpy(target.tid_str, title_list->str_list[selection], 16);
        target.tid_str[16] = '\0';

        strcpy(target.patch_dir, IPSWITCH_DIR);
        strcat(target.patch_dir, title_list->str_list[selection]);

        strcpy(target.elf_dir, target.patch_dir);

        strcat(target.patch_dir, "/main.swelfpatch");
        strcat(target.elf_dir, "/main.elf");

        PatchTextTarget pchtxt_target;
        
        memcpy(pchtxt_target.tid_str, title_list->str_list[selection], 16);
        pchtxt_target.tid_str[16] = '\0';

        strcpy(pchtxt_target.patch_txt_dir, IPSWITCH_DIR);
        strcat(pchtxt_target.patch_txt_dir, title_list->str_list[selection]);
        strcat(pchtxt_target.patch_txt_dir, "/");

        strcpy(pchtxt_target.nso_name, "ips");

        if (kDown & KEY_A) {
            printf("Patching elf content...\n");

            rc = patchTarget(target);

            if(R_SUCCEEDED(rc))
                printf("Done\n\nAll Done\n");
            else
                printf("Failed: %d\n", rc);
        }

        if (kDown & KEY_Y) {
            printf("Testing parsePatchText...\n");

            rc = parsePatchText(&pchtxt_target);

            if(R_SUCCEEDED(rc))
                printf("Done\n\nAll Done\n");
            else
                printf("Failed: %d\n", rc);
        }

        if (kDown & KEY_X) {
            printf("Deleting patched nso...\n");
            char nso_dir[0x100];
            strcpy(nso_dir, ATMOS_TITLE_DIR);
            strcat(nso_dir, target.tid_str);
            strcat(nso_dir, "/exefs");
            strcat(nso_dir, "/main");
            remove(nso_dir);
        }
    }

    freeStrList(title_list);
}


