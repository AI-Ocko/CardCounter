#ifndef SETTINGS_H
#define SETTINGS_H

#define SETTINGS_FILE "bj_settings.cfg"

typedef struct {
  int h17;
  int s17;
  int das;
  int rsa;
  int surrender;
  int numDecks;

} Settings;

void saveSettings(const Settings *s);
int loadSettings(Settings *s);
void printSettings(const Settings *s);
void settingsMenu(Settings *s);

#endif SETTINGS_H
