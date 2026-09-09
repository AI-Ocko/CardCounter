#include "../include/basicStrategy.h"
#include "../include/layout.h"
#include "../include/trainer_cardDrawFunctions.h"
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Dealer
// UpCard--------------A------2------3------4------5------6------7------8------9-----10
Action SoftTotalsH17[8][10] = {
    /* A,2 */ {H, H, H, H, D, D, H, H, H, H},
    /* A,3 */ {H, H, H, H, D, D, H, H, H, H},
    /* A,4 */ {H, H, H, D, D, D, H, H, H, H},
    /* A,5 */ {H, H, H, D, D, D, H, H, H, H},
    /* A,6 */ {H, H, D, D, D, D, H, H, H, H},
    /* A,7 */ {H, Ds, Ds, Ds, Ds, Ds, S, S, H, H},
    /* A,8 */ {S, S, S, S, S, Ds, S, S, S, S},
    /* A,9 */ {S, S, S, S, S, S, S, S, S, S}};

Action SoftTotalsS17[8][10] = {
    /* A,2 */ {H, H, H, H, D, D, H, H, H, H},
    /* A,3 */ {H, H, H, H, D, D, H, H, H, H},
    /* A,4 */ {H, H, H, D, D, D, H, H, H, H},
    /* A,5 */ {H, H, H, D, D, D, H, H, H, H},
    /* A,6 */ {H, H, D, D, D, D, H, H, H, H},
    /* A,7 */ {H, S, Ds, Ds, Ds, Ds, S, S, H, H},
    /* A,8 */ {S, S, S, S, S, S, S, S, S, S},
    /* A,9 */ {S, S, S, S, S, S, S, S, S, S}};

static const char *softTotalActions[] = {
    "Hit",
    "Stand",
    "Double",
};

static const int numberOfUserActions =
    sizeof(softTotalActions) / sizeof(softTotalActions[0]);

Card generateAceCard() {
  Card aceCard;

  aceCard.rank = 0;
  aceCard.suit = rand() % SUIT_COUNT;

  return aceCard;
}

static void drawTrainerWindow(WINDOW *window, int selection, Card dealerUpCard,
                              Card playerCard, Card aceCard,
                              const char *feedback) {
  TrainerLayout layout = computeTrainerLayout(window);

  werase(window);
  box(window, 0, 0);

  // Title
  printCenteredText(window, layout.titleRow, "Soft Total Trainer");

  // Draw dealer upcard
  drawCardBack(window, layout.dealerRow, layout.dealerBackCol);
  drawCardTemplate(window, layout.dealerRow, layout.dealerFaceCol,
                   dealerUpCard);

  // Draw player soft total
  drawCardTemplate(window, layout.playerBackRow, layout.playerBackCol,
                   playerCard);
  drawCardTemplate(window, layout.playerFrontRow, layout.playerFrontCol,
                   aceCard);

  // Prompt, or the result of the last answer when there is no row to spare
  // for it of its own
  if (feedback != NULL && layout.feedbackRow == layout.promptRow) {
    printCenteredText(window, layout.promptRow, feedback);
  } else {
    printCenteredText(window, layout.promptRow, "Hit, Stand, or Double?");
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
    totalWidth += (int)strlen(softTotalActions[i]);
    if (i > 0)
      totalWidth += textSpacing;
  }
  int col = (layout.width - totalWidth) / 2;
  if (col < 0)
    col = 0;

  for (int i = 0; i < numberOfUserActions; i++) {
    if (i == selection)
      wattron(window, A_STANDOUT);
    mvwaddnstr(window, layout.actionsRow, col, softTotalActions[i],
               layout.width - col);
    if (i == selection)
      wattroff(window, A_STANDOUT);
    col += (int)strlen(softTotalActions[i]) +
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

int softTotalTrainer(WINDOW *window, Score *score, Settings *settings) {
  int selection = 0, keyPress, running = 1, keepApplicationRunning = 1;

  /* While a result is on screen the trainer waits for the player to
   * acknowledge it before dealing again.  Holding it as state rather than
   * blocking on a nested wgetch() is what lets a resize redraw the board with
   * the result still on it. */
  int awaitingAcknowledgement = 0;
  char feedback[64] = "";

  Card dealerUpCard = generateDealerUpCard();
  Card playerCard = generatePlayerCard();
  Card aceCard = generateAceCard();
  keypad(window, TRUE);

  while (running) {
    if (!ensureUsableTerminal()) {
      keepApplicationRunning = 0;
      break;
    }

    fitTrainerWindow(window);
    drawTrainerWindow(window, selection, dealerUpCard, playerCard, aceCard,
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

      Action correctAnswer =
          settings->h17OrS17 == 'H'
              ? SoftTotalsH17[playerCard.rank][dealerUpCard.rank]
              : SoftTotalsS17[playerCard.rank][dealerUpCard.rank];

      int correctOption;
      if (correctAnswer == H) {
        correctOption = 0;
      } else if (correctAnswer == S) {
        correctOption = 1;
      } else if (correctAnswer == D) {
        correctOption = 2;
      } else {
        correctOption = settings->h17OrS17 == 'H' ? 0 : 1;
      }

      score->total++;
      if (correctOption == selection) {
        score->correct++;
        snprintf(feedback, sizeof(feedback), "Correct!");
      } else {
        snprintf(feedback, sizeof(feedback), "Incorrect. The answer is: %s",
                 softTotalActions[correctOption]);
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
