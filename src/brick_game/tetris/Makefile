lib_static:
	CFLAGS=" -I../../ -fpic" LDFLAGS=" -shared `[[ -v MSYSTEM ]] && echo -static`" make "libtetris.a(tetris.o)"

lib_shared:
	CFLAGS=" -I../../ -fpic" LDFLAGS=" -shared `[[ -v MSYSTEM ]] && echo -static`" make tetris

test: lib_static
	checkmk checkmk.basic_complete.ts > test_tetris.c
	CFLAGS="-I../../" LDFLAGS=" -L." LDLIBS="-ltetris `pkg-config --cflags --libs --static check`" make test_tetris
ifeq ($(shell uname -s), Darwin)
	bash -c "grep -e 'leaks for' <(CK_FORK=no leaks -q --atExit -- ./test_tetris)"
else                   # Linux, Windows
	@CK_FORK=no valgrind --vgdb=no --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=RESULT_VALGRIND.txt ./test_tetris || ./test_tetris
endif