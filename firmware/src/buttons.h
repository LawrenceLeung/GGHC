#define nBUTTONS 5

const int buttonDebounceTime = 40;

typedef struct button {
  int pressed;
  uint32_t debounceTime;
} button_t;

extern button_t buttons[5];

void ButtonsInit();
void ReadButtons();