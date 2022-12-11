#ifndef _MY_HW_DRIVER_
#define _MY_HW_DRIVER_

#include "types.h"

#ifdef __GNUC__
// Defines to prevent the GCC compiler from complaining
#define __xdata
#define __interrupt(x)
#else
#define __interrupt(x) __interrupt x
#endif

enum BtnIndex {
  kDownBtn = 0,
  kUpBtn = 1,
  kLeftBtn = 2,
  kRightBtn = 3,
  kOkBtn = 4,
  kRstBtn = 5,
  kBtnCount
};

// A lightweight version that initializes the display
// and buttons for manual operation. Does not enable
// the interrupts.
void InitHwLite();

// Initialize the hardware, the display buffer and the
// interrupt handler.
void InitHw();

void SetPixelValue(int8 row, int8 col, int8 v);
int8 GetPixelValue(int8 row, int8 col);

void SetPixel(int8 row, int8 col);
void ClearPixel(int8 row, int8 col);

int8 WasButtonPressed(int8 btn_index);
int8 IsButtonDown(int8 btn_index);

void DelayMs(int ms);

// Sets the `row` pin the specified value `v`. 
// Rows are sources, 1 - ON, 0 - OFF
void SetRowPin(int row, int v);

// Sets the `col` pin the specified value `v`. 
// Columns are sinks, 1 - OFF, 0 - ON
void SetColPin(int col, int v);

void ClearScreen();

void OnTick();

#endif  // _MY_HW_DRIVER_
