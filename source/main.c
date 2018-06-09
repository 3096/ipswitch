#include <string.h>
#include <stdio.h>
#include <dirent.h>

#include <switch.h>

#include "elf2nso.h"
#include "util.h"
#include "patch.h"

const char * TITLE_DIR = "/switch/swelfpatch/";

int main(int argc, char **argv)
{
    gfxInitDefault();
    consoleInit(NULL);
    Result rc = 0;

    printf("Welcome to SwElfPatch. Do no evil.\n\n");

    printf("Reading contents from %s\n", TITLE_DIR);

    DIR* dir;
    struct dirent* ent;

    typedef char String[0x100];
    String* tid_list = NULL;
    char** tid_list_ptr = NULL;
    int list_count = 0;
    int list_max = 0;

    char tid_str[17];
    tid_str[16] = '\0';

    dir = opendir(TITLE_DIR);
    if(dir==NULL)
    {
        printf("Failed to open dir.\n");
    }
    else
    {
        while ((ent = readdir(dir)))
        {
            if (ent->d_type != DT_DIR)
                continue;

            memcpy(tid_str, ent->d_name, 16);
            if (!isValidHexStr(tid_str))
                continue;

            if ( list_count >= list_max ){
                list_max = list_count + 4;
                tid_list = realloc(tid_list, list_max * sizeof(String));
                tid_list_ptr = realloc(tid_list_ptr, list_max * sizeof(char*));
            }
            strcpy((char*)&tid_list[list_count], ent->d_name);
            tid_list_ptr[list_count] = tid_list[list_count];
            list_count++;
        }
        closedir(dir);
    }

    // Main loop
    while(appletMainLoop())
    {
        int selection;
        printf("\n%s\n", "Select Title or Press + to quit:");
        rc = selectFromList(&selection, tid_list_ptr, list_count);
        if (rc == 1)
            break;

        printf("Patching elf content...\n");

        PatchTarget target;
        memcpy(target.tid_str, tid_list[selection], 16);

        strcpy(target.patch_dir, TITLE_DIR);
        strcat(target.patch_dir, tid_list[selection]);

        strcpy(target.elf_dir, target.patch_dir);

        strcat(target.patch_dir, "/main.swelfpatch");
        strcat(target.elf_dir, "/main.elf");

        rc = patchTarget(target);
        if(R_SUCCEEDED(rc))
            printf("Done\n");
        else
            printf("Failed: %d\n", rc);
    }

    gfxExit();
    return 0;
}
