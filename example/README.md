# How to use IPSwitch and Patch Text (`.pchtxt` Files)

## Brief
This program reads `.pchtxt` (patch text) files and use them to create `.ips` patches, or AMS cheat txt files. `.pchtxt` is a custom text file format designed to conveniently share and manage patches/cheats. You can find the specs for `.pchtxt` below.

The program looks for `.pchtxt` files at the following paths:

- `sdmc:/switch/ipswitch/{patch_description}.pchtxt`
- `sdmc:/switch/ipswitch/{patch}/{description}.pchtxt`

You can name your file or folder whatever you want. The `{patch description}` is used as the folder name in `atmosphere/exefs_patches/{patch description}`.

Run the homebrew and it will let you select a `.pchtxt` file to use.

## Step by Step Guide
Coming soon™

---
# How to Create your own Patch Text (Specs of `.pchtxt` Files)
`.pchtxt` files are plain text. In this demonstration, I will use `splat310.pchtxt` as an example. It provides some simple patches for Splatoon 2 version 3.1.0. Here is a snippet from it to show what a `.pchtxt` may look like:

```
@title Splatoon™ 2 3.1.0 USA
@program 01003BC0000A0000
@url "https://raw.githubusercontent.com/3096/ipswitch/master/example/splat310.pchtxt"

// This is a comment

@flag offset_shift 0x100  // <- This flag auto adds 0x100 to all offsets in the text
// This is to ignore 0x100-sized NSO headers

@flag nsobid 034C8FA7A63B7A87F96F408B2AEFFF6C  // <- a build id is required before parsing any patches

// patches can start after a build id is specified

// This is a patch ↓
// Force money to 6969699 [dualie]
@enabled
0019784C     400DA052602C8B72
// ^ offset  ^ value

@flag offset_shift 0 // <- You can flag offset_shift back to 0 if you want

// String swap shooting range
@enabled
03713CDC "Fld_Shakeup00_Cop"  // <- this is example of a string patch

// AMS cheats can also be used without modification
[Force sea snail to 696]
04000000 00197898 52805700
```

You can also find [`splat310.pchtxt`](splat310.pchtxt) as a file in `example`.

---
## Meta Info of `.pchtxt`
Each `.pchtxt` file should start with some meta information. It should be the beginning of the `.pchtxt` file and located before any empty lines, and the meta parsing will only look there to avoid going through the entire file. The meta info in `splat310.pchtxt` was:

```
@title Splatoon™ 2 3.1.0 USA
@program 01003BC0000A0000
@url "https://raw.githubusercontent.com/3096/ipswitch/master/example/splat310.pchtxt"
```

Each of these `@` symbols that lead each line is considered a "tag". OK then, so what is a ta-

---
## Tags
Tags are at the beginning of a line, and can be used to signify special intents for the parsing of `.pchtxt`. Each tag can have its own "tag content" separated by a space after the tag. There are two types of tags: meta tags and parsing tags.

### Meta Tags
Meta tags, as mentioned above, provides meta information for a `.pchtxt` located at the beginning of `.pchtxt` files. While they are not mandatory for a `.pchtxt` to be parsed, mom says it's good for you. Use them.

| Tag | Purpose | Tag Content |
| --- |---------| ----------- |
| `@title` | The title for this `.pchtxt`, for display purposes | Example: `Splatoon™ 2 3.1.0 USA` |
| `@program` | The program ID to be targeted by this `.pchtxt`. Also know as "title ID".<br> This meta tag is mandatory for exporting to AMS cheat txt files, as it is required in order to find the correct directory inside `sdmc:/atmosphere` | Example: `01003BC0000A0000` |
| `@url` | The url from which this `.pchtxt` can be downloaded.<br> It is used as a convenience feature for users to update a `.pchtxt` that is published at a static url, straight from inside the IPSwitch app | Any static url. (MUST be surrounded in quotation marks, otherwise `/` will be parsed as a comment) |

### Parsing Tags
Parsing tags are other tags that can be used anywhere in the `.pchtxt`. They are your best friends because most of them are, in fact, required for parsing your `.pchtxt` correctly.

