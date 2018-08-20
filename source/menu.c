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

            if(!isValidHexStr(tid_str)) {
                continue;
            }

            addToStrList(title_list, ent->d_name);
        }
        closedir(dir);
    }

    while(appletMainLoop())
    {
        int selection = 0;
        printf("\n%s\n", "Select Title or Press + to quit:");
        u64 kDown = selectFromList(&selection, title_list);
        if (kDown & KEY_PLUS)
            break;

        PatchTextTarget pchtxt_target;
        
        memcpy(pchtxt_target.tid_str, title_list->str_list[selection], 16);
        pchtxt_target.tid_str[16] = '\0';

        strcpy(pchtxt_target.patch_txt_dir, IPSWITCH_DIR);
        strcat(pchtxt_target.patch_txt_dir, title_list->str_list[selection]);
        strcat(pchtxt_target.patch_txt_dir, "/");

        strcpy(pchtxt_target.nso_name, "ips-main");

        if (kDown & KEY_A) {
            PatchList* pchlist = initPatchList();
            pchlist->target = pchtxt_target;

            // TODO: check rc
            rc = parsePatchText(pchlist);
            
            rc = patchTarget(pchlist);

            free(pchlist);

            if(R_SUCCEEDED(rc))
                printf("\nAll Done\n");
            else
                printf("\nFailed: %d\n", rc);
        }

        if (kDown & KEY_X) {
            printf("Deleting ips...\n");
            char ips_dir[0x100];
            strcpy(ips_dir, ATMOS_EXEPCH_DIR);
            strcat(ips_dir, pchtxt_target.tid_str);
            remove(ips_dir);
            printf("%s deleted.\n", ips_dir);
        }
    }

    freeStrList(title_list);
}


