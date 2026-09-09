#include "../include/basicStrategy.h"
#include "../include/layout.h"
#include "../include/trainer_cardDrawFunctions.h"
#include <curses.h>
#include <stdio.h>
#include <string.h>

// Dealer
// UpCard-----A------2------3------4------5------6------7------8------9-----10
Action PairSplitting[10][10] = {
    /* A,A */ {Y, Y, Y, Y, Y, Y, Y, Y, Y, Y},
    /* 2,2 */ {N, YN, YN, Y, Y, Y, Y, N, N, N},
    /* 3,3 */ {N, YN, YN, Y, Y, Y, Y, N, N, N},
    /* 4,4 */ {N, N, N, N, YN, YN, N, N, N, N},
    /* 5,5 */ {N, N, N, N, N, N, N, N, N, N},
    /* 6,6 */ {N, YN, Y, Y, Y, Y, N, N, N, N},
    /* 7,7 */ {N, Y, Y, Y, Y, Y, Y, N, N, N},
    /* 8,8 */ {Y, Y, Y, Y, Y, Y, Y, Y, Y, Y},
    /* 9,9 */ {N, Y, Y, Y, Y, Y, N, Y, Y, N},
    /* 10,10 */ {N, N, N, N, N, N, N, N, N, N},
};

int surrender[3][10] = {
    /* 14 */ {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    /* 15 */ {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    /* 16 */ {0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
};

static const char *pairSplittingActions[] = {
    "Split",
    "Don't Split",
};

static const int numberOfUserActions =
    sizeof(pairSplittingActions) / sizeof(pairSplittingActions[0]);

static void drawTrainerWindow(WINDOW *window, int selection, Card dealerUpCard,
                              Card playerCard, const char *feedback) {
  TrainerLayout layout = computeTrainerLayout(window);

  werase(window);
  box(window, 0, 0);

  // Title
  printCenteredText(window, layout.titleRow, "Pair Splitting Trainer");

  // Draw dealer upcard
  drawCardBack(window, layout.dealerRow, layout.dealerBackCol);
  drawCardTemplate(window, layout.dealerRow, layout.dealerFaceCol,
                   dealerUpCard);

  // Draw player pair
  drawCardTemplate(window, layout.playerBackRow, layout.playerBackCol,
                   playerCard);
  drawCardTemplate(window, layout.playerFrontRow, layout.playerFrontCol,
                   playerCard);

  // Prompt, or the result of the last answer when there is no row to spare
  // for it of its own
  if (feedback != NULL && layout.feedbackRow == layout.promptRow) {
    printCenteredText(window, layout.promptRow, feedback);
  } else {
    printCenteredText(window, layout.promptRow, "Do you split?");
    if (feedback != NULL)
      printCenteredText(window, layout.feedbackRow, feedback);
  }

  // Print User Actions
  //
  // Get starting column based on ((width of window)-(length of multiple
  // strings))/2 for centered position
  int totalWidth = 0;
  int textSpacing = 4;
  for (int i = 0; i < numberOfUserActions; i++) {
    totalWidth += (int)strlen(pairSplittingActions[i]);
    if (i > 0)
      totalWidth += textSpacing;
  }
  int col = (layout.width - totalWidth) / 2;
  if (col < 0)
    col = 0;

  for (int i = 0; i < numberOfUserActions; i++) {
    if (i == selection)
      wattron(window, A_STANDOUT);
    mvwaddnstr(window, layout.actionsRow, col, pairSplittingActions[i],
               layout.width - col);
    if (i == selection)
      wattroff(window, A_STANDOUT);
    col += (int)strlen(pairSplittingActions[i]) +
           textSpacing; /* increment starting column for next loop*/
  }

  // Print hints
  wattron(window, A_DIM);
  printCenteredText(window, layout.hintRow,
                    "h/l or left/right to move     Enter to select");
  if (layout.quitHintRow >= 0)
    printCenteredText(window, layout.quitHintRow, "q to quit");
  wattroff(window, A_DIM);

  wrefresh(window);
}

int pairSplittingTrainer(WINDOW *window, Score *score, Settings *settings) {

  int selection = 0, keyPress, running = 1, keepApplicationRunning = 1;

  /* While a result is on screen the trainer waits for the player to
   * acknowledge it before dealing again.  Holding it as state rather than
   * blocking on a nested wgetch() is what lets a resize redraw the board with
   * the result still on it. */
  int awaitingAcknowledgement = 0;
  char feedback[64] = "";

  Card dealerUpCard = generateDealerUpCard();
  Card playerCard = generatePlayerCard();
  keypad(window, TRUE);

  while (running) {
    if (!ensureUsableTerminal()) {
      keepApplicationRunning = 0;
      break;
    }

    fitTrainerWindow(window);
    drawTrainerWindow(window, selection, dealerUpCard, playerCard,
                      awaitingAcknowledgement ? feedback : NULL);

    keyPress = wgetch(window);

    if (keyPress == 'q') {
      running = 0;
      continue;
    }
    if (keyPress == KEY_RESIZE) {
      handleResize();
      continue;
    }

    if (awaitingAcknowledgement) {
      // Any other key clears the result and deals the next hand
      awaitingAcknowledgement = 0;
      dealerUpCard = generateDealerUpCard();
      playerCard = generatePlayerCard();
      selection = 0;
      continue;
    }

    switch (keyPress) {
    case 'h':
    case KEY_LEFT:
      if (selection > 0)
        selection--;
      break;
    case 'l':
    case KEY_RIGHT:
      if (selection < numberOfUserActions - 1)
        selection++;
      break;
    case '\n':
    case '\r':
    case KEY_ENTER: {

      Action correctAnswer = PairSplitting[playerCard.rank][dealerUpCard.rank];

      int correctOption;
      if (correctAnswer == Y) {
        correctOption = 0;
      } else if (correctAnswer == N) {
        correctOption = 1;
      } else {
        correctOption = settings->doubleAfterSplit == 'Y' ? 0 : 1;
      }

      score->total++;
      if (correctOption == selection) {
        score->correct++;
        snprintf(feedback, sizeof(feedback), "Correct!");
      } else {
        snprintf(feedback, sizeof(feedback), "Incorrect. The answer is: %s",
                 pairSplittingActions[correctOption]);
      }
      awaitingAcknowledgement = 1;
      break;
    }
    }
  }

  werase(window);
  wrefresh(window);

  return keepApplicationRunning;
}
