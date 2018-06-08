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

    printf("Welcome to SwElfPatch. Do no evil.\n\n");


    DIR* dir;
    struct dirent* ent;

    typedef char String[0x100];
    String* tid_list = NULL;
    char** tid_list_ptr = NULL;
    int list_count = 0;
    int list_max = 0;

    dir = opendir(TITLE_DIR);
    if(dir==NULL)
    {
        printf("Failed to open dir.\n");
    }
    else
    {
        printf("Using '%s':\n", TITLE_DIR);
        while ((ent = readdir(dir)))
        {
            if (ent->d_type != DT_DIR)
                continue;

            char tid_str[17];
            tid_str[16] = '\0';
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
        int ret = 0;
        printf("%s\n", "Select Title:");
        ret = selectFromList(&selection, tid_list_ptr, list_count);
        if (ret == 1)
            break;
        // TODO implement file read and patching
    }

    gfxExit();
    return 0;
}
