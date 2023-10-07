# 🐧 Tux Says

Project of development board in form of badge for [LinuxDays 2023](https://www.linuxdays.cz/2023/) conference. 
It is based on CH32V003 MCU and implements simple game called **Tux Says** inspirated by elecronic game [Simon](https://en.wikipedia.org/wiki/Simon_(game)).

*Tux Says is a simple game where you have to repeat the sequence of sounds and colors that Tux makes.*

<img src="doc/images/3d-top.png" height="300px">

## HW

- [Schematics](hw/tux_schematics.png)
- [BOM](hw/bom.md)

## FW

FW is based on [ch32v003fun](https://github.com/cnlohr/ch32v003fun) project. 

In `src` directory there are two projects: `demo` (test application fer testing HW functionality) and `simon` (tux says game).

### Compilation

1. Ch32v003fun is submodule of this project, so you have to clone it with `--recursive` option.
2. Follow [installation instruction](https://github.com/cnlohr/ch32v003fun/wiki/Installation) in ch32v003fun project.
3. Run `make` in `src/simon` directory.

⚠️ **Warning**

In *simon* project there is cooperative multitasking scheduler implemented. Part of it is written in assembly and requires **GCC version 12**. This limitation may be resolved in future. But for now if you use older GCC buttons will not work properly.

For win users, you can use pre build version from [xpack project](https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases/tag/v12.2.0-3/)

