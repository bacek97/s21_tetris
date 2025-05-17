/* -------------------"THE Cola-WARE LICENSE" (Revision 21):-------------------
 * <liliammo@student.21-school.ru> wrote this code.
 * As long as you complies with Google Style guidelines, you can do whatever 
 * you want with this stuff. If we meet someday, and you think this stuff is
 * worth it, you can buy me a Cola in return.                    Vasilii Kostin
 * ------------------------------------------------------------------------- */
#ifndef TETRIS_H
#define TETRIS_H

#include <brick_game/include/brickgame.h>
#include <stdbool.h>
#include <stdint.h>
// #define _POSIX_C_SOURCE 199309L
#include <time.h>
#ifdef _WIN32
#include <io.h>
#include <pthread_time.h>
#endif
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>

/*! @file
 * @brief Диаграмма переходов состояния FSM:

  Диаграмма требующая для отображения HAVE_DOT = YES
 * @dot
 * digraph finite_state_machine {
 * dpi=58;
	fontname="Helvetica,Arial,sans-serif"
	node [fontname="Helvetica,Arial,sans-serif"]
	edge [fontname="Helvetica,Arial,sans-serif"]
	rankdir=LR;

	node [shape = doublecircle];
  NULL STATE_UNLOADED [URL="brickgame_8h.html"];
	node [shape = circle];

	NULL -> STATE_SPLASH_SCR [label = "updateCurrentState()"];

	STATE_SPLASH_SCR -> STATE_MOVING [label = "pthread_cond_wait() == userInput(Start)"];
  STATE_SPLASH_SCR -> STATE_UNLOADED [label = "pthread_cond_wait() == userInput(Terminate || Left || Right)"];
  
  STATE_MOVING -> STATE_MOVING [label = "pthread_cond_timedwait() == ETIMEDOUT  && isShifted() == true"];
  STATE_MOVING -> STATE_MOVING [label = "pthread_cond_timedwait() == userInput(...)"];
  STATE_MOVING -> STATE_SPLASH_SCR [label = "pthread_cond_timedwait() == ETIMEDOUT && isShifted() == false"];
  STATE_MOVING -> STATE_PAUSED [label = "pthread_cond_timedwait() == userInput(Pause)"];
  STATE_MOVING -> STATE_UNLOADED [label = "pthread_cond_timedwait() == userInput(Terminate)"];
  
  STATE_PAUSED -> STATE_MOVING [label = "pthread_cond_wait() == userInput(Pause)"];
  STATE_PAUSED -> STATE_UNLOADED [label = "pthread_cond_wait() == userInput(Terminate)"];
}
 * @enddot

  Диаграмма требующая для отображения указания пути к файлу PLANTUML_JAR_PATH = plantuml-1.2024.7.jar
 * @startuml
left to right direction

[*] --> STATE_SPLASH_SCR : updateCurrentState()

STATE_SPLASH_SCR --> STATE_MOVING : pthread_cond_wait() == userInput(Start)
STATE_SPLASH_SCR --> STATE_UNLOADED : pthread_cond_wait() == userInput(Terminate || Left || Right)

STATE_MOVING --> STATE_MOVING : pthread_cond_timedwait() == ETIMEDOUT && isShifted() == true
STATE_MOVING --> STATE_MOVING : pthread_cond_timedwait() == userInput(...)
STATE_MOVING --> STATE_SPLASH_SCR : pthread_cond_timedwait() == ETIMEDOUT && isShifted() == false
STATE_MOVING --> STATE_PAUSED : pthread_cond_timedwait() == userInput(Pause)
STATE_MOVING --> STATE_UNLOADED : pthread_cond_timedwait() == userInput(Terminate)

STATE_PAUSED --> STATE_MOVING : pthread_cond_wait() == userInput(Pause)
STATE_PAUSED --> STATE_UNLOADED : pthread_cond_wait() == userInput(Terminate)

@enduml
*/



/*! Количество элементов массива с фиксированным размером,
 * определённым на момент компиляции */
#define NELEMS(a) (sizeof(a) / sizeof((a)[0]))


/*! Начисление очков будет происходить следующим образом:
 * - 1 линия — 100 очков;
 * - 2 линии — 300 очков;
 * - 3 линии — 700 очков;
 * - 4 линии — 1500 очков.
*/
#define BONUS 0, 1, 3, 7, 15

/*! Каждый раз, когда игрок набирает 600 очков,
 * скорость и уровень увеличивается на 1. */
enum { ScoresInLevel = 6 };

//!  Максимальное количество уровней — 10.
enum { MaxLevel = 10 };

/*!
 * @brief Функция переключающая состояние конечного автомата.
 * 
 * \snippet brick_game/tetris/tetris.c Adding a resource
 * @param WhoInit Адрес функции вызвавшей запрос на мену состояния
 * @return Возвращает false в случае если запрашиваемый переход 
 * не был найден в белом списке и соответственно
 * не была произведена смена состояния
 */
bool updateFsmPtr(uintptr_t WhoInit);

enum {
  OLD = 0,
  NEW = 1,
  CountHistory = 2,
  NMINO = 4,
  SizeActionsStack = 10,
  CHMOD = 0666,
  NsPerS = 1000000000
};

enum states {
  StateSplashScr,
  StateMoving,
  StatePaused,
  StateUnloading,
  CountStates,
  StateNull
};



typedef struct {
  int y, x;
} Point_t;

struct condition_bundle_t {
  pthread_mutex_t lock;
  pthread_cond_t cond;
  struct timespec abstime;
};

typedef struct {
  struct condition_bundle_t pthread_bundle[1];
  enum states fsm_state;
  struct {
    int *row[FieldRows];
    int cell[FieldRows][FieldCols];
  } field;
  struct {
    int *row[FnextRows];
    int cell[FnextRows][FnextCols];
  } next;
  struct {
    struct {
      UserAction_t ua;
      bool hold;
      bool new;
    } stack[SizeActionsStack];
    unsigned cur_w;
    unsigned cur_r;
    unsigned hash_all_pressed_keys;
  } actions;
  Point_t *source_figure[CountHistory];
  struct {
    int offset_x;
    int offset_y;
    unsigned rotate_count : 2;  // rotate % 4
    Point_t p[NMINO];
  } figure[CountHistory];
} TetrisInfo_t;


void unloadingThr();
void *infinityThr(void *ptr_arg);
void shiftingAttachingSpawn();
void spawnFigure();
void initThr();
void movingThr();
void shiftingThr();
void timerThr();
void pauseThr();
void splashscrThr();
bool getLastActionAndEraseItFromStack(UserAction_t *action, bool *hold);
void userInputPause(bool hold);
void userInputTerminate(bool hold);
void userInputTerminateLeft(bool hold);
void userInputStart(bool hold);
void userInputRandomOnly(bool hold);
void userInputLeft(bool hold);
void userInputRight(bool hold);
void userInputDown(bool hold);
void userInputAction(bool hold);
void renderLogo();
struct timespec addTimeoutToAbstime(long gravity_tv_nsec);
void threadUpdateCurrentState();
void attaching();
bool moveFigure(int **field, UserAction_t signal, Point_t source_figure[NMINO]);

#endif  // TETRIS_H