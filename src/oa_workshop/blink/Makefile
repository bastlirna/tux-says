all : flash

# uncomment if you want to use a different compiler then sugested in ch32v003fun
# (e.g. https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases/tag/v12.2.0-3/)
# PREFIX?=riscv-none-elf

EXTRA_CFLAGS+=-Werror
TARGET:=blink
ADDITIONAL_C_FILES:=
CH32V003FUN:=../../../ch32v003fun/ch32v003fun
include ../../../ch32v003fun/ch32v003fun/ch32v003fun.mk

flash : cv_flash
clean : cv_clean

