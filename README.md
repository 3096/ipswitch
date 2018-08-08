# IPSwitch
Use text files to generate `.ips` patches or patched `nso` to use with Atmosphere.

### To use `.ips` patches make sure you have Atmosphere 44e2412a (July 30, 2018) or later loader build.

See example for details.

---
## Credit
- plutoo for making elf2nso, used in compressing patched elf to nso

---
## Build
Make sure you have:
- DevkitA64
- libnx
- [lz4](https://github.com/lz4/lz4) compiled with DevkitA64

The simply run: `make`

---
## How to use
See example

---
## TODO
- Improve selection interface
- Support selecting `.pchtxt` files
- Toggle an edit enable or disable
- Manage any existing `.ips` files, like a true `.ips` patch manager
