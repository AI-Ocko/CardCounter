#include "../include/layout.h"
#include "../include/basicStrategy.h"
#include "../include/trainer_cardDrawFunctions.h"
#include <curses.h>
#include <stdio.h>
#include <string.h>

#define COLOR_PAIR_ALERT 1

/* The player's two cards are fanned so both ranks stay readable.  Three rows
 * of offset looks best; one row is the least that still shows the lower
 * card's rank corner. */
#define PLAYER_FAN_MAX 3
#define PLAYER_FAN_MIN 1

/* How far the dealer's face-up card and its face-down partner sit either side
 * of centre, and the same for the player's pair. */
#define DEALER_SPREAD 4
#define PLAYER_SPREAD 3

static int clampInt(int value, int low, int high) {
  if (high < low)
    return low;
  if (value < low)
    return low;
  if (value > high)
    return high;
  return value;
}

/* Spend up to `amount` of the remaining spare rows and report how many were
 * actually available.  Callers spend in priority order, so the first things a
 * growing terminal buys back are the ones that matter most. */
static int takeSpare(int *spare, int amount) {
  int taken = amount < *spare ? amount : *spare;
  *spare -= taken;
  return taken;
}

TrainerLayout computeTrainerLayout(WINDOW *window) {
  TrainerLayout layout;
  int height, width;
  getmaxyx(window, height, width);

  layout.height = height;
  layout.width = width;
  layout.titleRow = 0;

  /* Row 0 holds the title on the top border and row height-1 the bottom
   * border, leaving everything between them for content. */
  int interior = height - 2;
  if (interior < 1)
    interior = 1;

  /* What every layout must pay for no matter how short the terminal is. */
  const int required = CARD_HEIGHT                    /* dealer card       */
                       + CARD_HEIGHT + PLAYER_FAN_MIN /* player pair       */
                       + 3;                           /* prompt, actions,
                                                         key hints         */
  int spare = interior - required;
  if (spare < 0)
    spare = 0;

  int fan = PLAYER_FAN_MIN + takeSpare(&spare, PLAYER_FAN_MAX - PLAYER_FAN_MIN);
  int quitHintLine = takeSpare(&spare, 1);
  int gapCards = takeSpare(&spare, 1);
  /* The result row is bought before the total row: every trainer shows a
   * result, but only the hard totals trainer has a total to show. */
  int feedbackLine = takeSpare(&spare, 1);
  int infoLine = takeSpare(&spare, 1);
  int gapText = takeSpare(&spare, 1);
  int gapActions = takeSpare(&spare, 1);
  int gapTop = takeSpare(&spare, 1);
  int gapHints = takeSpare(&spare, 1);

  /* Anything still unspent goes into the gaps between the big visual blocks,
   * so a tall terminal breathes instead of hugging the top border.  Spending
   * every spare row is what keeps the hints pinned to the bottom. */
  int *stretchable[] = {&gapCards, &gapText, &gapTop};
  for (int i = 0; spare > 0; i = (i + 1) % 3, spare--)
    (*stretchable[i])++;

  int row = 1 + gapTop;

  layout.dealerRow = row;
  row += CARD_HEIGHT + gapCards;

  layout.playerBackRow = row;
  layout.playerFrontRow = row + fan;
  row = layout.playerFrontRow + CARD_HEIGHT + gapText;

  layout.infoRow = infoLine ? row++ : -1;
  layout.promptRow = row++;
  row += gapActions;
  layout.actionsRow = row++;
  layout.feedbackRow = feedbackLine ? row++ : layout.promptRow;
  row += gapHints;
  layout.hintRow = row++;
  layout.quitHintRow = quitHintLine ? row++ : -1;

  /* Card columns: the face-down dealer card is drawn first and the face-up
   * one overlaps it from the left, which is what gives the dealer's hand its
   * depth.  The player's pair is spread the same way. */
  int centreCol = (width - CARD_WIDTH) / 2;
  int lastCardCol = width - 1 - CARD_WIDTH;

  layout.dealerFaceCol = clampInt(centreCol - DEALER_SPREAD, 1, lastCardCol);
  layout.dealerBackCol = clampInt(centreCol + DEALER_SPREAD, 1, lastCardCol);
  layout.playerFrontCol = clampInt(centreCol - PLAYER_SPREAD, 1, lastCardCol);
  layout.playerBackCol = clampInt(centreCol + PLAYER_SPREAD, 1, lastCardCol);

  return layout;
}

