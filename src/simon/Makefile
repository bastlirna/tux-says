all : flash

EXTRA_CFLAGS+=-Werror
TARGET:=tux_says_main
ADDITIONAL_C_FILES:= logger.c ch32v003fun_missing.c errors.c tasks.c systick.c \
	game.c idle_task.c isr_handler.c tone.c leds.c buttons.c power_management.c \
	games/memory_game.c games/attract_game.c games/test_game.c games/game_utils.c
CH32V003FUN:=../../ch32v003fun/ch32v003fun
include ../../ch32v003fun/ch32v003fun/ch32v003fun.mk

flash : cv_flash
clean : cv_clean

