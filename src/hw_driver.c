#include "hw_driver.h"
#include "stc15f2k60s2.h"
#include "types.h"

#define ROW_COUNT 16
#define COL_COUNT 8

int col = 0;
// Marked as "xdata" to fit in memory, we can switch to a bit-per-pixel
// mode that it may affect the interrupt performance.
// uint8 d[ROW_COUNT][COL_COUNT];
uint8 __xdata d[ROW_COUNT][COL_COUNT];

uint16 tick;
uint8 btn_state[kBtnCount];
// btn_event_count is accessed from both the interrupt handler and the main thread.
// Marked as volatile to prevent the compiler from optimizing it away.
volatile uint8 btn_event_count[kBtnCount];
uint16 btn_tick[kBtnCount];

void OffAll() {
  P0 = 0;
  P1 = 0x80;
  P2 = 0x7E;
  P3 = 0x20;
  P4 = 0x16;
  P5 = 0;
}

void SetRowPin(int row, int v) {
  switch (row) {
    case 0: P20 = v; break;
    case 1: P45 = v; break;
    case 2: P04 = v; break;
    case 3: P27 = v; break;
    case 4: P00 = v; break;
    case 5: P03 = v; break;
    case 6: P01 = v; break;
    case 7: P02 = v; break;
    case 8: P05 = v; break;
    case 9: P07 = v; break;
    case 10: P13 = v; break;
    case 11: P06 = v; break;
    case 12: P14 = v; break;
    case 13: P12 = v; break;
    case 14: P11 = v; break;
    case 15: P54 = v; break;

    // The digits have to be driven in reverse.
    case 16: P23 = v; break;
    case 17: P25 = v; break;
    case 18: P41 = v; break;
  }
}

void SetColPin(int col, int v) {
  switch (col) {
    case 0: P24 = v; break;
    case 1: P26 = v; break;
    case 2: P44 = v; break;
    case 3: P21 = v; break;
    case 4: P42 = v; break;
    case 5: P22 = v; break;
    case 6: P35 = v; break;
    case 7: P17 = v; break;
  }
}

void ResetTimer0(void) {
  TH0 = 0xEF;
  TL0 = 0xCD;         // Set to 4.5 milliseconds  
}

void ConfigureTimer0(void) {
  ResetTimer0();          // Load timer0 start value
  TR0 = 1;                // Start Timer 0
  ET0 = 1;                // Enable Timer 0 overflow interrupt
}

void EnableInterrupt(void) {
  EA = 1;
}

void InitHwLite() {
  // Turn off all the segments;
  P0 = 0;
  P1 = 0x80;
  P2 = 0x7E;
  P3 = 0x20;
  P4 = 0x16;
  P5 = 0;

  // Set the rows to have push-pull mode, 20ma current source
  P0M0 = 0xFF;
  P1M0 = 0x1E;
  P2M0 = 0x81;
  P4M0 = 0x20;
  P5M0 = 0x10;

  // Drive buttons up.
  P30 = 1;  // OK
  P31 = 1;  // Reset
  P32 = 1;  // Up
  P33 = 1;  // Right
  P36 = 1;  // Down
  P37 = 1;  // Left
}

void InitHw() {
  int8 i, j;

  OffAll();
  col = 0;

  // Set the rows to have push-pull mode, 20ma
  P0M0 = 0xFF;
  P1M0 = 0x1E;
  P2M0 = 0x81;
  P4M0 = 0x20;
  P5M0 = 0x10;

  // Drive buttons up.
  P30 = 1;  // OK
  P31 = 1;  // Reset
  P32 = 1;  // Up
  P33 = 1;  // Right
  P36 = 1;  // Down
  P37 = 1;  // Left

  for (i = 0; i < ROW_COUNT; ++i) {
    for (j = 0; j < COL_COUNT; ++j) {
      d[i][j] = 0;
    }
  }

	tick = 0;
	
	for (i = 0; i < kBtnCount; ++i) {
		btn_state[i] = 0;
    btn_event_count[i] = 0;
		btn_tick[i] = 0;
	}

  // Or "Demo Program of 16-bit Auto-Reload Timer/Counter 0 (C and ASM)"
  // AUXR = 0x80; //T0 in 1T mode ?
  // TMOD = 0x04; // T0 in 16-bit auto-relaod mode?
  
  // Use Timer 0 in 16-bit Timer operating mode  
  TMOD = 0x01;     

  ConfigureTimer0();  // Load the value, start and enable "function call" when overflow
  EnableInterrupt();  // Enable
}

