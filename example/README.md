# How to use

This program reads `.pchtxt` (patch text) files and can create `.ips` patches or create a patched `.nso` from a `.elf`. `.pchtxt` is a in-house text file format to represent patches. You can find the specs for `.pchtxt` below.

The program looks for `.pchtxt` files from directory `sdmc:/switch/ipswitch/{titleid-title_description}/`. "title_description" is optional but it is used for display and help user to identify the title.

For generating ips patch, name your patch text as `sdmc:/switch/ipswitch/{titleid-title_description}/ips-{nso_name}.pchtxt`. NSO build id is required in the file (see "Structure of `.pchtxt` File" section.)

For generating patched nso from elf, place your elf file in `sdmc:/switch/ipswitch/{titleid-title_description}/{nso_name}.elf`. {nso_name} mush match the file name of the nso inside exefs. The `.pchtxt` should be `sdmc:/switch/ipswitch/{titleid-title_description}/{nso_name}.pchtxt`. 

Run the homebrew and it will let you select a `.pchtxt` and generate either a `.ips` patch to `sdmc:/atmosphere/exefs_patches/{titleid}/{nsoid}.ips` or a patched nso to `sdmc:/atmosphere/titles/{titleid}/exefs/{nso_name}`.

---
## Structure of `.pchtxt` File
`.pchtxt` will be a text file. See the ips-main.pchtxt for some simple Splatoon 2 3.1.0 edit I provided for example.

---
The first line is mandatory to specify the value's Endianess with character `@`. It should either be `@little-endian` or `@big-endian`.

---
The second line is mandatory to specify the nso's build id with `@nsobid-{a_buncha_hex}`. If this is missing, the program will attempt to treat the file as an elf to nso patch.

---
The patches are represented in lines of offset and value pair, each are 4 bytes. 

Offset will always be in Big-Endian, and is the absolute offset from uncompressed nso file (or your elf file). The value can be either Big-Endian or Little-Endian, but it must be specified in the first line.

For every patch, it is necessary to specify whether it is enabled or disabled by putting `@enabled` or `@disabled` flag on top of it. When encountering this flag, the program will print the last "pure comment line" from the file, in attempt to explain the patch. Thus, it's also necessary to put a comment before each patch about what the patch does.

Comments are marked with `/` character:

```// This is a comment.```

To sum it up, this is what a patch should look like:

```
// Some comment to explain the patch
@enabled
00123458 1F2003D5
0012345C DEADBEEF
...      ...
^        ^
|        |
Offset   Some Value
```

---
`@stop` can be used in the end of valid patches, telling the program to stop reading any patches that might come after this flag, in case you want to leave in any deprecated patches just for your own records.

---
Lines starting with `#` will always be printed when the program parses the text file, so it can be used to provide extra information, like:

```#01003BC0000A0000 Splatoon™ 2 3.0.1 USA```

---
### Editing the text file
Please don't use Windows Notepad. 99% chance you'll end up with some bad encoding that will cause this to not work. Use a real text editor like Sublime, Atom, or Notepad++. Heck, use a hex editor, if you will, to make sure no extra bytes are there to break things.
