# How to use

This program reads `.pchtxt` (patch text) files and can create `.ips` patches or create a patched `.nso` from a `.elf`. `.pchtxt` is a in-house text file format to represent patches. You can find the specs for `.pchtxt` below.

The program looks for `.pchtxt` files from directory `sdmc:/switch/ipswitch/{titleid-title_description}/`. "title_description" is optional but it is used for display and help user to identify the title. 

For generating ips patch, name your patch text as `sdmc:/switch/ipswitch/{titleid-title_description}/ips.pchtxt`. NSO build id is required in the file (see "Structure of `.pchtxt` File" section.)

For generating patched nso from elf, place your elf file in `sdmc:/switch/ipswitch/{titleid-title_description}/{nso_name}.elf`. {nso_name} mush match the file name of the nso inside exefs. The `.pchtxt` should be `sdmc:/switch/ipswitch/{titleid-title_description}/{nso_name}.pchtxt`. 

Run the homebrew and it will let you select a `.pchtxt` and generate either a `.ips` patch to `sdmc:/atmosphere/exefs_patches/{titleid}/{nsoid}.ips` or a patched nso to `sdmc:/atmosphere/titles/{titleid}/exefs/{nso_name}`.

---
## Structure of `.pchtxt` File
`.pchtxt` will be a text file. See the main.pchtxt for some simple Splatoon 2 3.0.1 edit I provided for example.

---
The first line is mandatory to specify the value's Endianess with character `@`. It should either be `@little-endian` or `@big-endian`.

---
The second line is mandatory to specify the nso's build id with `@nsobid-{a_buncha_hex}`. If this is missing, the program will attempt to treat the file as an elf to nso patch.

---
The third line can be used to provide optional info about these patches preceded by a `#` symbol. The first 16 characters is the title id, and followed by whatever description necessary.

```#01003BC0000A0000 Splatoon™ 2 3.0.1 USA```

---
Comments are marked with '/' character:

```// This is a comment.```

---
For every patch, it is necessary to specify whether it is enabled or disabled by putting `@enabled` or `@disabled` on top of it. When encountering this flag, the program will print the last line of pure comment that it sees to explain the patch, so it's also necessary to comment each patch.

`@stop` can be used in the end of valid patches, telling the program to stop reading any patches that might come after this specifier, useful for leaving in deprecated patches for records.

The actual edits are represented in a line of offset and value pair, each are 4 bytes. Offset will always be in Big-Endian, and it's the absolute offset from either uncompressed nso file or elf file. The value will be either Big-Endian or Little-Endian like specified in the first line.

This is what a patch should look like:

```
// Some comment to explain the patch
@enabled
00123456 1F2003D5
^        ^
|        |
Offset   Value
```

---
### Editing the text file
Please don't use Windows Notepad. 99% chance you'll end up with some bad encoding that will cause this to not work. Use a real text editor like Sublime or Notepad++. Heck, use a hex editor, if you will, to make sure no extra bytes are there to break things.
