/* -------------------"THE Cola-WARE LICENSE" (Revision 21):-------------------
 * <liliammo@student.21-school.ru> wrote this code.
 * As long as you complies with Google Style guidelines, you can do whatever 
 * you want with this stuff. If we meet someday, and you think this stuff is
 * worth it, you can buy me a Cola in return.                    Vasilii Kostin
 * ------------------------------------------------------------------------- */

#include <brick_game/tetris/tetris.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>

#include "brick_game/brickgame.h"

enum {
  OLD,
  NEW,
  CountHistory,
  NMINO = 4,
  SizeActionsStack = 10,
  CHMOD = 0666,
  NsPerS = 1000000000
};

int writeReadHighscore(int highscore) {
  int fd = open("highscore.tetris", O_RDWR | O_CREAT, CHMOD);
  if (highscore) {
    write(fd, &highscore, sizeof(highscore));
  } else {
    read(fd, &highscore, sizeof(highscore));
  }
  close(fd);
  return highscore;
}

void unloadingThr();

void *infinityThr(void *ptr_arg);

void sozdatelUbivatel(bool b) {
  static pthread_t thr_x[4];
  static int n = 0;
  if (b) {
    pthread_create(&thr_x[n++], NULL, infinityThr, NULL);
  } else {
    --n;
    pthread_cancel(thr_x[n]);
    pthread_join(thr_x[n], NULL);
  }
}

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

typedef struct {
  int y, x;
} Point_t;

struct condition_bundle_t {
  pthread_mutex_t lock;
  pthread_cond_t cond;
  struct timespec abstime;
};

enum states {
  StateSplashScr,
  StateMoving,
  StatePaused,
  StateUnloading,
  CountStates,
  StateNull
};

bool moveFigure(int **field, UserAction_t signal, Point_t source_figure[NMINO]);

