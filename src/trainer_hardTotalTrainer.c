#include "../include/basicStrategy.h"
#include "../include/layout.h"
#include "../include/trainer_cardDrawFunctions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Dealer
// UpCard-----A------2------3------4------5------6------7------8------9-----10
Action HardTotalsH17[10][10] = {
    /*  8 */ {H, H, H, H, H, H, H, H, H, H},
    /*  9 */ {H, H, D, D, D, D, H, H, H, H},
    /* 10 */ {H, D, D, D, D, D, D, D, D, H},
    /* 11 */ {D, D, D, D, D, D, D, D, D, D},
    /* 12 */ {H, H, H, S, S, S, H, H, H, H},
    /* 13 */ {H, S, S, S, S, S, H, H, H, H},
    /* 14 */ {H, S, S, S, S, S, H, H, H, H},
    /* 15 */ {H, S, S, S, S, S, H, H, H, H},
    /* 16 */ {H, S, S, S, S, S, H, H, H, H},
    /* 17 */ {S, S, S, S, S, S, S, S, S, S}};

Action HardTotalsS17[10][10] = {
    /*  8 */ {H, H, H, H, H, H, H, H, H, H},
    /*  9 */ {H, H, D, D, D, D, H, H, H, H},
    /* 10 */ {H, D, D, D, D, D, D, D, D, H},
    /* 11 */ {H, D, D, D, D, D, D, D, D, D},
    /* 12 */ {H, H, H, S, S, S, H, H, H, H},
    /* 13 */ {H, S, S, S, S, S, H, H, H, H},
    /* 14 */ {H, S, S, S, S, S, H, H, H, H},
    /* 15 */ {H, S, S, S, S, S, H, H, H, H},
    /* 16 */ {H, S, S, S, S, S, H, H, H, H},
    /* 17 */ {S, S, S, S, S, S, S, S, S, S}};

static const char *hardTotalActions[] = {
    "Hit",
    "Stand",
    "Double",
};

static const int numberOfUserActions =
    sizeof(hardTotalActions) / sizeof(hardTotalActions[0]);

Card generatePlayerCardOne() {
  Card playerCardOne;

  playerCardOne.rank = rand() % 9 + 1;
  playerCardOne.suit = rand() % SUIT_COUNT;

  return playerCardOne;
}
Card generatePlayerCardTwo(Card playerCardOne) {
  Card playerCardTwo;

  do {
    if (playerCardOne.rank == 9) {
      playerCardTwo.rank = rand() % 6 + 1;
    } else if (playerCardOne.rank == 8) {
      playerCardTwo.rank = rand() % 7 + 1;
    } else if (playerCardOne.rank == 7) {
      playerCardTwo.rank = rand() % 8 + 1;
    } else {
      playerCardTwo.rank = rand() % 9 + 1;
    }
  } while (playerCardTwo.rank == playerCardOne.rank ||
           (playerCardOne.rank + 1) + (playerCardTwo.rank + 1) <= 7);
  playerCardTwo.suit = rand() % SUIT_COUNT;

  return playerCardTwo;
}

static void drawTrainerWindow(WINDOW *window, int selection, int playerTotal,
                              Card dealerUpCard, Card playerCardOne,
                              Card playerCardTwo, const char *feedback) {
  TrainerLayout layout = computeTrainerLayout(window);

  werase(window);
  box(window, 0, 0);

  // Title
  printCenteredText(window, layout.titleRow, "Hard Total Trainer");

  // Draw dealer upcard
  drawCardBack(window, layout.dealerRow, layout.dealerBackCol);
  drawCardTemplate(window, layout.dealerRow, layout.dealerFaceCol,
                   dealerUpCard);

  // Draw player hard total
  drawCardTemplate(window, layout.playerBackRow, layout.playerBackCol,
                   playerCardTwo);
  drawCardTemplate(window, layout.playerFrontRow, layout.playerFrontCol,
                   playerCardOne);

  // The player's total gets its own line when there is one, and is folded
  // into the prompt when there is not
  char totalText[64];
  char promptText[96];
  const char *prompt = "Hit, Stand, or Double?";

  if (layout.infoRow >= 0) {
    snprintf(totalText, sizeof(totalText), "You have a total of %d.",
             playerTotal);
    printCenteredText(window, layout.infoRow, totalText);
  } else {
    snprintf(promptText, sizeof(promptText), "Total %d - Hit, Stand, or Double?",
             playerTotal);
    prompt = promptText;
  }

  // Prompt, or the result of the last answer when there is no row to spare
  // for it of its own
  if (feedback != NULL && layout.feedbackRow == layout.promptRow) {
    printCenteredText(window, layout.promptRow, feedback);
  } else {
    printCenteredText(window, layout.promptRow, prompt);
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
    totalWidth += (int)strlen(hardTotalActions[i]);
    if (i > 0)
      totalWidth += textSpacing;
  }
  int col = (layout.width - totalWidth) / 2;
  if (col < 0)
    col = 0;

  for (int i = 0; i < numberOfUserActions; i++) {
    if (i == selection)
      wattron(window, A_STANDOUT);
    mvwaddnstr(window, layout.actionsRow, col, hardTotalActions[i],
               layout.width - col);
    if (i == selection)
      wattroff(window, A_STANDOUT);
    col += (int)strlen(hardTotalActions[i]) +
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

int hardTotalTrainer(WINDOW *window, Score *score, Settings *settings) {
  int selection = 0, keyPress, running = 1, keepApplicationRunning = 1;

  /* While a result is on screen the trainer waits for the player to
   * acknowledge it before dealing again.  Holding it as state rather than
   * blocking on a nested wgetch() is what lets a resize redraw the board with
   * the result still on it. */
  int awaitingAcknowledgement = 0;
  char feedback[64] = "";

  Card dealerUpCard = generateDealerUpCard();
  Card playerCardOne = generatePlayerCardOne();
  Card playerCardTwo = generatePlayerCardTwo(playerCardOne);
  int playerTotal = ((playerCardOne.rank + 1) + (playerCardTwo.rank + 1));
  keypad(window, TRUE);

  while (running) {
    if (!ensureUsableTerminal()) {
      keepApplicationRunning = 0;
      break;
    }

    fitTrainerWindow(window);
    drawTrainerWindow(window, selection, playerTotal, dealerUpCard,
                      playerCardOne, playerCardTwo,
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
      playerCardOne = generatePlayerCardOne();
      playerCardTwo = generatePlayerCardTwo(playerCardOne);
      playerTotal = ((playerCardOne.rank + 1) + (playerCardTwo.rank + 1));
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
              ? HardTotalsH17[playerTotal - 8][dealerUpCard.rank]
              : HardTotalsS17[playerTotal - 8][dealerUpCard.rank];

      int correctOption;
      switch (correctAnswer) {
      case H:
        correctOption = 0;
        break;
      case S:
        correctOption = 1;
        break;
      case D:
        correctOption = 2;
        break;
      default:
        correctOption = 1;
        break;
      }

      score->total++;
      if (correctOption == selection) {
        score->correct++;
        snprintf(feedback, sizeof(feedback), "Correct!");
      } else {
        snprintf(feedback, sizeof(feedback), "Incorrect. The answer is: %s",
                 hardTotalActions[correctOption]);
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
