#ifndef LAYOUT_H
#define LAYOUT_H

#include <curses.h>

/* Smallest terminal the interface can be drawn in.
 *
 * The height comes from the tightest trainer layout that still shows every
 * element: the title row, a 9-row dealer card, a player pair fanned by a
 * single row (10 rows), then one row each for the prompt, the action list and
 * the key hints, all inside a box.  1 + 9 + 10 + 3 + 1 == 24.
 *
 * The width comes from the widest string the interface ever draws,
 * "h/l or left/right to move     Enter to select" (45 columns), plus the box
 * and a column of padding on either side. */
#define MIN_TERM_HEIGHT 24
#define MIN_TERM_WIDTH 50

/* Windows stop growing past this.  Without a cap, a maximised terminal
 * strands the cards in the middle of an enormous empty box. */
#define MAX_WINDOW_HEIGHT 40
#define MAX_WINDOW_WIDTH 120

/* Widest line either menu draws, used to size the menu panel. */
#define MENU_CONTENT_WIDTH 46

/* Every row and column a trainer draws to, derived from the window's current
 * size.  Rows marked optional are -1 when the terminal is too short to hold
 * them; the trainers fold that content elsewhere rather than dropping it. */
typedef struct {
  int height, width;

  int titleRow;

  int dealerRow;
  int dealerFaceCol;
  int dealerBackCol;

  int playerBackRow, playerFrontRow;
  int playerBackCol, playerFrontCol;

  int infoRow; /* optional: -1 when there is no row to spare */
  int promptRow;
  int actionsRow;
  int feedbackRow;  /* equals promptRow when it has no row of its own */
  int hintRow;
  int quitHintRow; /* optional: -1 when there is no row to spare */
} TrainerLayout;

/* Row positions for a vertically centred list of menu options. */
typedef struct {
  int height, width;

  int titleRow;
  int firstOptionRow;
  int optionStride; /* 2 rows per option when there is room, otherwise 1 */
  int hintRow;
  int quitHintRow; /* optional: -1 when there is no row to spare */
} MenuLayout;

TrainerLayout computeTrainerLayout(WINDOW *window);

/* The window must be at least (optionCount * 2 + 5) rows tall: the title, a
 * blank row, the options at full stride, a blank row, both hints and the
 * bottom border.  fitMenuWindow() never returns anything smaller. */
MenuLayout computeMenuLayout(WINDOW *window, int optionCount);

/* Resize and recentre an existing window for the terminal's current size.
 * Doing nothing when the geometry already matches keeps these cheap enough to
 * call once per keypress. */
void fitWindow(WINDOW *window, int nlines, int ncols);
void fitMenuWindow(WINDOW *window, int optionCount);
void fitTrainerWindow(WINDOW *window);

void initColors(void);

int terminalIsTooSmall(void);
void drawTerminalTooSmall(void);

/* Hold the "terminal too small" screen until the terminal is big enough
 * again.  Returns 1 when the interface can be drawn, 0 when the user pressed
 * q to quit from that screen. */
int ensureUsableTerminal(void);

/* Discard the previous geometry's screen contents after a KEY_RESIZE so stale
 * glyphs do not survive underneath the redrawn windows. */
void handleResize(void);

#endif
