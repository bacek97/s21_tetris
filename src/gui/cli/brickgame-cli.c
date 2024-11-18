/*
 * ------------------------------------------------------------
 * "THE Cola-WARE LICENSE" (Revision 21):
 * <liliammo@student.21-school.ru> wrote this code.
 * As long as you complies with Google Style guidelines,
 * you can do whatever you want with this stuff. If we meet
 * someday, and you think this stuff is worth it,
 * you can buy me a Cola in return.              Vasilii Kostin
 * ------------------------------------------------------------
 */

/*! @file

*/

enum { BrickgameTimeout = 30 };

/*!
 * @brief
 * При использовании ncurses неизбежны утечки памяти:
 * @code
 * #include <ncurses.h>  // libncurses-dev 6.4+20240113-1ubuntu2
 * int main() {
 *     initscr();  // valgrind-3.22.0 ERROR SUMMARY: 3 errors from 3 contexts
 *     endwin();
 * }
 * @endcode
 */
#ifdef _WIN32
#include <pdcurses.h>
#else
#include <ncurses.h>
#endif
#include <brick_game/brickgame.h>
#include <dlfcn.h>

/*!
 * @brief
 * Вшитые имена файлов библиотек с играми, можно переопределить до компиляции:
 * @code
 * CFLAGS="-I../../ -DLIBS_GAMES=\\\"libsnake.dll\\\"" LDLIBS="-lncurses" make
 * brickgame_cli{.o,}
 * @endcode
 * А также можно передать имена файлов библиотек в качестве аргументов звпуска:
 * @code
 * LD_LIBRARY_PATH=. ./brickgame_cli "libsnake.dll" "libfrogger.so"
 * @endcode
 */
#if !defined(LIBS_GAMES)
#if defined(_WIN32)
#define LIBS_GAMES "libtetris.dll"
#else
#define LIBS_GAMES "libtetris.so"
#endif
#endif

bool getActionHold(UserAction_t *action, bool *hold) {
  static int q_key[3] = {ERR, ERR, ERR};
  q_key[0] = q_key[1];
  q_key[1] = q_key[2];
  q_key[2] = getch();

  bool ret = q_key[0] == ERR && q_key[1] != ERR;
  if (ret) {
    ret = false;
    *hold = q_key[1] == q_key[2];

    const int UserActionKeys[] = {KEY_F(7),  KEY_F(2), 27,       KEY_LEFT,
                                  KEY_RIGHT, KEY_UP,   KEY_DOWN, 32};
    for (UserAction_t ua = Start; ua <= Action; ua++) {
      if (q_key[1] == UserActionKeys[ua]) {
        *action = ua;
        ret = true;
      }
    }
  }
  // if (ret)
  // fprintf("\nPRESSED: key:%d %d %d ua:%d hold:%d\n", q_key[0], q_key[1],
  // q_key[2], *action, *hold);

  return ret;
}

bool unLoadDllSo(char *libname_dll_so,
                 void (**userInput_ptr)(UserAction_t action, bool hold),
                 GameInfo_t (**updateCurrentState_ptr)()) {
  static void *library_handler = NULL;
  if (library_handler) {
    dlclose(library_handler);
    *userInput_ptr = NULL;
    *updateCurrentState_ptr = NULL;
  }
  if (libname_dll_so) {
    library_handler = dlopen(libname_dll_so, RTLD_LAZY | RTLD_LOCAL);
    union {
      void *void_ptr;
      void (*func_ptr)(UserAction_t, bool);
    } pedantic_ptr_converter1;
    pedantic_ptr_converter1.void_ptr = dlsym(library_handler, "userInput");
    *userInput_ptr = pedantic_ptr_converter1.func_ptr;

    union {
      void *void_ptr;
      GameInfo_t (*func_ptr)();
    } pedantic_ptr_converter2;
    pedantic_ptr_converter2.void_ptr =
        dlsym(library_handler, "updateCurrentState");
    *updateCurrentState_ptr = pedantic_ptr_converter2.func_ptr;
  } else {
    library_handler = NULL;
  }
  return library_handler;
}

