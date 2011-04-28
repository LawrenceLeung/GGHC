#define nBUTTONS 5


#ifndef  __BUTTONS_H
#define  __BUTTONS_H

extern const int buttonDebounceTime;

typedef struct button {
  int pressed;
  uint32_t debounceTime;
} button_t;

extern button_t buttons[nBUTTONS];

void ButtonsInit(void);
void ButtonsRead(void);

#endif
