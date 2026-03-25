#include "basic_strategy.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
  srand(time(NULL));

  int dealerUpcard = (rand() % 9) + 1;
  int playerPair = (rand() % 9) + 1;
  char printPlayerPair;

  if (playerPair == 1) {
    printPlayerPair = 'A';
  } else {
    printPlayerPair = playerPair;
  }

  printf("You have a pair of %d's!\n", printPlayerPair);
  printf("Dealer's up card is %d\n", dealerUpcard);
  printf("Do you split? (Y)es or (N)o: ");
  // scanf()

  return 0;
}
