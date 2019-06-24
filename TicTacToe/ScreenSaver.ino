/*
   This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License



   Exception for Friedrichs-Gymnasium Herford:

   This work is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License only for Friedrichs-Gymnasium Herford

   Author: Magnus Müller
   e-Mail: magnusm1709@icloud.com
*/


// Variables used for ScreenSaver as Buffer
byte activePhase = 0;
byte activeScreenSaver = 0;
byte maxScreenSaver = 3;

// Used for Colorchanging method
byte actRisingIndex = 0;


// for Phase changing
unsigned long previousPhaseMillis = 0, phaseTime = 200;

// for SScreensaver changing
unsigned long previousScreenSaverMillis = 0, screenSaverTime = 20000;

// for Color changing
unsigned long previousColorMillis = 0, ColorTime = 20;

// for defining maximum Phase for each ScreenSaver
byte maxPhase[] = {5, 5, 5, 3, 5, 5, 5, 5, 5, 5, 5, 5};

// For setting if phase changed since last call of screensaver
boolean phaseChanged = false;

// For color of screensaver
byte phaseColor[] = {255, 0, 0};

// For explosion screensaver
byte expX, expY, expZ;


// holds information used by screensaver
byte screenSaverWave[6][4][4] =
{
  {
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}
  },
  {
    {0, 0, 0, 0},
    {1, 0, 0, 0},
    {0, 1, 0, 1},
    {0, 0, 1, 0}
  },
  {
    {0, 0, 0, 0},
    {0, 0, 0, 1},
    {1, 0, 1, 0},
    {0, 1, 0, 0}
  },
  {
    {0, 0, 0, 1},
    {0, 0, 1, 0},
    {0, 1, 0, 0},
    {1, 0, 0, 0}
  },
  {
    {0, 0, 1, 0},
    {0, 1, 0, 1},
    {1, 0, 0, 0},
    {0, 0, 0, 0}
  },
  {
    {0, 1, 0, 0},
    {1, 0, 1, 0},
    {0, 0, 0, 1},
    {0, 0, 0, 0}
  }
};

void showScreenSaver() {

  // Do every screenSaverTime ms
  if (millis() - previousScreenSaverMillis >= screenSaverTime) {
    // change activeScreensaver
    //activeScreenSaver++;

    // If max Screensaver is reached start at 0
    if (activeScreenSaver > maxScreenSaver)activeScreenSaver = 0;

    previousScreenSaverMillis = millis();
  }
  
  if (millis() - previousPhaseMillis >= phaseTime) {
    // change activeScreensaver
    activePhase++;

    // If max Screensaver is reached start at 0
    if (activePhase > maxPhase[activeScreenSaver])activePhase = 0;
    phaseChanged = true;
    previousPhaseMillis = millis();
  }
  if (millis() - previousColorMillis >= ColorTime) {
    changePhaseColor();
    previousColorMillis = millis();
  }


  cubeClear();
  switch (activeScreenSaver) {
    case 0:
      screenSaver0();
      break;

    case 1:
      screenSaver1();
      break;

    case 2:
      screenSaver2();
      break;

    case 3:
      screenSaver3();
      break;

    case 4:
      screenSaver4();
      break;
  }
  phaseChanged = false;
}

void changePhaseColor() {
  if(phaseColor[actRisingIndex] >255)actRisingIndex++;
  if(actRisingIndex >= 3)actRisingIndex = 0;
  switch (actRisingIndex){
    case 0:
      phaseColor[0]+=10;
      phaseColor[2]-=10;
      break;
    case 1:
      phaseColor[1]+=10;
      phaseColor[0]-=10;
      break;
    case 2:
      phaseColor[2]+=10;
      phaseColor[1]-=10;
      break;
  }
}

// Wave screensaver in X-Direction
void screenSaver0() {
  for (byte i = 0; i < 4; i++) {
    for (byte j = 0; j < 4; j++) {
      byte temp = screenSaverWave[activePhase][i][j];
      for (byte k = 0; k < 4; k++) {
        if (temp == 1)setPixel(i, j, k, phaseColor[0], phaseColor[1], phaseColor[2]);
      }
      
    }
  }
}
// Wave screensaver in Y-Direction
void screenSaver1() {
  for (byte i = 0; i < 4; i++) {
    for (byte j = 0; j < 4; j++) {
      byte temp = screenSaverWave[activePhase][i][j];
      for (byte k = 0; k < 4; k++) {
        if (temp == 1)setPixel(j, i, k, phaseColor[0], phaseColor[1], phaseColor[2]);
      }
    }
  }
}



// Some Sort of Random explosion
void screenSaver2() {
  #define phase1 0.7
  #define phase2 0.5
  if (phaseChanged && activePhase == 0) {
    expX = random(0, 4);
    expY = random(0, 4);
    expZ = random(0, 4);
  }
  if (activePhase == 0 || activePhase == 1) {
    

    setPixel(expX, expY, expZ, phaseColor[0], phaseColor[1], phaseColor[2]);
  }

  if (activePhase == 1 || activePhase == 2) {
    setPixel(expX + 1, expY, expZ, phase1*phaseColor[0], phase1*phaseColor[1], phase1*phaseColor[2]);
    setPixel(expX, expY + 1, expZ, phase1*phaseColor[0], phase1*phaseColor[1], phase1*phaseColor[2]);
    setPixel(expX, expY, expZ + 1, phase1*phaseColor[0], phase1*phaseColor[1], phase1*phaseColor[2]);

    setPixel(expX - 1, expY, expZ, phase1*phaseColor[0], phase1*phaseColor[1], phase1*phaseColor[2]);
    setPixel(expX, expY - 1, expZ, phase1*phaseColor[0], phase1*phaseColor[1], phase1*phaseColor[2]);
    setPixel(expX, expY, expZ - 1, phase1*phaseColor[0], phase1*phaseColor[1], phase1*phaseColor[2]);

  }
  if (activePhase == 2 || activePhase == 3) {
    setPixel(expX + 1, expY + 1, expZ, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);
    setPixel(expX + 1, expY - 1, expZ, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);

    setPixel(expX - 1, expY + 1, expZ, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);
    setPixel(expX - 1, expY - 1, expZ, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);


    setPixel(expX, expY + 1, expZ + 1, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);
    setPixel(expX, expY + 1, expZ - 1, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);

    setPixel(expX, expY + 1, expZ + 1, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);
    setPixel(expX, expY + 1, expZ - 1, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);


    setPixel(expX + 1, expY, expZ + 1, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);
    setPixel(expX - 1, expY, expZ + 1, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);

    setPixel(expX + 1, expY, expZ - 1, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);
    setPixel(expX - 1, expY, expZ - 1, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);




    setPixel(expX + 2, expY, expZ, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);
    setPixel(expX, expY + 2, expZ, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);
    setPixel(expX, expY, expZ + 2, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);

    setPixel(expX - 2, expY, expZ, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);
    setPixel(expX, expY - 2, expZ, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);
    setPixel(expX, expY, expZ - 2, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);

  }
}
//END
void screenSaver3() {

}
void screenSaver4() {

}
