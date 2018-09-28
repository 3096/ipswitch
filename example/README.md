# How to use

This program reads `.pchtxt` (patch text) files and can create `.ips` patches. `.pchtxt` is a in-house text file format to represent patches. You can find the specs for `.pchtxt` below.

The program looks for `.pchtxt` files with the path `sdmc:/switch/ipswitch/{patch_description}/{whatever_name}.pchtxt`. The dir {patch_description}/{whatever_name}.pchtxt is used for display purpose and help user to identify the patch. The {patch_description} folder name is used as the folder name in `atmosphere/exefs_patches/{patch_description}`.

Run the homebrew and it will let you select a `.pchtxt` file to generate `.ips` patch with.

---
## Structure of `.pchtxt` File
`.pchtxt` will be a text file. See the `splat310.pchtxt` for some simple Splatoon 2 3.1.0 edit I provided for example.

---
### Mandatory Lines
The first line is mandatory to specify the nso's build id with `@nsobid-{a_buncha_hex}`. If this is missing, the program will attempt to treat the file as an elf to nso patch, see "Using `.elf` Files and Generate a Patched NSO" section.

---
### Patches
The patches are represented in lines of offset and value pair. The offset is always 4 bytes, while value can be either any amount of bytes or a string.

Offset will always be in Big-Endian, and is the absolute offset from uncompressed nso file. However, in most cases, it's easier to use the "offset_shift" flag to match the .text offsets, see "Flags" section for details.

The value can be either bytes or a string.

When using bytes, each byte must be represented in 2 characters in their hexadecimal representable, and multiple bytes should be arranged in the order they appear in the binary (Little Endian in most cases). 

When using Strings, each string must open and close with a pair of `"`. C style string escapes are also supported. So a string value can be something like `"New\nLine"`.

For every patch, it is necessary to specify whether it is enabled or disabled by putting `@enabled` or `@disabled` flag on top of it. When encountering this flag, the program will print the last "pure comment line" from the file, in attempt to explain the patch. Thus, it's also necessary to put a comment before each patch about what the patch does.

Comments are marked with `/` character:

```// This is a comment.```

Be mindful that IPSwitch uses a 0x100 sized line buffer when parsing patch text. While this shouldn't be an issue in most cases, having a line over 255 characters will not work.

To sum it up, this is what a patch should look like:

```
// Some comment to explain the patch
@enabled
00123458 1F2003D5DEADBEEF
00123460 "a cool string"
...      ...
^        ^
|        |
Offset   Some Value or String
```

---
### Flags
`@flag` can be used to add flags in the patch text. This is what the line should look like:

`@flag {flag_name}` 

Or for flags with an value:

`@flag {flag_name} {flag_value}`

For now, the supported flags are

---
`offset_shift`: allows you to add or substract a certain amount to all patch text offsets that comes after this flag. For example, if you want to use .text addresses, you can do

```@flag offset_shift 0x100```

Which will automatically add 0x100 to all offsets, since the raw offset from IDA doesn't include the 0x100 nso header.

---
`print_values`: to enable printing applied values of all patches

```@flag print_values```

---
See `splat310.pchtxt` to find out how you can use the flag in it.

---
### Ending the Patch Text Early
`@stop` can be used in the end of desired patches, telling the program to stop reading any patches that might come after this flag, in case you want to leave in any not in use patches without having to write all of them as disabled.

---
### # Symbol
Lines starting with `#` will always be printed when the program parses the text file, so it can be used to provide extra information, like:

```#01003BC0000A0000 Splatoon™ 2 3.0.1 USA```

---
### Using `.elf` Files and Generate a Patched NSO
IPSwitch has a legacy support for elf to nso patching that dates back to before Atmosphere loader's `.ips` patch support. Since there has been previous reports of Atmosphere loader skipping `.ips` patches when custom nso is present, this feature can be used when there's need for a completely different nso.

In fact, IPSwitch will attempt to treat any patch text without a specified `@nsobid-` as an elf to nso patch. However, generating the right path for custom nso files requires in addition: 1. the title id, and 2. the nso name.

For generating patched nso from elf, place your elf file in `sdmc:/switch/ipswitch/{titleid-title_description}/{nso_name}.elf`. {nso_name} mush match the file name of the nso inside exefs. The `.pchtxt` should be `sdmc:/switch/ipswitch/{titleid-title_description}/{nso_name}.pchtxt`.

Be careful when using the "offset_shift" flag when using this, as the converted elf file has different offset shifts in different segments.

See `main.pchtxt` as an example.

---
### Editing the Text File
Please don't use Windows Notepad, because chances are that you'll end up with some bad encoding that may cause unwanted behavior when parsing. Use a real text editor like Sublime, Atom, or Notepad++. Heck, use a hex editor, if you will, to make sure no extra bytes are there to break things.
