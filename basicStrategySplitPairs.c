#include "basic_strategy.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
  srand(time(NULL));

  int dealerUpcard = (rand() % 9) + 1;
  int playerPair = (rand() % 9) + 1;
  char printPlayerPair;
  char userAnswer;
  char correctAnswer;

  // typecasting for converting a randomly generated '1' into an 'A'
  if (playerPair == 1) {
    printPlayerPair = 'A';
  } else {
    printPlayerPair = playerPair;
  }

  // Print messages
  if (printPlayerPair == 'A') {
    printf("You have a pair of %c's!\n", printPlayerPair);
  } else {
    printf("You have a pair of %d's!\n", printPlayerPair);
  }
  printf("Dealer's up card is %d\n", dealerUpcard);

  // Get user choice
  printf("Do you split? (Y)es or (N)o: ");
  scanf("%c", &userAnswer);

  // Check the actual correct answer
  if (PairSplitting[playerPair][dealerUpcard] == Y) {
    correctAnswer = 'Y';
  } else {
    correctAnswer = 'N';
  }

  // Compare
  if (correctAnswer == userAnswer) {
    printf("Correct!");
  } else {
    printf("Incorrect!");
  }

  return 0;
}
