.PHONY: all, install, uninstall, clean, dvi, dist, test, gcov_report

all: dist install
	cd DESTDIR/usr/local/bin/ && LD_LIBRARY_PATH=../lib/ ./brickgame-cli ../lib/libtetris.dll ../lib/libtetris.so && cd -

tetris:
	make lib_shared -C brick_game/tetris # lib_static

test:
	make test -C brick_game/tetris

brickgame-cli:
	make -C gui/cli

ifeq ($(OS),Windows_NT)
JAVA_4UML := PATH=$$(cygpath -u "C:\ProgramData\Oracle\Java\javapath"):$$PATH
# else ifeq ($(shell uname -s),Darwin)
endif
dvi:
	$(JAVA_4UML) doxygen
# $(MAKE) -C $(BUILDDIR)/doc/latex/


# По традиции open-source, цель dist должна включать исходные коды.
# Однако, на мой взгляд, эта традиция устарела после того, как GITHub.com стал стандартом для распространения кода.
# Поэтому я считаю, что можно использовать цель dist для упаковки бинарников с целью последующей публикации архива в разделе Releases.

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
	-strip BrickGame-1.0/{bin,lib}/*
	-upx BrickGame-1.0/{bin,lib}/*
	-cp `ls /*/*/{libdl.{??,???},lib*curses.{??,???}} 2>/dev/null` BrickGame-1.0/lib/
	tar -czf DIST_BrickGame-1.0.tar.gz BrickGame-1.0
	rm -r BrickGame-1.0

clean:
	-rm -r brick_game/tetris/{tetris{.su,.o,},libtetris.a}
	-rm -r brick_game/tetris/{checkmk.basic_complete.c,tetris_test{.exe,.c,},highscore.tetris}
	-rm -r gui/cli/brickgame-cli{.exe,.su,}
	-rm -r DIST_BrickGame-1.0.tar.gz BrickGame-1.0
	-rm -r DESTDIR
	-find . -name "*.gc??" -exec rm -f {} \;
	-rm -r report_from_lcov report_from_geninfo lcov.genhtml geninfo.genhtml
	-rm -r doc

# https://www.opennet.ru/docs/RUS/coding_standard/standard-5.html
DESTDIR := DESTDIR
prefix := /usr/local
bindir = $(prefix)/bin
libdir = $(prefix)/lib
includedir := $(prefix)/include
INSTALL := tar --strip-components=2 -xzf  # распаковка архива
INSTALL_PROGRAM := $(INSTALL)
INSTALL_DATA := $(INSTALL)
install:
	mkdir -p $(DESTDIR)$(bindir) $(DESTDIR)$(libdir)
	$(INSTALL_PROGRAM) DIST_BrickGame-1.0.tar.gz -C $(DESTDIR)$(bindir) BrickGame-1.0/bin
	$(INSTALL_DATA) DIST_BrickGame-1.0.tar.gz -C $(DESTDIR)$(libdir) BrickGame-1.0/lib

# make install DESTDIR= prefix=/mingw64 libdir=/mingw64/bin -n uninstall
uninstall:
	rm $(DESTDIR)$(bindir)/brickgame-cli*
	rm $(DESTDIR)$(libdir)/libtetris*

gcov_report:
	make build_test_with_coverage_for_gcov -C brick_game/tetris
	./brick_game/tetris/test_tetris  #  /only/ascii/full.path!!!
	lcov -c  -o lcov.genhtml -d .  `[[ -v MSYSTEM ]] || echo --no-external`
	geninfo  -o geninfo.genhtml --ignore-errors gcov . 
	-genhtml -o report_from_lcov lcov.genhtml  # --ignore-errors unmapped
	-genhtml -o report_from_lcov lcov.genhtml    --ignore-errors unmapped
	-genhtml -o report_from_geninfo geninfo.genhtml 


# Dependencies
# https://github.com/upx/upx/releases/download/v4.2.4/upx-4.2.4-win64.zip
# pacman -S mingw-w64-x86_64-lcov mingw-w64-x86_64-clang upx mingw-w64-x86_64-check mingw-w64-x86_64-gcc mingw-w64-x86_64-pdcurses make mingw-w64-x86_64-dlfcn mingw-w64-x86_64-doxygen mingw-w64-x86_64-cppcheck
# brew install upx
# apt install make gcc check lcov libncurses-dev upx-ucl doxygen graphviz

# find . -name "*.c" -exec clang-tidy {} -checks="readability-*,bugprone-*,performance-*" -- -I. \;

-include google_style_guide.mk
