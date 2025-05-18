
all:
ifeq ($(OS),Windows_NT)
# CFLAGS=" -I../../" LDLIBS=" /mingw64/lib/libncursesw.a /mingw64/lib/libdl.a /mingw64/lib/libwinpthread.a -lgdi32 -luser32 -lcomdlg32 -lwinmm" make brickgame-cli
	CFLAGS=" -I../../" LDLIBS=" -lncursesw -ldl "                  make brickgame-cli
else
	CFLAGS=" -I../../" LDLIBS=" -lncurses" LDFLAGS=" -Wl,-rpath,." make brickgame-cli
endif
