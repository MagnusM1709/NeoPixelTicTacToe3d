/*
   This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License



   Exception for Friedrichs-Gymnasium Herford:

   This work is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License only for Friedrichs-Gymnasium Herford

   Author: Magnus Müller
   e-Mail: magnusm1709@icloud.com
*/

// Color Definitons for Cube
#define BLACK 0,0,0
#define GREEN 0,255,0
#define RED   255,0,0
#define BLUE  0,0,255


// Colordefinitions for Players
#define ColorNO1  GREEN
#define ColorNO2  BLUE
#define ColorHIGHLIGHT  RED

#define PLAYER1 1
#define PLAYER2 2
#define ACTIVESELECTION 3


// Boolean to save which turn it is (True == Player ONE)
boolean currentPlayer1 = false;

// gameField (Hold all Information, which player has a Stone on, which Place)
uint8_t gameField[4][4][4] =
{
  {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  },
  {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  },
  {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  },
  {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  }
};




//GAME

void resetGame() {
  

  Serial.println("RESET");
  for (uint8_t i = 0; i < 4; i++) {
    for (uint8_t j = 0; j < 4; j++) {
      for (uint8_t k = 0; k < 4; k++) {
        gameField[i][j][k] = 0;
      }
    }
  }
  

  changeCurrentPlayerNumber();
  
}
uint8_t getCurrentPlayerNumber() {
  if (currentPlayer1) return 1;
  return 2;
}
void setCurrentPlayerNumber(uint8_t i) {
  if (i == 1) {
    currentPlayer1 = true;
  } else if (i == 2) {
    currentPlayer1 = false;
  }
}
void changeCurrentPlayerNumber() {
  currentPlayer1 = !currentPlayer1;
}
boolean setStone(uint8_t x, uint8_t y, uint8_t z) {
  if (x > 3 || y > 3 || z > 3)return false;
  if (gameField[x][y][z] == 0 || gameField[x][y][z] == ACTIVESELECTION) {
    
    gameField[x][y][z] = uint8_t(getCurrentPlayerNumber());
    
    changeCurrentPlayerNumber();
    return true;
  }
  return false;
}


uint8_t checkEOGCondition() {

  //Code smell
  uint8_t field [4][4][4];
  for (uint8_t i = 0; i < 4; i++) {
    for (uint8_t j = 0; j < 4; j++) {
      for (uint8_t k = 0; k < 4; k++) {
        field[i][j][k] = gameField[i][j][k];
      }
    }
  }
  //EO code smell

  for (uint8_t i = 0; i < 4; i++) {
    for (uint8_t j = 0; j < 4; j++) {
      uint8_t temp[4];
      for (uint8_t k = 0; k < 4; k++) {
        temp[k] = field[i][j][k];
      }
      if (temp[0] == temp[1] && temp[0] == temp[2] && temp[0] == temp[3] && temp[0] != 0) {
        if (temp[0] == 1)return 1;
        if (temp[0] == 2)return 2;
      }
    }
  }
  for (uint8_t i = 0; i < 4; i++) {
    for (uint8_t j = 0; j < 4; j++) {
      uint8_t temp[4];
      for (uint8_t k = 0; k < 4; k++) {
        temp[k] = field[i][k][j];
      }
      if (temp[0] == temp[1] && temp[0] == temp[2] && temp[0] == temp[3] && temp[0] != 0) {
        if (temp[0] == 1)return 1;
        if (temp[0] == 2)return 2;
      }
    }
  }
  for (uint8_t i = 0; i < 4; i++) {
    for (uint8_t j = 0; j < 4; j++) {
      uint8_t temp[4];
      for (uint8_t k = 0; k < 4; k++) {
        temp[k] = field[k][i][j];
      }
      if (temp[0] == temp[1] && temp[0] == temp[2] && temp[0] == temp[3] && temp[0] != 0) {
        if (temp[0] == 1)return 1;
        if (temp[0] == 2)return 2;
      }
    }
  }
  for (uint8_t i = 0; i < 4; i++) {
    uint8_t temp[4];
    for (uint8_t k = 3; k >= 4; k--) {
      temp[k] = field[i][i][k];
    }
    if (temp[0] == temp[1] && temp[0] == temp[2] && temp[0] == temp[3] && temp[0] != 0) {
      if (temp[0] == 1)return 1;
      if (temp[0] == 2)return 2;
    }
  }
  for (uint8_t i = 0; i < 4; i++) {
    for (uint8_t j = 0; j < 4; j++) {
      for (uint8_t k = 0; k < 4; k++) {
        if (field[i][i][k] == 0) {
          return 0;
        }
      }
    }
  }
  return 255; // Draw
}



void showField() {
  //Code smell
  uint8_t field [4][4][4];
  for (uint8_t i = 0; i < 4; i++) {
    for (uint8_t j = 0; j < 4; j++) {
      for (uint8_t k = 0; k < 4; k++) {
        uint8_t temp = gameField[i][j][k];
        if (temp == 1) setPixel(i, j, k, ColorNO1);
        if (temp == 2) setPixel(i, j, k, ColorNO2);
        if (temp == 3) setPixel(i, j, k, ColorHIGHLIGHT);
        if (temp == 0) setPixel(i, j, k, BLACK);
      }
    }
  }
  //EO code smell
}