void HandleButtonState(int8 btn, int8 is_pressed) {
  if (is_pressed) {  // Down
		// We want to make it so that the first event is issued right
		// as soon as we find the button pressed, but if the button
		// is held down the second even should come after a delay,
		// and the third and subsequent events need to be raised after
		// a smaller delay.
		if (btn_state[btn] == 0) {
			// This is the first time we see the button pressed
			// issue a press even right away and start the timer.
			++btn_event_count[btn];
			btn_tick[btn] = tick;
		} else {
			// The button is being held, see if enough time has passed 
			// to issue another event.
			if ((tick - btn_tick[btn]) > 200) {
				++btn_event_count[btn];  // Overflow??
				// Arm the timer for another even, we want third and 
				// subsequent events to be issued more often than the 
				// second one.
				btn_tick[btn] = tick - 150;
			}
		}
		if (btn_state[btn] < 5) {
			++btn_state[btn];
		}
  } else {
		if (btn_state[btn] > 0) {
			--btn_state[btn];
		}
	}
}

void Display() {
  int8 i;
	++tick;

  // Turn OFF whatever columns was ON.
  SetColPin(col, 1);

  // Turn off all the rows.
  for (i = 0; i < ROW_COUNT; ++i) {
    SetRowPin(i, 0);
  }
  ++col;
  if (col == COL_COUNT) { 
    col = 0; 
  }

  for (i = 0; i < ROW_COUNT; ++i) {
    if (d[i][col] == 1) {
      SetRowPin(i, 1);
    }
  }
	
  // Turn ON col.
  SetColPin(col, 0);	
}

void HandleButtons() {
  HandleButtonState(kDownBtn, P36 == 0);
  HandleButtonState(kUpBtn, P32 == 0);
  HandleButtonState(kRightBtn, P33 == 0);
  HandleButtonState(kLeftBtn, P37 == 0);
  HandleButtonState(kOkBtn, P30 == 0);
  HandleButtonState(kRstBtn, P31 == 0);
}

int8 WasButtonPressed(int8 btn_index) {
  // It would be nice to disable the interrupts here to avoid the
  // interrupt method changing btn_event_count.
  if (btn_event_count[btn_index] > 0) {
    --btn_event_count[btn_index];
    return 1;
  }
  return 0;
}

int8 IsButtonDown(int8 btn_index) {
  return btn_state[btn_index];
}

void SetPixelValue(int8 row, int8 col, int8 v) {
  d[row][col] = v;
}

int8 GetPixelValue(int8 row, int8 col) {
  return d[row][col];
}

void SetPixel(int8 row, int8 col) {
  d[row][col] = 1;
}

void ClearPixel(int8 row, int8 col) {
  d[row][col] = 0;
}

void Delay1ms() {
  unsigned char i, j, k;

  i = 1;
  j = 15;
  k = 44;
  do {
    do {
      while (--k) {};
    } while (--j);
  } while (--i);
}

void Delay10ms() {
  unsigned char i, j, k;

  i = 1;
  j = 153;
  k = 44;
  do {
    do {
      while (--k) {};
    } while (--j);
  } while (--i);
}

void Delay100ms() {
  unsigned char i, j, k;

  i = 10;
  j = 153;
  k = 44;
  do {
    do {
      while (--k) {};
    } while (--j);
  } while (--i);
}

// Designed to run at 27.000MHz
void DelayMs(int ms) {
  while (ms > 100) {
    ms -= 100;
    Delay100ms();
  }
  while (ms > 10) {
    ms -= 10;
    Delay10ms();
  }
  while (ms > 1) {
    ms -= 1;
    Delay1ms();
  }
}

// Runs each time timer 0 interrupt is generated.
void OnTick() {
  Display();
  HandleButtons();
  ResetTimer0();
}
