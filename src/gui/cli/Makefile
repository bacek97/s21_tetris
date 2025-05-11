
all:
ifeq ($(OS),Windows_NT)
#	CFLAGS=" -I../../" LDLIBS=" -lpdcurses -ldl"                   make gui/cli/brickgame-cli
	CFLAGS=" -I../../" LDLIBS=" -lncursesw -ldl"                   make brickgame-cli
else
	CFLAGS=" -I../../" LDLIBS=" -lncurses" LDFLAGS=" -Wl,-rpath,." make brickgame-cli
endif
