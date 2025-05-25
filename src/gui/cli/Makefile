SHELL := /bin/bash
CC := gcc
CFLAGS += -std=c11 
CFLAGS += -Wall -Wextra 
# CFLAGS += -Werror
CFLAGS += -Wpedantic
# CFLAGS += -g
CFLAGS += -march=native
CFLAGS += -Oz -funroll-all-loops -floop-unroll-and-jam
CFLAGS += -fopt-info-optimized -fstack-usage
LDFLAGS += -Wl,--stack,0  # objdump -p gui/cli/brickgame-cli.exe | grep 'Stack'

all:
ifeq ($(OS),Windows_NT)
# CFLAGS=" -I../../" LDLIBS=" /mingw64/lib/libncursesw.a /mingw64/lib/libdl.a /mingw64/lib/libwinpthread.a -lgdi32 -luser32 -lcomdlg32 -lwinmm" make brickgame-cli
	CFLAGS=" -I../../" LDLIBS=" -lncursesw -ldl "                  make brickgame-cli
else
	CFLAGS=" -I../../" LDLIBS=" -lncurses" LDFLAGS=" -Wl,-rpath,." make brickgame-cli
endif
