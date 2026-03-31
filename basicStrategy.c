#include "basicStrategy.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
  char menu_option, initials;
  int difficulty;

  printf("==============Basic==Strategy==Trainer=============\n");

  do {
    printf("Main Menu\n");
    printf("1. To Split or Not to Split\n");
    printf("0. Exit\n");
    printf("\n");
    printf("\n");
    printf("Please enter an option from the main menu: ");
    scanf(" %c", &menu_option);

    switch (menu_option) {
    case '1':
      srand(time(NULL));
      while (pairSplittingTrainer()) {
      };
      break;
    case '2':
      srand(time(NULL));
      while (softTotalTrainer()) {
      };
    case '0':
      break;
    default:
      printf("invalid input");
      break;
    }
  } while (menu_option != '0');

  return 0;
}
