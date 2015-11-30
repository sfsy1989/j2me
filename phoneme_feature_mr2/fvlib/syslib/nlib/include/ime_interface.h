#include <QtGui>

void ime_init();
void ime_uninit();
extern "C" {
int ime_open();
int ime_close();
}
void ime_focusMovUp();
void ime_focusMovDown();
void ime_focusMovLeft();
void ime_focusMovRight();
QKeyEvent *ime_transKey();

