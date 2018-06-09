# How to use
Place your elf file in `sdmc:/switch/swelfpatch/{titleid-title_description}/main.elf`. (For now the code only reads main.elf) "title_description" is optional as it is only used for display and easier to identify. The patches will be represented is a text file of swelfpatch format, `main.swelfpatch`. Run the homebrew and it will let you select and title and generate its patched nso to `sdmc:/atmosphere/titles/{titleid}/exefs/main`.

## Structure of `.swelfpatch` file
`.swelfpatch` will be a text file. See the main.swelfpatch for some simple Splatoon 2 3.0.1 edit I provided for example.

---
The first line is mandatory to specify the value's Endianess with character `@`. It should either be `@little-endian` or `@big-endian`.

---
The second line can be used to provide optional info about these patches preceded by a `#` symbol. The first 16 characters is the title id, and followed by whatever description necessary.

```#01003BC0000A0000 Splatoon™ 2 3.0.1 USA```

---
Comments are marked with '/' character:

```// This is a comment.```

---
For every patch, it is necessary to specify whether it is in use by putting `@enabled` or `@disabled` on top of it. The program will also print the last line of comment that it sees to explain the patch.

The actual edits are represented in a line of offset and value pair, each are a word (consecutive 4 bytes). Offset will always be in Big-Endian. The value will be in like specified in the beginning, either Big-Endian or Little-Endian.

```
00123456 1F2003D5
^        ^
|        |
Offset   Value
```
