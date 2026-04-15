#include "settings.h"
#include <stdio.h>

static const Settings defaults = {
    .h17 = 1, .s17 = 0, .das = 1, .rsa = 1, .surrender = 1, .numDecks = 6};

void saveSettings(const Settings *s) {
  FILE *f = fopen(SETTINGS_FILE, "w");

  if (!f) {
    printf("Warning: could not save settings.\n");
    return;
  }

  fprintf(f, "h17=%d\n", s->h17);
  fprintf(f, "das=%d\n", s->das);
  fprintf(f, "rsa=%d\n", s->rsa);
  fprintf(f, "surrender=%d\n", s->surrender);
  fprintf(f, "numDecks=%d\n", s->numDecks);
  fclose(f);
}

int loadSettings(Settings *s) {
  FILE *f = fopen(SETTINGS_FILE, "r");

  if (!f) {
    *s = defaults;
    return 0;
  }

  fscanf(f, "h17=%d\n", &s->h17);
  fscanf(f, "das=%d\n", &s->das);
  fscanf(f, "rsa=%d\n", &s->rsa);
  fscanf(f, "surrender=%d\n", &s->surrender);
  fscanf(f, "numDecks=%d\n", &s->numDecks);
  fclose(f);

  return 1;
}

void printSettings(const Settings *s) {
  printf("Current Settings:\n");
  printf("  Dealer soft 17 : %s\n", s->h17 ? "Hit (H17)" : "Stand (S17)");
  printf("  Double after split : %s\n", s->das ? "Yes" : "No");
  printf("  Resplit Aces : %s\n", s->rsa ? "Yes" : "No");
  printf("  Late surrender : %s\n", s->surrender ? "Yes" : "No");
  printf("  Number of decks : %d\n", s->numDecks);
}

void settingsMenu(Settings *s) {
  char option;
  do {
    printf("\n--- Settings ---\n");
    printSettings(s);
    printf("\n");
    printf("1. Toggle H17/S17\n");
    printf("2. Toggle Double After Split (DAS)\n");
    printf("3. Toggle Resplit Aces (RSA)\n");
    printf("4. Toggle Late Surrender\n");
    printf("5. Set Number of Decks\n");
    printf("Option: ");
    scanf("%c", &option);

    switch (option) {
    case '1':
      s->h17 = !s->h17;
      break;
    case '2':
      s->das = !s->das;
      break;
    case '3':
      s->rsa = !s->rsa;
    case '4':
      s->surrender = !s->surrender;
    case '5':
      printf("Enter the number of decks (1, 2, 6, 8): ");
      scanf("%d", &s->numDecks);
      break;
    case '0':
      saveSettings(s);
      printf("Settings saved.\n");
      break;
    default:
      printf("Invalid option.\n");
      break;
    }

  } while (option != '0');
}
