#include "basicStrategy.h"
#include "settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void runTrainer(int (*trainerFunc)(Score *score, Settings *settings),
                Settings *settings) {
  Score score = {0, 0};
  srand(time(NULL));
  while (trainerFunc(&score, settings)) {
    if (score.total > 0) {
      printf("\n--- Results ---\n");
      printf("Score: %d / %d\n", score.correct, score.total);
      printf("Accuracy: %.1f%%\n\n", (float)score.correct / score.total * 100);
    }
  }
}

int main(void) {
  char menu_option;
  Settings settings = {0};

  if (!loadSettings(&settings)) {
    printf("No settings file found, using defaults.\n");
  }

  printf("\n==Basic==Strategy==Trainer==\n");

  do {
    printf("\n--- Main Menu ---\n");
    printf("1. To Split or Not to Split\n");
    printf("2. Are You Soft Right Now Step-Bro?\n");
    printf("3. Nah, I'm Hard AF\n");
    printf("4. Settings\n");
    printf("0. Exit\n");
    printf("\n");
    printf("\n");
    printf("Please enter an option from the main menu: ");
    scanf(" %c", &menu_option);

    switch (menu_option) {
    case '1':
      runTrainer(pairSplittingTrainer, &settings);
      break;
    case '2':
      runTrainer(softTotalTrainer, &settings);
      break;
    case '3':
      runTrainer(hardTotalTrainer, &settings);
      break;
    case '4':
      settingsMenu(&settings);
      break;
    case '0':
      break;
    default:
      printf("invalid input");
      break;
    }
  } while (menu_option != '0');

  return 0;
}
