#ifndef BASICSTRATEGY_H
#define BASICSTRATEGY_H

#include <curses.h>

// Settings
typedef struct {
  char doubleAfterSplit; //   Y/N
  char h17OrS17;         //   H/S

} Settings;

// Trainer Tools
typedef struct {
  int correct;
  int total;

} Score;

typedef enum {
  H = 0,
  S = 1,
  D = 2,
  Ds = 3,
  N = 4,
  Y = 5,
  YN = 6,
  Surr = 7

} Action;

/* Trainers return 1 to carry on, 0 when the user quit the application from
 * the "terminal too small" screen. */
int pairSplittingTrainer(WINDOW *window, Score *score, Settings *settings);
int softTotalTrainer(WINDOW *window, Score *score, Settings *settings);
int hardTotalTrainer(WINDOW *window, Score *score, Settings *settings);

typedef int (*trainerPointer)(WINDOW *window, Score *score, Settings *settings);

static const trainerPointer TrainerOptions[] = {
    pairSplittingTrainer,
    softTotalTrainer,
    hardTotalTrainer,
};

// Trainer Helper Functions
int dealDealerUpCard(void);
void printDealerUpCard(WINDOW *window, int dealerUpCard);
char answerToChar(WINDOW *window, Action a, Settings *settings);
void checkAndScore(WINDOW *window, Score *score, char correctAnswer,
                   char userAnswer);

void loadSettings(Settings *settings);

/* Returns 1 to carry on, 0 when the user quit the application from the
 * "terminal too small" screen. */
int settingsMenu(WINDOW *window, int selection, Settings *settings);

/* Centre `text` on `row`, measuring against the window's current width and
 * clipping anything that does not fit rather than wrapping it. */
void printCenteredText(WINDOW *window, int row, const char *text);

// Basic Strategy Charts
extern Action HardTotalsH17[10][10];
extern Action HardTotalsS17[10][10];
extern Action SoftTotalsH17[8][10];
extern Action SoftTotalsS17[8][10];
extern Action PairSplitting[10][10];
extern int surrender[3][10];

#endif
