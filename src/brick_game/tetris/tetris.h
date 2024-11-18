/* -------------------"THE Cola-WARE LICENSE" (Revision 21):-------------------
 * <liliammo@student.21-school.ru> wrote this code.
 * As long as you complies with Google Style guidelines, you can do whatever 
 * you want with this stuff. If we meet someday, and you think this stuff is
 * worth it, you can buy me a Cola in return.                    Vasilii Kostin
 * ------------------------------------------------------------------------- */
#ifndef C7_BRICKGAME_BRICK_GAME_TETRIS_TETRIS_H
#define C7_BRICKGAME_BRICK_GAME_TETRIS_TETRIS_H

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


#include <brick_game/brickgame.h>

#include <stdbool.h>
void userInput(UserAction_t action, bool hold);
GameInfo_t updateCurrentState();

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
#define SCORES_IN_LEVEL 6

//!  Максимальное количество уровней — 10.
#define MAX_LEVEL 10



typedef void *(*ThreadFunc_t)(void *);
// /*!
//  * @brief Главная функция программы.
//  * 
//  *  \snippet brick_game/tetris/tetris.c Adding a resource
//  */

// #define _POSIX_C_SOURCE 199309L  // POSIX.1b: Real-time extensions
#include <pthread.h>
// #include <stdint.h>
#ifndef _UINTPTR_T_DEFINED
#define _UINTPTR_T_DEFINED
#undef uintptr_t
  typedef unsigned long long uintptr_t;
#endif /* _UINTPTR_T_DEFINED */

  bool updateFsmPtr(const uintptr_t WhoInit);

#endif  // C7_BRICKGAME_TETRIS_TETRIS_H