#include "hw_driver.h"
#include "stc15f2k60s2.h"
#include "types.h"

void Die() {
  while (1) {
    DelayMs(100);
    if (WasButtonPressed(kRstBtn) > 0) {
      break;
    }
  }
}

int8 snake[5][2];

enum SnakeDirection {
  kRight = 0,
  kDown = 1,
  kLeft = 2,
  kUp = 3,
};

uint8 dir;

void main(void) {
  int i;
  InitHw();

TEST:

  snake[0][0] = 4;
  snake[0][1] = 0;
  snake[1][0] = 3;
  snake[1][1] = 0;
  snake[2][0] = 2;
  snake[2][1] = 0;
  snake[3][0] = 1;
  snake[3][1] = 0;
  snake[4][0] = 0;
  snake[4][1] = 0;
  dir = 0;  // right

  // Draw the snake
  for (i = 0; i < 5; ++i) {
    SetPixel(/*row=*/snake[i][1], /*column=*/snake[i][0]);
  }

  while (1) {
    DelayMs(200);
    ClearPixel(/*row=*/snake[4][1], /*column=*/snake[4][0]);
    // Move the snake
    for (i = 4; i >= 1; --i) {
      snake[i][0] = snake[i - 1][0];
      snake[i][1] = snake[i - 1][1];
    }
    if (WasButtonPressed(kRightBtn) > 0) {
      dir = kRight;
    } else if (WasButtonPressed(kDownBtn) > 0) {
      dir = kDown;
    } else if (WasButtonPressed(kLeftBtn) > 0) {
      dir = kLeft;
    } else if (WasButtonPressed(kUpBtn) > 0) {
      dir = kUp;
    }

    // Check for wall hit
    switch (dir) {
      case 0:
        if (snake[0][0] == 7) {
          Die();
          goto TEST;
        }
        break;
      case 1:
        if (snake[0][1] == 15) {
          Die();
          goto TEST;
        }
        break;
      case 2:
        if (snake[0][0] == 0) {
          Die();
          goto TEST;
        }
        break;
      case 3:
        if (snake[0][1] == 0) {
          Die();
          goto TEST;
        }
        break;
    }

    // Move the head
    switch (dir) {
      case 0:
        snake[0][0]++;
        break;
      case 1:
        snake[0][1]++;
        break;
      case 2:
        snake[0][0]--;
        break;
      case 3:
        snake[0][1]--;
        break;
    }
    SetPixel(/*row=*/snake[0][1], /*column=*/snake[0][0]);
  }
}

// For some reason if the interrupt method is declared in hw_driver.c
// it doesn't work. Need to declare it here and invoke the other one.
void OnTickInt() __interrupt(1) { OnTick(); }
