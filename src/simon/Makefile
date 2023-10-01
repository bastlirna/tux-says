all : flash

EXTRA_CFLAGS+=-Werror 
TARGET:=tux_says_main
ADDITIONAL_C_FILES:=tone.c leds.c buttons.c isr_handler.c systick.c game.c \
	logger.c ch32v003fun_missing.c errors.c games/battle_game.c games/attract_game.c \
	games/test_game.c games/game_utils.c
CH32V003FUN:=../../ch32v003fun/ch32v003fun
include ../../ch32v003fun/ch32v003fun/ch32v003fun.mk

flash : cv_flash
clean : cv_clean

