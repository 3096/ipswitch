#include "patch.h"

int patchTarget(PatchTarget target) {
    size_t elf_len;
    uint8_t* elf = ReadEntireFile(target.elf_dir, &elf_len);

    elf2nso(elf, elf_len, ATMOS_TITLE_DIR);

	return 0;
}
