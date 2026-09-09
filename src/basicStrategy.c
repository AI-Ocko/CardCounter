#include "../include/basicStrategy.h"
#include "../include/layout.h"
#include <curses.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void printCenteredText(WINDOW *window, int row, const char *text) {
  int height, width;
  getmaxyx(window, height, width);
  if (row < 0 || row >= height)
    return;

  int col = (width - (int)strlen(text)) / 2;
  if (col < 0)
    col = 0;

  /* Clip instead of letting ncurses wrap the overflow onto the next row. */
  mvwaddnstr(window, row, col, text, width - col);
}

static const struct {
  const char *optionsName;
} Options[] = {
    {"Pair Splitting"}, {"Soft Totals"}, {"Hard Totals"},
    {"Full Game"},      {"Settings"},
};

static const int numberOfOptions = sizeof(Options) / sizeof(Options[0]);

/* Menu entries that map onto TrainerOptions, in the same order.  "Full Game"
 * and "Settings" sit past the end of that table. */
static const int numberOfTrainers =
    sizeof(TrainerOptions) / sizeof(TrainerOptions[0]);
static const int settingsOption = 4;

static void drawMainMenu(WINDOW *window, int selection) {
  MenuLayout layout = computeMenuLayout(window, numberOfOptions);

  werase(window);
  box(window, 0, 0);
  printCenteredText(window, layout.titleRow, "Main Menu");

  for (int i = 0; i < numberOfOptions; i++) {
    if (i == selection)
      wattron(window, A_STANDOUT);
    printCenteredText(window, layout.firstOptionRow + i * layout.optionStride,
                      Options[i].optionsName);
    if (i == selection)
      wattroff(window, A_STANDOUT);
  }

  wattron(window, A_DIM);
  printCenteredText(window, layout.hintRow,
                    "j/k or up/down to move     Enter to select");
  if (layout.quitHintRow >= 0)
    printCenteredText(window, layout.quitHintRow, "q to quit");
  wattroff(window, A_DIM);

  wrefresh(window);
}

/* Run one trainer in a window sized for the current terminal.  Returns what
 * the trainer returned: 0 means the user quit the application. */
static int runTrainer(int selection, Settings *settings) {
  WINDOW *trainerWindow = newwin(1, 1, 0, 0);
  keypad(trainerWindow, TRUE);
  fitTrainerWindow(trainerWindow);

  Score gameScore = {0, 0};
  int keepRunning = TrainerOptions[selection](trainerWindow, &gameScore,
                                              settings);

  delwin(trainerWindow);
  /* The trainer covered the screen; clear what it left behind so the menu is
   * not drawn over its remains. */
  handleResize();

  return keepRunning;
}

int main(void) {
  // Initialize ncurses
  setlocale(LC_ALL, "");
  initscr();   // Start ncurses mode, creates stdscr
  cbreak();    // Disable line buffering, get input char-by-char
  noecho();    // don't echo typed keys automatically
  curs_set(0); // hides the terminal cursor
  keypad(stdscr, TRUE); // stdscr reads the keys on the "too small" screen
  initColors();

  // Initialize Settings
  Settings gameSettings;
  Settings *ptrSettings = &gameSettings;
  loadSettings(ptrSettings);

  // Random seed
  srand(time(NULL));

  /* The window is sized from the terminal on every pass through the loop, so
   * it starts out as a placeholder. */
  WINDOW *mainMenuWindow = newwin(1, 1, 0, 0);
  keypad(mainMenuWindow, TRUE); // enable arrow keys, F-keys, etc.

  int selection = 0, keyPress, running = 1;

  while (running) {
    if (!ensureUsableTerminal())
      break;

    fitMenuWindow(mainMenuWindow, numberOfOptions);
    drawMainMenu(mainMenuWindow, selection);

    switch ((keyPress = wgetch(mainMenuWindow))) {
    case 'q':
      running = 0;
      break;
    case KEY_RESIZE:
      handleResize();
      break;
    case 'k':
    case KEY_UP:
      if (selection > 0)
        selection--;
      break;
    case 'j':
    case KEY_DOWN:
      if (selection < numberOfOptions - 1)
        selection++;
      break;
    case '\n':
    case '\r':
    case KEY_ENTER:
      if (selection == settingsOption) {
        running = settingsMenu(mainMenuWindow, 0, ptrSettings);
      } else if (selection < numberOfTrainers) {
        running = runTrainer(selection, ptrSettings);
      }
      /* "Full Game" has no trainer behind it yet, so it does nothing. */
      break;
    }
  }

  delwin(mainMenuWindow);
  endwin();

  return 0;
}
