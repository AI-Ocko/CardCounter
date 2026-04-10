#ifndef BASICSTRATEGY_H
#define BASICSTRATEGY_H

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

int pairSplittingTrainer(Score *score);
int softTotalTrainer(Score *score);
int hardTotalTrainer(Score *score);

extern Action HardTotals[10][10];
extern Action SoftTotals[8][10];
extern Action PairSplitting[10][10];
extern int surrender[3][10];

#endif