MenuLayout computeMenuLayout(WINDOW *window, int optionCount) {
  MenuLayout layout;
  int height, width;
  getmaxyx(window, height, width);

  layout.height = height;
  layout.width = width;
  layout.titleRow = 0;

  /* Hints sit directly above the bottom border.  The second line is the first
   * thing dropped when the window is too short to hold both. */
  if (height >= optionCount + 5) {
    layout.hintRow = height - 3;
    layout.quitHintRow = height - 2;
  } else {
    layout.hintRow = height - 2;
    layout.quitHintRow = -1;
  }

  int firstUsableRow = 1;
  int lastUsableRow = layout.hintRow - 1;
  int available = lastUsableRow - firstUsableRow + 1;
  if (available < 1)
    available = 1;

  /* A blank row between options needs 2n-1 rows, plus one clear row above and
   * below so the block does not touch the title or the hints. */
  layout.optionStride = available >= optionCount * 2 + 1 ? 2 : 1;

  int block = (optionCount - 1) * layout.optionStride + 1;
  layout.firstOptionRow = firstUsableRow + (available - block) / 2;
  if (layout.firstOptionRow < firstUsableRow)
    layout.firstOptionRow = firstUsableRow;

  return layout;
}

void fitWindow(WINDOW *window, int nlines, int ncols) {
  int screenHeight, screenWidth;
  getmaxyx(stdscr, screenHeight, screenWidth);

  nlines = clampInt(nlines, 1, screenHeight);
  ncols = clampInt(ncols, 1, screenWidth);

  int top = (screenHeight - nlines) / 2;
  int left = (screenWidth - ncols) / 2;

  int currentHeight, currentWidth, currentTop, currentLeft;
  getmaxyx(window, currentHeight, currentWidth);
  getbegyx(window, currentTop, currentLeft);
  if (currentHeight == nlines && currentWidth == ncols && currentTop == top &&
      currentLeft == left)
    return;

  /* Shrink to a single cell before moving.  Both wresize() and mvwin() fail
   * outright if the window would stick out past the screen edge at any point,
   * and a 1x1 window fits anywhere the final one will. */
  wresize(window, 1, 1);
  mvwin(window, top, left);
  wresize(window, nlines, ncols);
}

void fitMenuWindow(WINDOW *window, int optionCount) {
  int screenHeight, screenWidth;
  getmaxyx(stdscr, screenHeight, screenWidth);

  /* title, a blank row, the options at full stride, a blank row, both hint
   * lines and the bottom border. */
  int height = 1 + 1 + (optionCount * 2 - 1) + 1 + 2 + 1;
  int width = MENU_CONTENT_WIDTH + 4;

  /* Track the terminal, but stay a panel rather than filling the screen. */
  int preferredHeight = screenHeight * 2 / 3;
  int preferredWidth = screenWidth / 2;
  if (preferredHeight > height)
    height = preferredHeight;
  if (preferredWidth > width)
    width = preferredWidth;

  fitWindow(window, clampInt(height, 1, MAX_WINDOW_HEIGHT),
            clampInt(width, 1, MAX_WINDOW_WIDTH));
}

