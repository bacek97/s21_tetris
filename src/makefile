.PHONY: all, install, uninstall, clean, dvi, dist, test, gcov_report
SHELL := /bin/bash
CC := gcc
CFLAGS += -std=c11 
CFLAGS += -Wall -Wextra 
# CFLAGS += -Werror
CFLAGS += -Wpedantic
# CFLAGS += -g
# CFLAGS += -march=native  
CFLAGS += -Ofast

all: dist install
	cd DESTDIR/usr/local/bin/ && LD_LIBRARY_PATH=../lib/ ./brickgame-cli ../lib/libtetris.dll ../lib/libtetris..so && cd -

tetris:
	CFLAGS=" -I. -fpic" LDFLAGS=" -shared `[[ -v MSYSTEM ]] && echo -static`" make "brick_game/tetris/libtetris.a(brick_game/tetris/tetris.o)" brick_game/tetris/tetris

brickgame-cli: tetris
ifeq ($(OS),Windows_NT)
	CFLAGS=" -I." LDLIBS=" -lpdcurses -ldl" 				  make gui/cli/brickgame-cli
else
	CFLAGS=" -I." LDLIBS=" -lncurses" LDFLAGS=" -Wl,-rpath,." make gui/cli/brickgame-cli
endif

test: tetris
	checkmk brick_game/tetris/checkmk.basic_complete.ts > brick_game/tetris/test.c
	CFLAGS="-I." LDFLAGS=" -Lbrick_game/tetris" LDLIBS="-ltetris `pkg-config --cflags --libs --static check`" make brick_game/tetris/test
ifeq ($(shell uname -s), Darwin)
	bash -c "grep -e 'leaks for' <(CK_FORK=no leaks -q --atExit -- ./brick_game/tetris/test)"
else                   # Linux, Windows
	@CK_FORK=no valgrind --vgdb=no --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=RESULT_VALGRIND.txt ./brick_game/tetris/test || ./brick_game/tetris/test
endif



ifeq ($(OS),Windows_NT)
JAVA_4UML := PATH=$$(cygpath -u "C:\ProgramData\Oracle\Java\javapath"):$$PATH
# else ifeq ($(shell uname -s),Darwin)
endif
dvi:
	$(JAVA_4UML) doxygen
# $(MAKE) -C $(BUILDDIR)/doc/latex/

