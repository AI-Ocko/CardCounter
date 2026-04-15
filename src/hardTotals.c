#include "basicStrategy.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

// Dealer
// UpCard-----A------2------3------4------5------6------7------8------9-----10
Action HardTotals_H17[10][10] = {
    /*  8 */ {H, H, H, H, H, H, H, H, H, H},
    /*  9 */ {H, D, D, D, D, H, H, H, H, H},
    /* 10 */ {D, D, D, D, D, D, D, D, H, H},
    /* 11 */ {D, D, D, D, D, D, D, D, D, D},
    /* 12 */ {H, H, S, S, S, H, H, H, H, H},
    /* 13 */ {S, S, S, S, S, H, H, H, H, H},
    /* 14 */ {S, S, S, S, S, H, H, H, H, H},
    /* 15 */ {S, S, S, S, S, H, H, H, H, H},
    /* 16 */ {S, S, S, S, S, H, H, H, H, H},
    /* 17 */ {S, S, S, S, S, S, S, S, S, S}};

Action HardTotals_S17[10][10] = {
    /*  8 */ {H, H, H, H, H, H, H, H, H, H},
    /*  9 */ {H, D, D, D, D, H, H, H, H, H},
    /* 10 */ {D, D, D, D, D, D, D, D, H, H},
    /* 11 */ {D, D, D, D, D, D, D, D, D, H},
    /* 12 */ {H, H, S, S, S, H, H, H, H, H},
    /* 13 */ {S, S, S, S, S, H, H, H, H, H},
    /* 14 */ {S, S, S, S, S, H, H, H, H, H},
    /* 15 */ {S, S, S, S, S, H, H, H, H, H},
    /* 16 */ {S, S, S, S, S, H, H, H, H, H},
    /* 17 */ {S, S, S, S, S, S, S, S, S, S}};

int hardTotalTrainer(Score *score, Settings *settings) {
  // Assign an Action pointer to either S17 or H17 tables
  Action(*HardTotals)[10] = (settings->h17) ? HardTotals_H17 : HardTotals_S17;

  int dealerUpCard = (rand() % 10) + 1; // shifts value to A(1)-10
  int playerHardTotal =
      (rand() % 10) + 8; // shifts hardtotal to actual range 8-17

  char printPlayerHardTotal;
  char printDealerUpCard;
  char userAnswer;
  char correctAnswer;

  // set print cards to appropriate values
  if (dealerUpCard == 1) {
    printDealerUpCard = 'A';
  } else {
    printDealerUpCard = dealerUpCard;
  }
  printPlayerHardTotal =
      playerHardTotal; // add 8 to the index to reflect actual total

  // Print hard total and delaer up card
  printf("You have a total of %d!\n", printPlayerHardTotal);
  if (printDealerUpCard == 'A') {
    printf("Dealer's up card is %c!\n", printDealerUpCard);
  } else {
    printf("Dealer's up card is %d!\n", printDealerUpCard);
  }

  // Get user choice
  printf("Do you (H)it, (D)ouble, (S)tand, or (Q)uit?: ");
  scanf(" %c", &userAnswer);

  // exit
  if (toupper(userAnswer) == 'Q') {
    return 0;
  }

  // Increment total number of hands played
  score->total++;

  // Check correct answer
  switch (HardTotals[playerHardTotal - 8][dealerUpCard - 1]) {
  case 0:
    correctAnswer = 'H';
    break;
  case 1:
    correctAnswer = 'S';
    break;
  case 2:
    correctAnswer = 'D';
    break;
  default:
    printf("error checking answer\n");
    break;
  }

  // Compare
  if (correctAnswer == toupper(userAnswer)) {
    printf("Correct!\n");
    score->correct++; // Increment correct number of answers
  } else {
    printf("Incorrect!, Correct answer was %c\n", correctAnswer);
  }

  return 1;
};
