#include "basic_strategy.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
  srand(time(NULL));

  int dealerUpcard = (rand() % 9) + 1;
  int playerPair = (rand() % 9) + 1;

  printf("You have a pair of %d's!\n", playerPair);
  printf("Dealer's up card is %d\n", dealerUpcard);
  printf("Do you split? (Y)es or (N)o: ");
  // scanf()

  return 0;
}