void fitTrainerWindow(WINDOW *window) {
  int screenHeight, screenWidth;
  getmaxyx(stdscr, screenHeight, screenWidth);

  /* Inset the trainer only when the terminal is comfortably larger than the
   * minimum; at the minimum every row and column is spoken for. */
  int marginY = screenHeight >= MIN_TERM_HEIGHT + 4 ? 2 : 0;
  int marginX = screenWidth >= MIN_TERM_WIDTH + 8 ? 4 : 0;

  fitWindow(window,
            clampInt(screenHeight - marginY * 2, 1, MAX_WINDOW_HEIGHT),
            clampInt(screenWidth - marginX * 2, 1, MAX_WINDOW_WIDTH));
}

void initColors(void) {
  if (!has_colors())
    return;

  start_color();
  use_default_colors();
  init_pair(COLOR_PAIR_ALERT, COLOR_RED, -1);
}

/* Red where the terminal supports it, bold everywhere else, so the offending
 * dimension still stands out on a monochrome terminal. */
static attr_t alertAttribute(void) {
  return has_colors() ? COLOR_PAIR(COLOR_PAIR_ALERT) : A_BOLD;
}

int terminalIsTooSmall(void) {
  int height, width;
  getmaxyx(stdscr, height, width);
  return height < MIN_TERM_HEIGHT || width < MIN_TERM_WIDTH;
}

/* Write onto stdscr without letting an over-long string wrap onto the row
 * below, which would scramble the rest of the message. */
static void addClipped(int row, int col, const char *text) {
  int height, width;
  getmaxyx(stdscr, height, width);
  if (row < 0 || row >= height || col < 0 || col >= width)
    return;
  mvaddnstr(row, col, text, width - col);
}

/* One centred "Width = N   Height = N" line, highlighting either value that
 * falls short of the minimum. */
static void drawDimensions(int row, int reportedWidth, int reportedHeight,
                           int widthIsTooSmall, int heightIsTooSmall) {
  char widthText[32], heightText[32];
  const char *separator = "   ";

  snprintf(widthText, sizeof(widthText), "Width = %d", reportedWidth);
  snprintf(heightText, sizeof(heightText), "Height = %d", reportedHeight);

  int screenHeight, screenWidth;
  getmaxyx(stdscr, screenHeight, screenWidth);
  (void)screenHeight;

  int total = (int)strlen(widthText) + (int)strlen(separator) +
              (int)strlen(heightText);
  int col = (screenWidth - total) / 2;
  if (col < 0)
    col = 0;

  attr_t alert = alertAttribute();

  if (widthIsTooSmall)
    attron(alert);
  addClipped(row, col, widthText);
  if (widthIsTooSmall)
    attroff(alert);
  col += (int)strlen(widthText);

  addClipped(row, col, separator);
  col += (int)strlen(separator);

  if (heightIsTooSmall)
    attron(alert);
  addClipped(row, col, heightText);
  if (heightIsTooSmall)
    attroff(alert);
}

void drawTerminalTooSmall(void) {
  int height, width;
  getmaxyx(stdscr, height, width);

  erase();

  /* Centre the four-line message, but never start above the first row. */
  int row = height / 2 - 2;
  if (row < 0)
    row = 0;

  printCenteredText(stdscr, row, "Terminal size too small:");
  drawDimensions(row + 1, width, height, width < MIN_TERM_WIDTH,
                 height < MIN_TERM_HEIGHT);
  printCenteredText(stdscr, row + 2, "Needed for current config:");
  drawDimensions(row + 3, MIN_TERM_WIDTH, MIN_TERM_HEIGHT, 0, 0);

  refresh();
}

void handleResize(void) {
  clear();
  refresh();
}

int ensureUsableTerminal(void) {
  int drewMessage = 0;

  while (terminalIsTooSmall()) {
    drawTerminalTooSmall();
    drewMessage = 1;

    /* Any key re-measures the terminal; KEY_RESIZE is simply the one that
     * usually arrives first. */
    int keyPress = wgetch(stdscr);
    if (keyPress == 'q' || keyPress == 'Q')
      return 0;
  }

  if (drewMessage)
    handleResize();

  return 1;
}