typedef struct {
  struct condition_bundle_t pthread_bundle[1];
  enum states fsm_state;
  struct {
    int *row[FIELD_ROWS];
    int cell[FIELD_ROWS][FIELD_COLS];
  } field;
  struct {
    int *row[FNEXT_ROWS];
    int cell[FNEXT_ROWS][FNEXT_COLS];
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

TetrisInfo_t *staticPtrTetInfo() {
  static TetrisInfo_t tet_info = {0};
  static TetrisInfo_t *ptr_tet_info = NULL;
  if (!ptr_tet_info) {
    for (int y = 0; y < FIELD_ROWS; ++y) {
      tet_info.field.row[y] = tet_info.field.cell[y];
    }
    for (int y = 0; y < FNEXT_ROWS; ++y) {
      tet_info.next.row[y] = tet_info.next.cell[y];
    }
    pthread_mutex_init(&tet_info.pthread_bundle[0].lock, NULL);
    pthread_cond_init(&tet_info.pthread_bundle[0].cond, NULL);
    tet_info.fsm_state = StateNull;
    ptr_tet_info = &tet_info;
  }
  return ptr_tet_info;
}

GameInfo_t *staticPtrInfo() {
  static GameInfo_t info = {0};
  static GameInfo_t *ptr_info = NULL;
  if (!ptr_info) {
    TetrisInfo_t *tet_info = staticPtrTetInfo();
    info.field = tet_info->field.row;
    info.next = tet_info->next.row;
    info.high_score = writeReadHighscore(0);
    ptr_info = &info;
  }
  return ptr_info;
}

void *infinityThr(void *ptr_arg) {
  while (staticPtrTetInfo()->fsm_state != StateUnloading) {
    threadUpdateCurrentState();
  }
  pthread_exit(ptr_arg);
  return NULL;
}

void incrementScore(int add) {
  staticPtrInfo()->score += add;
  if (staticPtrInfo()->score > staticPtrInfo()->high_score) {
    writeReadHighscore(staticPtrInfo()->high_score = staticPtrInfo()->score);
  }
  int level = (staticPtrInfo()->score / SCORES_IN_LEVEL) + 1;
  if (level > MAX_LEVEL) {
    level = MAX_LEVEL;
  }
  staticPtrInfo()->level = level;
}

void (*const FsmFuncPtr[CountStates][Action + 1])(bool) = {
    {userInputStart, userInputRandomOnly, userInputTerminate,
     userInputTerminateLeft, userInputTerminate, userInputRandomOnly,
     userInputRandomOnly, userInputRandomOnly},  // STATE_SPLASH_SCR
    {NULL, userInputPause, userInputTerminate, userInputLeft, userInputRight,
     NULL, userInputDown, userInputAction},  // STATE_MOVING
    {NULL, userInputPause, userInputTerminate, NULL, NULL, NULL, NULL,
     NULL},  // STATE_PAUSED
};

bool checkOutOfBorderXAndCollisionY(Point_t p, int **field) {
  bool out_of_border = false;
  if (FIELD_ROWS <= p.y) {
    out_of_border = true;
  }
  if (p.x < 0 || FIELD_COLS <= p.x) {
    out_of_border = true;
  }
  bool collision = false;
  if (!out_of_border && p.y >= 0 && p.x >= 0) {
    if (field[p.y][p.x] != 0) {
      collision = true;
    }
  }
  return collision | out_of_border;
}

void renderFigure(Point_t figure_new[NMINO], int **field) {
  for (int i = 0; i < NMINO; ++i) {
    if (figure_new[i].x >= 0 && figure_new[i].y >= 0) {
      field[figure_new[i].y][figure_new[i].x] = '*';
    }
  }
}

void *s21Memcpy(void *dest, const void *src, size_t n) {
  for (size_t i = 0; i < n; i++) {
    *((char *)dest + i) = *((char *)src + i);
  }
  return dest;
}

void userInput(UserAction_t action, bool hold) {
  staticPtrTetInfo()->actions.hash_all_pressed_keys += action;
  unsigned *n = &staticPtrTetInfo()->actions.cur_w;
  staticPtrTetInfo()->actions.stack[*n].ua = action;
  staticPtrTetInfo()->actions.stack[*n].hold = hold;
  staticPtrTetInfo()->actions.stack[*n].new = true;
  (*n)++;
  *n %= SizeActionsStack;

  pthread_mutex_lock(
      &staticPtrTetInfo()->pthread_bundle->lock);  // Захватываем мьютекс
  pthread_cond_signal(
      &staticPtrTetInfo()->pthread_bundle->cond);  // Сигнализируем о завершении
  pthread_mutex_unlock(
      &staticPtrTetInfo()->pthread_bundle->lock);  // Освобождаем мьютекс
}

struct timespec addTimeoutToAbstime(long gravity_tv_nsec) {
  struct timespec abstime = {0};
  clock_gettime(CLOCK_REALTIME, &abstime);
  abstime.tv_sec += 0 + (abstime.tv_nsec + gravity_tv_nsec) / NsPerS;
  abstime.tv_nsec = (abstime.tv_nsec + gravity_tv_nsec) % NsPerS;
  return abstime;
}

bool compareTimespecLt(struct timespec t1, struct timespec t2) {
  return (t1.tv_sec == t2.tv_sec) ? (t1.tv_nsec < t2.tv_nsec)
                                  : (t1.tv_sec < t2.tv_sec);
}

void updateGravityTimeout() {
  // seconds = (0.8- ((level-1)*0.007) )^(level-1)
  const long GravityTvNsec[MAX_LEVEL] = {
      1000000000 - 1, 793000000, 617796032, 472729120, 355196992,
      262003536,      189677264, 134734704, 93882264,  64151572};
  int level = staticPtrInfo()->level;
  if (level != 0) {
    level -= 1;
  }
  struct timespec *t1 = &staticPtrTetInfo()->pthread_bundle[0].abstime;
  struct timespec t2;
  clock_gettime(CLOCK_REALTIME, &t2);
  if (compareTimespecLt(*t1, t2)) {
    *t1 = addTimeoutToAbstime(GravityTvNsec[level]);
    staticPtrInfo()->speed = GravityTvNsec[level];
  }
}

bool isUserInput() {
  struct timespec *abstime = &staticPtrTetInfo()->pthread_bundle[0].abstime;
  bool is_timedout = false;
  pthread_mutex_lock(
      &staticPtrTetInfo()->pthread_bundle[0].lock);  // Захватываем мьютекс
  if (staticPtrTetInfo()->fsm_state == StateMoving) {
    updateGravityTimeout();
    is_timedout = pthread_cond_timedwait(
        &staticPtrTetInfo()->pthread_bundle[0].cond,
        &staticPtrTetInfo()->pthread_bundle[0].lock, abstime);
  } else {
    pthread_cond_wait(&staticPtrTetInfo()->pthread_bundle[0].cond,
                      &staticPtrTetInfo()->pthread_bundle[0].lock);
  }
  pthread_mutex_unlock(&staticPtrTetInfo()->pthread_bundle[0].lock);
  if (is_timedout) {
    shiftingThr();
  }
  return !is_timedout;
}

GameInfo_t updateCurrentState() {  // не принимает сигнал
  if (staticPtrTetInfo()->fsm_state == StateNull) {
    initThr();
    sozdatelUbivatel(true);
  }
  if (staticPtrTetInfo()->fsm_state == StateUnloading) {
    unloadingThr();
  }
  return *staticPtrInfo();
}

Point_t *newNextFigure() {
  static Point_t fsc[][NMINO] = {
      {{0, -1}, {0, 0}, {0, 1}, {0, 2}},   // I
      {{1, -1}, {1, 0}, {0, 0}, {0, 1}},   // S
      {{0, -1}, {0, 0}, {1, 0}, {1, 1}},   // Z
      {{1, -1}, {1, 0}, {1, 1}, {0, 0}},   // T
      {{1, -1}, {1, 0}, {1, 1}, {0, 1}},   // L
      {{1, -1}, {1, 0}, {1, 1}, {0, -1}},  // J
      {{1, -1}, {1, 0}, {0, 0}, {0, -1}}   // O
  };
  unsigned rndm = staticPtrTetInfo()->actions.hash_all_pressed_keys;
  rndm %= NELEMS(fsc);
  return fsc[rndm];
}

void *s21Memset(void *dst, int c, size_t n) {
  for (char *p = (char *)dst + n; p > (char *)dst;) {
    *--p = (char)c;
  }
  return dst;
}

void eraseFigure(int **field, Point_t *figure_old, int ch);

void spawnFigure() {
  staticPtrTetInfo()->source_figure[OLD] =
      staticPtrTetInfo()->source_figure[NEW];
  staticPtrTetInfo()->source_figure[NEW] = newNextFigure();

  s21Memset(&staticPtrTetInfo()->figure[NEW], 0,
            sizeof(staticPtrTetInfo()->figure[NEW]));
  s21Memset(&staticPtrTetInfo()->figure[OLD], 0,
            sizeof(staticPtrTetInfo()->figure[OLD]));

  s21Memset(staticPtrTetInfo()->next.cell, 0,
            sizeof(staticPtrTetInfo()->next.cell));
  moveFigure(staticPtrInfo()->next, Right,
             staticPtrTetInfo()->source_figure[NEW]);

  staticPtrTetInfo()->figure[NEW].offset_x = FIELD_COLS / 2;
}

bool moveFigure(int **field, UserAction_t signal,
                Point_t source_figure[NMINO]) {
  staticPtrTetInfo()->figure[NEW].offset_x -= (signal == Left);
  staticPtrTetInfo()->figure[NEW].offset_x += (signal == Right);
  staticPtrTetInfo()->figure[NEW].offset_y += (signal == Down);
  staticPtrTetInfo()->figure[NEW].rotate_count += (signal == Action);
  bool err = false;

  eraseFigure(staticPtrInfo()->field, staticPtrTetInfo()->figure[NEW].p, 0);

  for (int i = 0; i < NMINO; i++) {
    Point_t *p = &staticPtrTetInfo()->figure[NEW].p[i];
    *p = source_figure[i];
    for (int j = 0; j < staticPtrTetInfo()->figure[NEW].rotate_count; j++) {
      // p->x = -(p->x ^= p->y ^= p->x ^= p->y);
      int y = 1 + (p->x - 1);
      p->x = 1 - (p->y - 1);
      p->y = y;
    }
    p->y += staticPtrTetInfo()->figure[NEW].offset_y;
    p->x += staticPtrTetInfo()->figure[NEW].offset_x;
    err |= checkOutOfBorderXAndCollisionY(*p, field);
  }
  if (!err) {
    s21Memcpy(&staticPtrTetInfo()->figure[OLD],
              &staticPtrTetInfo()->figure[NEW],
              sizeof(staticPtrTetInfo()->figure) / CountHistory);
  } else {
    s21Memcpy(&staticPtrTetInfo()->figure[NEW],
              &staticPtrTetInfo()->figure[OLD],
              sizeof(staticPtrTetInfo()->figure) / CountHistory);
  }
  renderFigure(staticPtrTetInfo()->figure[OLD].p, field);
  return err;
}

void initThr() {
  renderLogo();
  staticPtrInfo()->pause = true;
  staticPtrInfo()->level = 1;
  staticPtrInfo()->score = 0;

  updateFsmPtr((uintptr_t)&initThr);  // moving_thr
  spawnFigure();
  spawnFigure();
}

void shifting() {
  if (moveFigure(staticPtrInfo()->field, Down,
                 staticPtrTetInfo()->source_figure[OLD])) {
    updateFsmPtr((uintptr_t)&shifting);
    staticPtrInfo()->speed = 0;
    staticPtrInfo()->pause = 1;
  }
}

void shiftingThr() {
  int err = 0;
  err = moveFigure(staticPtrInfo()->field, Down,
                   staticPtrTetInfo()->source_figure[OLD]);
  if (err == true) {
    attaching();
    shifting();
  }
  updateFsmPtr((uintptr_t)&shiftingThr);
}

//! [Adding a resource]
bool updateFsmPtr(const uintptr_t WhoInit) {
  enum states *cur_state = &staticPtrTetInfo()->fsm_state;
  bool not_switched = true;
  struct {
    enum states src;
    enum states dst;
    uintptr_t sgn;
  } arr[] = {
      {StateNull, StateSplashScr, (uintptr_t)&initThr},
      {StateSplashScr, StateUnloading, (uintptr_t)&userInputTerminate},
      {StateSplashScr, StateUnloading, (uintptr_t)&userInputTerminateLeft},
      {StateMoving, StateUnloading, (uintptr_t)&userInputTerminate},
      {StatePaused, StateUnloading, (uintptr_t)&userInputTerminate},
      {StatePaused, StateMoving, (uintptr_t)&userInputPause},
      {StateMoving, StatePaused, (uintptr_t)&userInputPause},
      {StateSplashScr, StateMoving, (uintptr_t)&userInputStart},
      {StateMoving, StateSplashScr, (uintptr_t)&shifting},
  };

  for (int i = 0; i < (int)NELEMS(arr) && not_switched; i++) {
    if ((*cur_state == arr[i].src && arr[i].sgn == WhoInit)) {
      *cur_state = arr[i].dst;
      not_switched = false;
    }
  }

  return not_switched;
}
//! [Adding a resource]

bool getLastActionAndEraseItFromStack(UserAction_t *action, bool *hold) {
  unsigned *r = &staticPtrTetInfo()->actions.cur_r;
  bool new = staticPtrTetInfo()->actions.stack[*r].new;
  if (new) {
    staticPtrTetInfo()->actions.stack[*r].new = false;
    *action = staticPtrTetInfo()->actions.stack[*r].ua;
    *hold = staticPtrTetInfo()->actions.stack[*r].hold;
    *r += 1;
    *r %= SizeActionsStack;
  }
  // else { alternateTimer(); }
  return new;
}

void threadUpdateCurrentState() {
  isUserInput();
  UserAction_t action = Start;
  bool hold = false;
  while (getLastActionAndEraseItFromStack(&action, &hold)) {
    if (FsmFuncPtr[staticPtrTetInfo()->fsm_state][action]) {
      FsmFuncPtr[staticPtrTetInfo()->fsm_state][action](hold);
    }
  }
}

void userInputPause(bool hold) {
  (void)hold;
  if (!updateFsmPtr((uintptr_t)&userInputPause)) {
    staticPtrInfo()->pause = !staticPtrInfo()->pause;
  }
}

void unloadingThr() {
  sozdatelUbivatel(false);
  staticPtrInfo()->field = NULL;
}

void renderLogo() {
  char splashscr[] =
      "          "
      "          "
      "          "
      "          "
      " CCCCCCCC "
      "    C     "
      "    C     "
      " C  C   C "
      "CC  C   CC"
      " C  C   C "
      "    C     "
      "    C     "
      "    C     "
      "          "
      "          "
      "          "
      "          "
      "          "
      "          "
      "          ";
  for (size_t i = 0; i < NELEMS(splashscr) - 1; ++i) {
    staticPtrInfo()->field[i / FIELD_COLS][i % FIELD_COLS] = splashscr[i] - ' ';
  }
}

void userInputRandomOnly(bool hold) {
  spawnFigure();
  (void)hold;
}

void userInputStart(bool hold) {
  spawnFigure();
  (void)hold;
  if (!updateFsmPtr((uintptr_t)&userInputStart)) {
    s21Memset(&staticPtrTetInfo()->field.cell[0][0], 0,
              FIELD_ROWS * FIELD_COLS * sizeof(int));
    staticPtrInfo()->pause = false;
  }
}

void userInputTerminate(bool hold) {
  (void)hold;
  if (!updateFsmPtr((uintptr_t)&userInputTerminate)) {
    staticPtrInfo()->field = NULL;
    staticPtrInfo()->next = NULL;
  }
}

void userInputTerminateLeft(bool hold) {
  (void)hold;
  if (!updateFsmPtr((uintptr_t)&userInputTerminateLeft)) {
    staticPtrInfo()->field = NULL;
  }
}

void userInputMove(UserAction_t action, bool hold) {
  bool err = false;
  do {
    err = moveFigure(staticPtrInfo()->field, action,
                     staticPtrTetInfo()->source_figure[OLD]);
  } while (hold && hold != err);
}

void userInputAction(bool hold) {
  hold = false;
  userInputMove(Action, hold);
}
void userInputDown(bool hold) {
  hold = true;
  userInputMove(Down, hold);
}
void userInputLeft(bool hold) { userInputMove(Left, hold); }
void userInputRight(bool hold) { userInputMove(Right, hold); }

void eraseFigure(int **field, Point_t *figure_old, int ch) {
  for (int i = 0; i < NMINO && figure_old; ++i) {
    if (figure_old[i].y >= 0 && figure_old[i].x >= 0) {
      field[figure_old[i].y][figure_old[i].x] = ch;
    }
  }
}

void attaching() {
  int **field = staticPtrInfo()->field;
  eraseFigure(field, staticPtrTetInfo()->figure[OLD].p, '#');
  spawnFigure();
  int yw = FIELD_ROWS - 1;
  int yr = FIELD_ROWS - 1;
  while (yr > 0) {
    int filled_cols = 0;
    for (int col = 0; col < FIELD_COLS; col++) {
      filled_cols += !!field[yr][col];
    }
    if (filled_cols == FIELD_COLS) {
      --yr;
      continue;
    }
    for (int copy_col = 0; copy_col < FIELD_COLS; copy_col++) {
      field[yw][copy_col] = field[yr][copy_col];
    }
    --yw;
    --yr;
  }
  const int A[] = {BONUS};
  incrementScore(A[yw - yr]);
}