# BrickGame-1.0/
# ├── bin/                    # Исполняемые файлы
# │   └── brick_game            # Основной исполняемый файл
# └── lib/                    # Библиотеки
#     ├── libtetris.so      	# Подключаемая библиотека игры
#	  ├── libncurses.so			# Библиотека необходимая для запуска
#	  └── libdl.so				# Библиотека необходимая для запуска
dist: tetris brickgame-cli
	mkdir -p BrickGame-1.0/{bin,lib,doc}
	cp gui/cli/brickgame-cli BrickGame-1.0/bin/
	cp brick_game/tetris/tetris`[[ -v MSYSTEM ]] && echo .exe` "BrickGame-1.0/lib/libtetris`[[ -v MSYSTEM ]] && echo .dll || echo .so`"
	-upx BrickGame-1.0/{bin,lib}/*
	cp `ls /*/*/{libdl.{??,???},lib*curses.{??,???}} 2>/dev/null` BrickGame-1.0/lib/
	tar -czf DIST_BrickGame-1.0.tar.gz BrickGame-1.0
	rm -r BrickGame-1.0

clean:
	-rm -r brick_game/tetris/{tetris{.o,},libtetris.a}
	-rm -r brick_game/tetris/{checkmk.basic_complete.c,test{.c,},highscore.tetris}
	-rm -r gui/cli/brickgame-cli{.exe,}
	-rm -r DIST_BrickGame-1.0.tar.gz BrickGame-1.0
	-rm -r DESTDIR
	-find . -name "*.gc??" -exec rm -f {} \;
	-rm -r report lcov.genhtml
	-rm -r doc

# https://www.opennet.ru/docs/RUS/coding_standard/standard-5.html
DESTDIR := DESTDIR
prefix := /usr/local
bindir = $(prefix)/bin
libdir = $(prefix)/lib
includedir := $(prefix)/include
INSTALL := tar --strip-components=2 -xzf
INSTALL_PROGRAM := $(INSTALL)
INSTALL_DATA := $(INSTALL)
install: DIST_BrickGame-1.0.tar.gz
	mkdir -p $(DESTDIR)$(bindir) $(DESTDIR)$(libdir)
	$(INSTALL_PROGRAM) $< -C $(DESTDIR)$(bindir) BrickGame-1.0/bin
	$(INSTALL_DATA) $< -C $(DESTDIR)$(libdir) BrickGame-1.0/lib

# make install DESTDIR= prefix=/mingw64 libdir=/mingw64/bin -n uninstall
uninstall:
	rm $(DESTDIR)$(bindir)/brickgame-cli*
	rm $(DESTDIR)$(libdir)/libtetris*

gcov_report:
	@if [[ -v MSYSTEM && `pwd | LC_ALL=C tr -d '[:print:]'` ]];then echo "ERROR: .gcda files cannot be created in a path containing non-ascii characters" && exit 1 ;fi
	CFLAGS=" -I. -g --coverage -fprofile-arcs -ftest-coverage" make "brick_game/tetris/libtetris.a(brick_game/tetris/tetris.o)"
	checkmk brick_game/tetris/checkmk.basic_complete.ts > brick_game/tetris/test.c
	CFLAGS="-I. --coverage" LDFLAGS="-Lbrick_game/tetris" LDLIBS="-ltetris `pkg-config --cflags --libs --static check`" make brick_game/tetris/test
	./brick_game/tetris/test  #  /only/ascii/full.path!!!
	lcov -c  -o lcov.genhtml -d brick_game/tetris  `[[ -v MSYSTEM ]] || echo --no-external`
	-genhtml -o report lcov.genhtml  # --ignore-errors unmapped
	-genhtml -o report lcov.genhtml    --ignore-errors unmapped



# Dependencies
# https://github.com/upx/upx/releases/download/v4.2.4/upx-4.2.4-win64.zip
# pacman -S mingw-w64-x86_64-lcov mingw-w64-x86_64-clang upx mingw-w64-x86_64-check mingw-w64-x86_64-gcc mingw-w64-x86_64-pdcurses make mingw-w64-x86_64-dlfcn mingw-w64-x86_64-doxygen mingw-w64-x86_64-cppcheck
# brew install upx
# apt install make gcc check lcov libncurses-dev upx-ucl doxygen graphviz

# find . -name "*.c" -exec clang-tidy {} -checks="readability-*,bugprone-*,performance-*" -- -I. \;

style:
# checkmk brick_game/tetris/checkmk.basic_complete.ts > brick_game/tetris/test.c
	-@if [[ "" != `find . \( -name "*.c" -o -name "*.h" \) -exec grep -iL "<.*@.*\..*>" {} +` ]];then echo "`find . \( -name "*.c" -o -name "*.h" \) -exec grep -iL "<.*@.*\..*>" {} +` Every file should contain license boilerplate" && exit 1 ;fi
	@find . -name "*.c" -exec clang-format  -style="{BasedOnStyle: Google, InsertBraces: true}" --verbose --Werror --ferror-limit=0 -i {} +;
	@clang-tidy `find . \( -name "*.c" -o -name "*.h" \) -not -name "*test*"` \
    --format-style="{BasedOnStyle: Google, InsertBraces: true}" -config="{CheckOptions: [ \
	{key: readability-identifier-naming.VariableCase, value: lower_case}, \
	{key: readability-identifier-naming.FunctionCase, value: CamelCase}, \
	{key: readability-identifier-naming.TypedefCase, value: CamelCase}, \
	{key: readability-identifier-naming.ConstantCase, value: CamelCase}, \
	{key: readability-identifier-naming.EnumConstantCase, value: CamelCase}, \
	{key: readability-identifier-naming.ConstantPrefix, value: k }, \
	{key: readability-identifier-naming.EnumConstantPrefix, value: k}, \
	{key: readability-identifier-naming.MacroDefinitionCase, value: UPPER_CASE}, \
	{key: readability-identifier-naming.NamespaceCase, value: lower_case}, \
	{key: readability-identifier-naming.ConceptCase, value: CamelCase}, \
	{key: readability-identifier-naming.MemberCase, value: lower_case}, \
	{key: readability-identifier-naming.ClassMemberSuffix, value: _}, \
	\
	{key: readability-identifier-naming.FunctionCase, value: camelBack}, \
	{key: readability-identifier-naming.ConstantPrefix, value: '' }, \
	{key: readability-identifier-naming.EnumConstantPrefix, value: '' }, \
	{key: readability-identifier-naming.TypedefSuffix, value: _t}, \
	]}" \
	-checks="readability-identifier-naming,llvm-header-guard,cppcoreguidelines-*,google-*,llvm-include-order" \
	--warnings-as-errors="-readability-identifier-naming,cppcoreguidelines-*,google-*" \
	 \
	-- -I"`pwd`"
	@cppcheck --enable=all --suppress=missingIncludeSystem  --suppress=checkersReport --check-level=exhaustive -I. .
