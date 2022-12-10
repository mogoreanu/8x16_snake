#include "hw_driver.h"
#include "stc15f2k60s2.h"
#include "types.h"
#include <stdio.h>       
#include <stdlib.h>  

void Die(length) {
  while (1) {
    DelayMs(100);
    if (WasButtonPressed(kRstBtn) > 0) {
      for(int i = length; i >= 1; --i) {
        ClearPixel(snake[i][1], snake[i][0]);
      }
      break;
    }
  }
}

int8 __xdata snake[128][2];
int8 __xdata food[1][2];

enum SnakeDirection {
  kRight = 0,
  kDown = 1,
  kLeft = 2,
  kUp = 3,
};

uint8 dir;

void main(void) {
  int i;
  int length = 4;   //length of snake
  int check = 0; 
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
  food[1][0] = 5;     //created 2d array for multi-food compatablity
  food[1][1] = 5;
  dir = 0;  //set base direction to right


  // Draw the snake
  for (i = 0; i < 5; ++i) {
    SetPixel(/*row=*/snake[i][1], /*column=*/snake[i][0]);
  }
  SetPixel(/*same as snake (x=1, y=0)*/ food[1][1], food[1][0]);

  while (1) {
    DelayMs(200);
    
    if(snake[length][1] == food[1][1] && snake[length][0] == food[1][0]) {     //check if the snake's head intersects with the "food" (singular pixel)
      ++length;
      while(1) {
        bool check = 0;  
        int tempFoodX = rand() % 8 + 1;     //randomly gen a x for the food
        int tempFoodY = rand() % 16 + 1;    //randomly gen a y for the food
        for(i = length; i >= 1; --i) {
          if(snake[i][1] == tempFoodX || snake[i][0] == tempFoodY) {
            check = 1;
          }
        }
        if(check == 0) {
          food[1][1] = tempFoodX;
          food[1][0] = tempFoodY;
        }
      }
      SetPixel(food[1][1], food[1][0]);
    } else {
          ClearPixel(/*row=*/snake[length][1], /*column=*/snake[length][0]); // Move the snake
    }

    ///////////////////////////////////////////
    //find if snake colides with itself
    /*
    for(i = length; i >= 1; --i) {
      if(snake[length])
    }
    */
    ///////////////////////////////////////////

    for (i = length; i >= 1; --i) {      //Move the snake 1 pixel forward
      snake[i][0] = snake[i - 1][0];
      snake[i][1] = snake[i - 1][1];
    }
    /////////////////////////////////////////////
    if (WasButtonPressed(kRightBtn) > 0) {      //change direction based on button press
      dir = kRight;
    } else if (WasButtonPressed(kDownBtn) > 0) {
      dir = kDown;
    } else if (WasButtonPressed(kLeftBtn) > 0) {
      dir = kLeft;
    } else if (WasButtonPressed(kUpBtn) > 0) {
      dir = kUp;
    }
    //////////////////////////////////////////////
    switch (dir) {        // Check for wall hit
      case 0:
        if (snake[0][0] == 7) {
          Die(length);
          goto TEST;
        }
        break;
      case 1:
        if (snake[0][1] == 15) {
          Die(length);
          goto TEST;
        }
        break;
      case 2:
        if (snake[0][0] == 0) {
          Die(length);
          goto TEST;
        }
        break;
      case 3:
        if (snake[0][1] == 0) {
          Die(length);
          goto TEST;
        }
        break;
    }
    ////////////////////////////////////////////////
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
    /////////////////////////////////////////////////
    if (WasButtonPressed(kRstBtn) > 0) {
      goto TEST;
    }
    ///////////////////////////////////////////////////
    SetPixel(/*row=*/snake[0][1], /*column=*/snake[0][0]);
    ////////////////////////////////////////////////
  }
}

// For some reason if the interrupt method is declared in hw_driver.c
// it doesn't work. Need to declare it here and invoke the other one.
void OnTickInt() __interrupt(1) { OnTick(); }
