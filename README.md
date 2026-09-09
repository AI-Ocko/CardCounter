# Basic Strategy Trainer

## What is This?

This is a personal project; a BlackJack Basic Strategy Trainer fully written in
C.
It uses the ncurses library for a feature rich TUI trainer application.

### How do I run it?

Simply go to the root of the project, and run `make all`, then run `./bin/basicStrategyTrainer`

The interface scales to fit the terminal and redraws itself when the terminal
is resized. It needs at least **50 columns by 24 rows**; below that it shows
the required size and waits until the terminal is big enough.

### TODO

1. Trainer Fixes

- [ ] Text printout of dealer upcard and player hand on the left side of the screen
- [ ] Score tracking, both in-game, and global saving, like high-scores
- [ ] Make the trainers timed, like the HellDivers minigame
