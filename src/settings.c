#include "../include/basicStrategy.h"
#include "../include/layout.h"
#include <curses.h>
#include <stdlib.h>

void loadSettings(Settings *settings) {
  FILE *settingsFilePointer = fopen("settings.txt", "r");
  if (settingsFilePointer ==
      NULL) { /* If settings.txt doesn't exist, write a new one. */
    settingsFilePointer = fopen("settings.txt", "w");
    return;
    fputc('Y', settingsFilePointer);
    fputc('H', settingsFilePointer);
    fclose(settingsFilePointer);
  } else {
    settings->doubleAfterSplit = fgetc(settingsFilePointer);
    settings->h17OrS17 = fgetc(settingsFilePointer);
    fclose(settingsFilePointer);
    // WINDOW *testWindow = newwin(10, 10, 10, 10);
    // box(testWindow, 0, 0);
    // mvwprintw(testWindow, 0, 0, "Loaded Settings Successfully!");
    // mvwprintw(testWindow, 2, 0, "Press any key to continue");
    // wgetch(testWindow);
    // werase(testWindow);
    // wrefresh(testWindow);
    // delwin(testWindow);
  }
}

static void saveSettings(Settings *settings) {
  FILE *settingsFilePointer = fopen("settings.txt", "w");
  if (settingsFilePointer == NULL) {
    fclose(settingsFilePointer);
    return;
  } else {
    fputc(settings->doubleAfterSplit, settingsFilePointer);
    fputc(settings->h17OrS17, settingsFilePointer);
    fclose(settingsFilePointer);
  }
}

static const struct {
  const char *settingsOptionTitle;
} settingsOptionTitle[] = {
    {"Double-After-Split:  "},
    {"Hit-17/Stand-17:  "},
};

static const int numberOfSettingsOptions =
    sizeof(settingsOptionTitle) / sizeof(settingsOptionTitle[0]);

static const char *settingsOptionState(int index, Settings *settings) {
  switch (index) {
  case 0:
    return settings->doubleAfterSplit == 'Y' ? "[  Enabled  ]"
                                             : "[  Disabled  ]";
  case 1:
    return settings->h17OrS17 == 'H' ? "[  Hit  ]" : "[  Stand  ]";
  default:
    return "";
  }
}

static void toggleSetting(int index, Settings *settings) {
  switch (index) {
  case 0:
    if (settings->doubleAfterSplit == 'Y') {
      settings->doubleAfterSplit = 'N';
    } else {
      settings->doubleAfterSplit = 'Y';
    }
    break;
  case 1:
    if (settings->h17OrS17 == 'H') {
      settings->h17OrS17 = 'S';
    } else {
      settings->h17OrS17 = 'H';
    }
    break;
  }
}

// Draw Settings Menu, similar to Main Menu
static void drawSettingsMenu(WINDOW *window, int selection,
                             Settings *settings) {
  MenuLayout layout = computeMenuLayout(window, numberOfSettingsOptions);

  werase(window);
  box(window, 0, 0);
  printCenteredText(window, layout.titleRow, "Settings");

  for (int i = 0; i < numberOfSettingsOptions; i++) {
    // Convert settingsOptionTitle and settingsOptionState to singular string
    // This lets printCenteredText to print the entire string
    char settingsOption[64];
    snprintf(settingsOption, sizeof(settingsOption), "%s%s",
             settingsOptionTitle[i].settingsOptionTitle,
             settingsOptionState(i, settings));
    if (i == selection)
      wattron(window, A_STANDOUT);
    printCenteredText(window, layout.firstOptionRow + i * layout.optionStride,
                      settingsOption);
    if (i == selection)
      wattroff(window, A_STANDOUT);
  }

  wattron(window, A_DIM);
  printCenteredText(window, layout.hintRow,
                    "j/k or up/down to move     Enter to toggle");
  if (layout.quitHintRow >= 0)
    printCenteredText(window, layout.quitHintRow, "q to save and quit");
  wattroff(window, A_DIM);

  wrefresh(window);
}

// Settings Menu Logic, similar to Main Menu
int settingsMenu(WINDOW *window, int selection, Settings *settings) {
  keypad(window, TRUE);
  int keyPress, inMenu = 1;

  while (inMenu) {
    if (!ensureUsableTerminal())
      return 0;

    fitMenuWindow(window, numberOfSettingsOptions);
    drawSettingsMenu(window, selection, settings);

    switch ((keyPress = wgetch(window))) {
    case 'q':
      inMenu = 0;
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
      if (selection < numberOfSettingsOptions - 1)
        selection++;
      break;
    case '\n':
    case '\r':
    case KEY_ENTER:
      toggleSetting(selection, settings);
      saveSettings(settings);
      break;
    }
  }

  return 1;
}