void renderField(WINDOW *win, int **field, int height, int width) {
  wmove(win, 0, 0);
  for (int i = 0; i < width * height; ++i) {
    waddch(win, (field[i / width][i % width] == 0)
                    ? '.'
                    : field[i / width][i % width]);
    if (getcurx(win) && ((i % width) == (width - 1))) {
      wmove(win, getcury(win) + 1, 0);
    }
  }
  wrefresh(win);
}

char *strtokGames(int argc, char *argv[], int req_game) {
  int offset = (req_game == Right ? 1 : -1);
  static int current = 0;
  enum { FnameLength = 100 };
  static char hardcoded_games[][FnameLength] = {LIBS_GAMES};
  int count_hardcoded_games =
      sizeof(hardcoded_games) / sizeof(hardcoded_games[0]);
  char *game = NULL;
  current = (current + offset) % (count_hardcoded_games + argc - 1);
  if (current == -1) {
    current = (count_hardcoded_games + argc - 1) - 1;
  }
  if (current < count_hardcoded_games) {
    game = hardcoded_games[current];
  } else if (current < count_hardcoded_games + argc - 1) {
    game = argv[current];
  }
  return game;
}

WINDOW *initNcurses() {
  WINDOW *stdscr_local = initscr();
  keypad(stdscr_local, TRUE);  // ncurses read KEY_DOWN codes
  curs_set(0);
  noecho();
  // raw();
  return stdscr_local;
}

void renderAside(WINDOW *win, GameInfo_t info) {
  renderField(win, info.next, FNEXT_ROWS, FNEXT_COLS);
  wprintw(win, "\n\nscore\n: %d00", info.score);
  wprintw(win, "\n\nhigh_score: %d00", info.high_score);
  wprintw(win, "\n\nlevel\n: %d", info.level);
  wprintw(win, "\n\nspeed\n:%9d", info.speed);
  wprintw(win, (info.pause) ? "\n\nPAUSE" : "\n\n     ");
}

int main(int argc, char *argv[]) {
  void (*user_input_ptr)(UserAction_t action, bool hold) = NULL;
  GameInfo_t (*update_current_state_ptr)() = NULL;

  WINDOW *stdscr_local = initNcurses();
  WINDOW *sw_field = subwin(stdscr_local, FIELD_ROWS, FIELD_COLS, 1, 1);
  WINDOW *sw_aside =
      subwin(stdscr_local, FIELD_ROWS, FIELD_COLS, 1, FIELD_COLS + 2);

  int next_game = Right;
  char *gamelib = NULL;
  while (1) {
    if (!update_current_state_ptr) {
      gamelib = strtokGames(argc, argv, next_game);
      unLoadDllSo(gamelib, &user_input_ptr, &update_current_state_ptr);
    }
    mvprintw(0, 0, "gamelib: %s %p %p", gamelib, user_input_ptr,
             update_current_state_ptr);

    UserAction_t action = Start;
    bool hold = false;
    timeout((update_current_state_ptr) ? BrickgameTimeout : -1);
    bool is_key_pressed = getActionHold(&action, &hold);
    if (is_key_pressed && user_input_ptr) {
      (*user_input_ptr)(action, hold);
    }

    if (update_current_state_ptr) {
      GameInfo_t info =
          (*update_current_state_ptr)();  // обязан выполнить очистку перед
                                          // завершением игры
      if (info.field == NULL) {
        unLoadDllSo(NULL, &user_input_ptr, &update_current_state_ptr);
        next_game = (info.next) ? Left : Right;
      } else {
        renderField(sw_field, info.field, FIELD_ROWS, FIELD_COLS);
        renderAside(sw_aside, info);
      }
      if (is_key_pressed && action == Terminate && hold == true) {
        break;
      }
    }
  }
  endwin();  // завершение работы с ncurses
  // cbreak();
}
