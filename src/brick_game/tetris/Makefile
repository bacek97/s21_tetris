lib_static:
	CFLAGS=" -I../../ -fpic -D_POSIX_C_SOURCE=199309L" LDFLAGS=" -lpthread -shared `[[ -v MSYSTEM ]] && echo -static`" make "libtetris.a(tetris.o)"

lib_shared:
	CFLAGS=" -I../../ -fpic -D_POSIX_C_SOURCE=199309L" LDFLAGS=" -lpthread -shared `[[ -v MSYSTEM ]] && echo -static`" make tetris

test: lib_static
	checkmk checkmk.basic_complete.ts > test_tetris.c
	CFLAGS="-I../../" LDFLAGS=" -L." LDLIBS="-ltetris `pkg-config --cflags --libs --static check`" make test_tetris
ifeq ($(shell uname -s), Darwin)
	bash -c "grep -e 'leaks for' <(CK_FORK=no leaks -q --atExit -- ./test_tetris)"
else                   # Linux, Windows
	@CK_FORK=no valgrind --vgdb=no --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=RESULT_VALGRIND.txt ./test_tetris || ./test_tetris
endif

# gcov_report: CFLAGS += -g --coverage -fprofile-arcs -ftest-coverage
# gcov_report: lib_static test
build_test_with_coverage_for_gcov:
	@if [[ -v MSYSTEM && `pwd | LC_ALL=C tr -d '[:print:]'` ]];then echo "ERROR: .gcda files cannot be created in a path containing non-ascii characters" && exit 1 ;fi
	CFLAGS=" -I../../ -g --coverage -fprofile-arcs -ftest-coverage" make "libtetris.a(tetris.o)"     #(make lib_static)
	checkmk checkmk.basic_complete.ts > test_tetris.c
	CFLAGS="-I../../ --coverage" LDFLAGS="-L." LDLIBS="-ltetris `pkg-config --cflags --libs --static check`" make test_tetris
# ./test_tetris  #  /only/ascii/full.path!!!
# lcov -c  -o lcov.genhtml -d .  `[[ -v MSYSTEM ]] || echo --no-external`
# -genhtml -o report lcov.genhtml  # --ignore-errors unmapped
# -genhtml -o report lcov.genhtml    --ignore-errors unmapped