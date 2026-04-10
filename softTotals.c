#include "basicStrategy.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

// Dealer
// UpCard-----A------2------3------4------5------6------7------8------9-----10
Action SoftTotals[8][10] = {
    /* A,2 */ {H, H, H, H, D, D, H, H, H, H},
    /* A,3 */ {H, H, H, H, D, D, H, H, H, H},
    /* A,5 */ {H, H, H, D, D, D, H, H, H, H},
    /* A,5 */ {H, H, H, D, D, D, H, H, H, H},
    /* A,6 */ {H, H, D, D, D, D, H, H, H, H},
    /* A,7 */ {H, Ds, Ds, Ds, Ds, Ds, S, S, H, H},
    /* A,8 */ {S, S, S, S, S, Ds, S, S, S, S},
    /* A,9 */ {S, S, S, S, S, S, S, S, S, S}};

int softTotalTrainer(Score *score) {
  int dealerUpCard = (rand() % 9) + 1;
  int playerSecondCard = rand() % 8;

  char printPlayerSecondCard;
  char printDealerUpCard;
  char userAnswer;
  char correctAnswer;

  // Convert int to char
  printPlayerSecondCard = playerSecondCard + '2';
  if (dealerUpCard == 1) {
    printDealerUpCard = 'A';
  } else {
    printDealerUpCard = dealerUpCard;
  }

  // Print messages
  printf("You have A,%c\n", printPlayerSecondCard);
  if (printDealerUpCard == 'A') {
    printf("Dealer's up card is %c\n", printDealerUpCard);
  } else {
    printf("Dealer's up card is %d\n", printDealerUpCard);
  }

  // Get user choice
  printf("Do you (H)it, (D)ouble, (S)tand, or (Q)uit?: ");
  scanf(" %c", &userAnswer);

  // exit
  if (toupper(userAnswer) == 'Q') {
    return 0;
  }

  score->total++;

  // Check correct answer
  switch (SoftTotals[playerSecondCard][dealerUpCard - 1]) {
  case 0:
    correctAnswer = 'H';
    break;
  case 1:
    correctAnswer = 'S';
    break;
  case 2:
    correctAnswer = 'D';
    break;
  case 3:
    correctAnswer = 'D';
    break;
  default:
    printf("error checking answer\n");
    break;
  }

  // Compare
  if (correctAnswer == toupper(userAnswer)) {
    printf("Correct!\n");
    score->correct++;
  } else {
    printf("Incorrect! Correct answer was %c\n", correctAnswer);
  }

  return 1;
};
