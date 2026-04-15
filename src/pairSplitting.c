#include "basicStrategy.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

// Dealer
// UpCard-----A------2------3------4------5------6------7------8------9-----10
Action PairSplitting[10][10] = {
    /* A,A */ {Y, Y, Y, Y, Y, Y, Y, Y, Y, Y},
    /* 2,2 */ {N, YN, YN, Y, Y, Y, Y, N, N, N},
    /* 3,3 */ {N, YN, YN, Y, Y, Y, Y, N, N, N},
    /* 4,4 */ {N, N, N, N, YN, YN, N, N, N, N},
    /* 5,5 */ {N, N, N, N, N, N, N, N, N, N},
    /* 6,6 */ {N, YN, Y, Y, Y, Y, N, N, N, N},
    /* 7,7 */ {N, Y, Y, Y, Y, Y, Y, N, N, N},
    /* 8,8 */ {Y, Y, Y, Y, Y, Y, Y, Y, Y, Y},
    /* 9,9 */ {N, Y, Y, Y, Y, Y, N, Y, Y, N},
    /* 10,10 */ {N, N, N, N, N, N, N, N, N, N},
};

int surrender[3][10] = {
    /* 14 */ {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    /* 15 */ {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    /* 16 */ {0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
};

int pairSplittingTrainer(Score *score, Settings *settings) {
  // Generate random pair and dealerUpCard
  int dealerUpCard = (rand() % 10) + 1;
  int playerPair = (rand() % 10) + 1;

  char printPlayerPair;
  char printDealerUpCard;
  char userAnswer;
  char correctAnswer;

  // typecasting for converting a randomly generated '1' into an 'A'
  if (playerPair == 1) {
    printPlayerPair = 'A';
  } else {
    printPlayerPair = playerPair;
  }
  if (dealerUpCard == 1) {
    printDealerUpCard = 'A';
  } else {
    printDealerUpCard = dealerUpCard;
  }

  // Print messages
  if (printPlayerPair == 'A') {
    printf("You have a pair of %c's!\n", printPlayerPair);
  } else {
    printf("You have a pair of %d's!\n", printPlayerPair);
  }
  if (printDealerUpCard == 'A') {
    printf("Dealer's up card is %c\n", printDealerUpCard);
  } else {
    printf("Dealer's up card is %d\n", printDealerUpCard);
  }

  // Get user choice
  printf("Do you split? (Y)es,(N)o, or (Q)uit: ");
  scanf(" %c", &userAnswer);

  // exit
  if (toupper(userAnswer) == 'Q') {
    return 0;
  }

  score->total++;

  // Check the actual correct answer
  if (PairSplitting[playerPair - 1][dealerUpCard - 1] == Y) {
    correctAnswer = 'Y';
  } else {
    correctAnswer = 'N';
  }

  // Compare
  if (correctAnswer == toupper(userAnswer)) {
    printf("Correct!\n");
    score->correct++;
  } else {
    printf("Incorrect! Correct answer was %c\n", correctAnswer);
  }

  return 1;
}