| Tag | Purpose | Tag Content |
| --- |---------| ----------- |
| `@enabled` / `@disabled` | To signify if the current patch is enabled or disabled.<br> This tag is mandatory for every patch except raw AMS cheats, which are enabled by default | Type of the [patch](#patches):<br> `bin`, `heap`, `ams`<br> |
| `@flag` | To signify flags used in parsing. There are many flag types, and they will be detailed [below](#flags-flag) | [Varies based on the flag type](#flags-flag) |
| `@stop` | Parsing will stop when this tag is reached.<br> This is normally not necessary as parsing will stop naturally when it reaches EOF| None |

---
## Patches

Before starting a patch, you must set the [build ID flag](#flags-flag).

```
// Force money to 6969699
@enabled
0019784C 400DA052602C8B72
```

Patches start with a line of comment. Comments are marked with `/` symbol.

```
// this is a comment
```

This comment line is used as the title/description of the patch. Additionally, you can use `[]` to credit the author of the patch.

```
// cool patch with author [author name]
```

After the comment line is the `@enabled`/`@disabled` tag. This tag represents if the patch is currently enabled or not. Note that this tag is also necessary as the parser uses it to recognize start of a patch.

You can also specify the type by adding it to the tag content of the `@enabled`/`@disabled` tag:

```
@enabled heap
```

There are three types of patches currently supported, and the content of the patch depends on the type:

| Type | Description | Patch Content Format | Applicable Target |
| :--: | ----------- | :------------------: | :---------------: |
| `bin` | patches to NSO/NRO binaries | Token | IPS, AMS cheat |
| `heap` | static value to heap | Token | AMS cheat |
| `ams` | same format as AMS cheat | AMS cheat | AMS cheat |

When the type is not explicitly specified, `bin` type will be used.

### Token Format

In token format, the patch is parsed line by line, as "tokens". There needs to be at least two tokens per line. The first token is the offset of the patch. The subsequent token(s) are "value tokens" to be written to that offset. This is a basic example of what a patch content line might look like: 

```
DEADBEEF DEEDEDEE
         ↑ a value token
↑ an offset token
```

There is no limit to how many value tokens you can have.

The offset token will always be in big endian. The value tokens are small endian by default, and big endian can be otherwise specified with a flag.

- Tokenized values are designed to easier handle multiple consecutive big endian values
- For the details on how little/big endian behave and how to specify the endianness of value tokens, see [`@flag` section](#endianness-flag-bele)

The tokens can be any length, with the exception that offset tokens cannot be longer than 4 bytes, or 8 characters (enough for handling any binary file size for the Switch, and even heap addresses won't exceed 32bit when calculated from the heap base). Each value token must consist of two characters for every byte. For example, `EAF` is not considered completed bytes, `0EAF` is. It is up to you to decide what length makes sense to you (like a 3-byte big endian value probably doesn't make sense).

These are all valid patch contents under the token format:

```
DEADBEEF DEEDEDEE
CAFEBABA CAABFEFEFED42060
FACEDEAD FACEFADE FACEDDAD
7707 3096
```

### String Patch

Instead of bytes, you can also patch using string literals. Each string opens and closes with a pair of `"`.

```
00044670 "a cool string"
```

String patches will only have 2 tokens: the offset token and the string token (like I can't really think of a reason for multiple consecutive string tokens since endianness doesn't apply here, you can just stitch them into one string).

C style string escapes are also supported. So a string value can be something like `"New\nLine"`.

### AMS Cheat Format

This format follows the same encoding as AMS cheats.

```
// Force sea snail to 696
@enabled ams
04000000 00197898 52805700
```

As a convenience feature, AMS cheats can also be dropped into a `.pchtxt` directly without any changes, and it will be treated as `@enabled ams` as default.

```
[Force sea snail to 696]
04000000 00197898 52805700
```

Unless you want to disable it, you could do:

```
[Force sea snail to 696]
@disabled
04000000 00197898 52805700
```

---
## Flags (`@flag`)

The `@flag` tag allows setting different flags at any line during the parsing of the `.pchtxt`, and the effect will apply to the lines below the flag. It follows the general format of

```
@flag flag_type flag_value
```

| Type | Description | Value | Example |
| :--: |-------------| ----- | ------- |
| [`be`/`le`](#endianness-flag-bele) | flags the parsing of value tokens to big/little endian | None | `@flag be` |
| [`nsobid`/`nrobid`](#build-id-flag-nsobidnrobid) | sets the build ID of the current NSO/NRO | build ID, without zero bytes | `@flag nsobid 034C8FA7A63B7A87F96F408B2AEFFF6C` |
| [`offset_shift`](#offset-shift-flag-offset_shift) | shift all upcoming offset by a certain amount | how many bytes to shift | `@flag offset_shift 0x100` |
| [`debug_info`](#debug-info-flag-debug_info) | display additional debug info in the parsing log | None | `@flag debug_info` |

### Endianness (`@flag be`/`le`)

This flag sets the parsing of value tokens to either big endian or little endian. When not flagged, little endian is used by default.

Since ARM uses little endian, when big endian flag is set, the values will be treated as big endian, thus having its byte order swapped to little endian during parsing. When parsing as little endian, the value is simply copied over.

Here is a table explaining what the parser does under the hood:

| Value Token | Parsed as LE | Parsed as BE |
| ----------- | ------------ | ------------ |
| 1122 | 11 22 | 22 11 |
| 11223344 | 11 22 33 44 | 44 33 22 11 |
| 11223344 55667788 | 11 22 33 44 55 66 77 88 | 44 33 22 11 88 77 66 55 |
| "ABCDE" | 41 42 43 44 45 00 | 41 42 43 44 45 00 |

Yeah, that last one is a string, and ignores endianness.

### Build ID (`@flag nsobid`/`nrobid`)

This flag is mandatory, and should be set at least once before any patches.

You can also set this flag more than once, which means you can apply patches to multiple NSO/NROs within the same `.pchtxt`.

### Offset Shift (`@flag offset_shift`)

This flag is useful if you want to use the `.text` addresses as your offset directly. You probably see `@flag offset_shift 0x100` used everywhere. It is the same as adding 0x100 to all your offsets, thus compensating for the 0x100-byte long NSO header.

### Debug Info (`@flag debug_info`)
Exactly what it sounds like. This enables printing additional debug information to parsing logs, including

- Parsed values
- Entering a new patch
- Changing build ID
- Ignored lines

This flag may be helpful for diagnosis if something isn't working as intended.

---
## Echo Identifier `#`
Lines starting with `#` will always be printed to the parsing log when the parser reaches it.

It can be used for providing extra information or for debug purposes.

```
# this line will be printed to parsing log
```

---
## Using `.elf` Files and Generate a Patched NSO
Did anyone seriously use this feature? Cuz I removed it.
