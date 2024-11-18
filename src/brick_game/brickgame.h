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
 *  @brief Каждая библиотека с игрой должна иметь функции: `userInput`
 * принимающую на вход пользовательское действие `UserAction_t` и дополнительный
 * параметр `bool`, который отвечает за зажатие клавиши.
 *
 * Функция `updateCurrentState` предназначена для получения данных для отрисовки
 * в интерфейсе. Она возвращает структуру, содержащую информацию о текущем
 * состоянии игры. Например, для тетриса истечение таймера приводит к смещению
 * фигуры вниз на один ряд. Эта функция должна вызываться из интерфейса с
 * некоторой периодичностью для поддержания интерфейса в актуальном состоянии.
 * @code
 * #include <stdbool.h>
 * void userInput(UserAction_t action, bool hold);
 * GameInfo_t updateCurrentState();
 * @endcode
 * В коде эмулятора консоли обе функции можно загрузить в локальные переменные:
 * @code
 * #include <dlfcn.h>
 * void (*userInput_ptr)(UserAction_t action, bool hold) = 
 * *dlsym(library_handler, "userInput");
 * 
 * GameInfo_t (*updateCurrentState_ptr)() =
 * *dlsym((*dll_so)(), "updateCurrentState");
 * @endcode
 */

// foo/src/bar/baz.h
// #ifndef FOO_BAR_BAZ_H_
#ifndef C7_BRICKGAME_BRICKGAME_H
#define C7_BRICKGAME_BRICKGAME_H

#define FIELD_ROWS 20  ///< Высота игрового поля
#define FIELD_COLS 10  ///< Ширина игрового поля
#define FNEXT_ROWS 4  ///< Высота поля для отображения следующей фигуры
#define FNEXT_COLS 4  ///< Ширина поля для отображения следующей фигуры


/*! **********************************************************
 * @brief У консоли имеется восемь физических кнопок:
 * начало игры, пауза, завершение игры, действие и четыре стрелочки.
 *
 ************************************************************/
typedef enum {
  Start,      // f1
  Pause,      // f2
  Terminate,  // f3
  Left,       // 3
  Right,      // 4
  Up,         // none
  Down,       // падение
  Action      // rotate  // 7
} UserAction_t;

/*!**********************************************************
 * Игровое поле представляется как матрица размерностью десять на двадцать.
 * Каждый элемент матрицы соответствует "пикселю" игрового поля и может
 * находится в одном из двух состояний: пустой и заполненный. Кроме игрового
 * поля у каждой игры есть дополнительная информация, которая выводится в
 * боковой панели справа от игрового поля. Для дополнительной информации, не
 * используемой во время игры, предусмотреть заглушки.
 ************************************************************/
typedef struct {
  int **field;  ///< Указатель на матрицу размерностью 20x10
  int **next;  ///< Указатель на матрицу размерностью 4x4
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

#endif  // C7_BRICKGAME_BRICKGAME_H