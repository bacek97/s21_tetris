// https://libcheck.github.io/check/doc/doxygen/html/check_8h.html
// https://www.mankier.com/1/checkmk
#include <stdio.h>
#include "brick_game/brickgame.h"
#include "brick_game/tetris/tetris.h"



#suite NULL_TEST
#tcase NULL_TEST

#test NULL_ADD
updateCurrentState();
userInput(Start, false);
userInput(Down, true);
userInput(Down, true);
userInput(Down, true);
userInput(Down, true);
userInput(Right, false);

struct timespec req = {18, 500000000};  // 1.5 секунды
nanosleep(&req, NULL);  // Задержка на 1.5 секунды

userInput(Action, false);
userInput(Pause, false);
userInput(Pause, false);
userInput(Pause, false);

char exp[] =
    "\n..........\n"
    "..........\n"
    "..........\n"
    ".....#....\n"
    "..........\n"
    "..........\n"
    "..........\n"
    "..........\n"
    "..........\n"
    "..........\n"
    "..........\n"
    "..........\n"
    "..........\n"
    ".....#....\n"
    "..........\n"
    "..........\n"
    "..........\n"
    "..........\n"
    "..........\n"
    "..........";

char res[] =
    "\n          \n"
    "          \n"
    "          \n"
    "          \n"
    "          \n"
    "          \n"
    "          \n"
    "          \n"
    "          \n"
    "          \n"
    "          \n"
    "          \n"
    "          \n"
    "          \n"
    "          \n"
    "          \n"
    "          \n"
    "          \n"
    "          \n"
    "          ";

GameInfo_t info = updateCurrentState();

size_t i2 = 0;
for (size_t i = 0; i < NELEMS(exp) - 1; ++i) {
  if (exp[i] != '\n') {
    res[i] = (info.field[i2 / FIELD_COLS][i2 % FIELD_COLS]) ? '#' : '.';
    ++i2;
  }
}
ck_assert_msg(1, "Failed123445");

#main-pre
  tcase_set_timeout(tc1_1, 30);
  printf("\033[0;36m");  // бирюзовый (Passed Failed)
    
#main-post
  printf("\033[0;31m"); // красный (CHECK.H Failures: N)
  if (nf != 0) {
    printf("CHECK.H Failures: %d\n", nf);
  }
  printf("\033[0;35m");  // розовый (VALGRIND)
  return 0;