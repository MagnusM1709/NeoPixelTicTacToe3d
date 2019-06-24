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




// Boolean to save which turn it is (True == Player ONE)
boolean currentPlayer = false;

// gameField (Hold all Information, which player has a Stone on, which Place)
byte gameField[4][4][4] =
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
  for (byte i = 0; i < 4; i++) {
    for (byte j = 0; j < 4; j++) {
      for (byte k = 0; k < 4; k++) {
        gameField[i][j][k] = 0;
      }
    }
  }
  

  changeCurrentPlayerNumber();
  
}
byte getCurrentPlayerNumber() {
  if (currentPlayer) return 1;
  return 2;
}
void setCurrentPlayerNumber(byte i) {
  if (i == 1) {
    currentPlayer = true;
  } else if (i == 2) {
    currentPlayer = false;
  }
}
void changeCurrentPlayerNumber() {
  currentPlayer = !currentPlayer;
}
boolean setStone(byte x, byte y, byte z) {
  if (x > 3 || y > 3 || z > 3)return false;
  if (gameField[x][y][z] == 0 || gameField[x][y][z] == 3) {
    
    gameField[x][y][z] = byte(getCurrentPlayerNumber());
    
    changeCurrentPlayerNumber();
    return true;
  }
  return false;
}


byte checkEOGCondition() {

  //Code smell
  byte field [4][4][4];
  for (byte i = 0; i < 4; i++) {
    for (byte j = 0; j < 4; j++) {
      for (byte k = 0; k < 4; k++) {
        field[i][j][k] = gameField[i][j][k];
      }
    }
  }
  //EO code smell

  for (byte i = 0; i < 4; i++) {
    for (byte j = 0; j < 4; j++) {
      byte temp[4];
      for (byte k = 0; k < 4; k++) {
        temp[k] = field[i][j][k];
      }
      if (temp[0] == temp[1] && temp[0] == temp[2] && temp[0] == temp[3] && temp[0] != 0) {
        if (temp[0] == 1)return 1;
        if (temp[0] == 2)return 2;
      }
    }
  }
  for (byte i = 0; i < 4; i++) {
    for (byte j = 0; j < 4; j++) {
      byte temp[4];
      for (byte k = 0; k < 4; k++) {
        temp[k] = field[i][k][j];
      }
      if (temp[0] == temp[1] && temp[0] == temp[2] && temp[0] == temp[3] && temp[0] != 0) {
        if (temp[0] == 1)return 1;
        if (temp[0] == 2)return 2;
      }
    }
  }
  for (byte i = 0; i < 4; i++) {
    for (byte j = 0; j < 4; j++) {
      byte temp[4];
      for (byte k = 0; k < 4; k++) {
        temp[k] = field[k][i][j];
      }
      if (temp[0] == temp[1] && temp[0] == temp[2] && temp[0] == temp[3] && temp[0] != 0) {
        if (temp[0] == 1)return 1;
        if (temp[0] == 2)return 2;
      }
    }
  }
  for (byte i = 0; i < 4; i++) {
    byte temp[4];
    for (byte k = 3; k >= 4; k--) {
      temp[k] = field[i][i][k];
    }
    if (temp[0] == temp[1] && temp[0] == temp[2] && temp[0] == temp[3] && temp[0] != 0) {
      if (temp[0] == 1)return 1;
      if (temp[0] == 2)return 2;
    }
  }
  for (byte i = 0; i < 4; i++) {
    for (byte j = 0; j < 4; j++) {
      for (byte k = 0; k < 4; k++) {
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
  byte field [4][4][4];
  for (byte i = 0; i < 4; i++) {
    for (byte j = 0; j < 4; j++) {
      for (byte k = 0; k < 4; k++) {
        byte temp = gameField[i][j][k];
        if (temp == 1) setPixel(i, j, k, ColorNO1);
        if (temp == 2) setPixel(i, j, k, ColorNO2);
        if (temp == 3) setPixel(i, j, k, ColorHIGHLIGHT);
        if (temp == 0) setPixel(i, j, k, BLACK);
      }
    }
  }
  //EO code smell
}
