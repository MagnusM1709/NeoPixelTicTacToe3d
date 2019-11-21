/*
   This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License



   Exception for Friedrichs-Gymnasium Herford:

   This work is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License only for Friedrichs-Gymnasium Herford

   Author: Magnus Müller
   e-Mail: magnusm1709@icloud.com
*/


// Variables used for ScreenSaver as Buffer
uint8_t activePhase = 0;
uint8_t activeScreenSaver = 0;

// Starts counting by zero
static uint8_t maxScreenSaver = 1;

// Used for Colorchanging method
uint8_t actRisingIndex = 0;


// for Phase changing
unsigned long previousPhaseMillis = 0, phaseTime = 200;

// for SScreensaver changing
unsigned long previousScreenSaverMillis = 0, screenSaverTime = 10000;

// for Color changing
unsigned long previousColorMillis = 0, ColorTime = 20;

// for defining maximum Phase for each ScreenSaver
static uint8_t maxPhase[] = {5, 5, 5, 3, 5, 5, 5, 5, 5, 5, 5, 5};

// For setting if phase changed since last call of screensaver
boolean phaseChanged = false, screenSaverChanged = true;

// For color of screensaver
uint8_t phaseColor[] = {245, 0, 0};

// For explosion screensaver
uint8_t expX, expY, expZ;

// For changing directions of screensaver (XYZ get swapped in realtion to this value); more information in method randXYZ
uint8_t xyzChangeValue = 0;

// holds information used by screensaver wave, pattern which determins which led row (X, Y or Z) will light
static uint8_t screenSaverWave[6][4][4] =
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


//Function that randomizes the XYZ meaning; everytime the Screensaver changes this will generate a new randomization
void randXYZ(uint8_t x, uint8_t y, uint8_t z, uint8_t r, uint8_t b, uint8_t g){
  uint8_t nx;
  uint8_t ny;
  uint8_t nz;
  if(screenSaverChanged){
    xyzChangeValue = random(0,6);
  }

  switch(xyzChangeValue){
    case 0:
      nx = x;
      ny = y;
      nz = z;
      break;
    case 1:
      nx = x;
      ny = z;
      nz = y;
      break;
    case 2:
      nx = y;
      ny = x;
      nz = z;
      break;
    case 3:
      nx = y;
      ny = z;
      nz = x;
      break;
    case 4:
      nx = z;
      ny = x;
      nz = y;
      break;
    case 5:
      nx = z;
      ny = y;
      nz = x;
      break;
    
      break;
    default:
      break;
  }

  
  randXYZ(nx, ny, nz, r, g, b);
}


void showScreenSaver() {

  // Do every screenSaverTime ms
  if (millis() - previousScreenSaverMillis >= screenSaverTime) {
    // change activeScreensaver
    activeScreenSaver++;

    // If max Screensaver is reached ->  restart at 0
    if (activeScreenSaver > maxScreenSaver){
      activeScreenSaver = 0;
    }
    screenSaverChanged = true;
    previousScreenSaverMillis = millis();
  }
  
  if (millis() - previousPhaseMillis >= phaseTime) {
    // change activeScreensaver
    activePhase++;

    // If max Screensaver is reached -> restart at 0
    if (activePhase > maxPhase[activeScreenSaver]){
      activePhase = 0;
    }
    
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
      wave();
      break;

    case 1:
      explosion();
      break;
    case 2:
      strobo();
      break;
    default:
      break;
  }
  phaseChanged = false;
  screenSaverChanged = false;
}

void changePhaseColor() {
  if(phaseColor[actRisingIndex] > 240)actRisingIndex++; // If max color is reached (here 240, because 240 + 10 is 5 under theoretical maximum )
  if(actRisingIndex >= 3)actRisingIndex = 0; // if the index is out of range it will restart at zero
  switch (actRisingIndex){ // changing colors thru the index
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
    default: // error recognition
      Serial.println("You fucked up something in method changePhaseColor: actRisingIndex is out of range");
      break;
  }
}

// Wave screensaver
void wave() {
  for (uint8_t i = 0; i < 4; i++) {
    for (uint8_t j = 0; j < 4; j++) {
      uint8_t temp = screenSaverWave[activePhase][i][j];
      for (uint8_t k = 0; k < 4; k++) {
        if (temp == 1)randXYZ(i, j, k, phaseColor[0], phaseColor[1], phaseColor[2]);
      }
      
    }
  }
}




// Some Sort of Random explosion
void explosion() {
  #define phase1 0.7
  #define phase2 0.5
  if (phaseChanged && activePhase == 0) { // if new explosion: generate new coordinates
    expX = random(0, 4);
    expY = random(0, 4);
    expZ = random(0, 4);
  }

  /*
   * Hard coded the explosion algorithm in relation of expX, expY, expZ 
   * Dont try to understand this
   * its working as it should
   */
  if (activePhase == 0 || activePhase == 1) {
    

    randXYZ(expX, expY, expZ, phaseColor[0], phaseColor[1], phaseColor[2]);
  }

  if (activePhase == 1 || activePhase == 2) {
    randXYZ(expX + 1, expY, expZ, phase1*phaseColor[0], phase1*phaseColor[1], phase1*phaseColor[2]);
    randXYZ(expX, expY + 1, expZ, phase1*phaseColor[0], phase1*phaseColor[1], phase1*phaseColor[2]);
    randXYZ(expX, expY, expZ + 1, phase1*phaseColor[0], phase1*phaseColor[1], phase1*phaseColor[2]);

    randXYZ(expX - 1, expY, expZ, phase1*phaseColor[0], phase1*phaseColor[1], phase1*phaseColor[2]);
    randXYZ(expX, expY - 1, expZ, phase1*phaseColor[0], phase1*phaseColor[1], phase1*phaseColor[2]);
    randXYZ(expX, expY, expZ - 1, phase1*phaseColor[0], phase1*phaseColor[1], phase1*phaseColor[2]);

  }
  if (activePhase == 2 || activePhase == 3) {
    randXYZ(expX + 1, expY + 1, expZ, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);
    randXYZ(expX + 1, expY - 1, expZ, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);

    randXYZ(expX - 1, expY + 1, expZ, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);
    randXYZ(expX - 1, expY - 1, expZ, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);


    randXYZ(expX, expY + 1, expZ + 1, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);
    randXYZ(expX, expY + 1, expZ - 1, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);

    randXYZ(expX, expY - 1, expZ + 1, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);
    randXYZ(expX, expY - 1, expZ - 1, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);


    randXYZ(expX + 1, expY, expZ + 1, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);
    randXYZ(expX - 1, expY, expZ + 1, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);

    randXYZ(expX + 1, expY, expZ - 1, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);
    randXYZ(expX - 1, expY, expZ - 1, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);




    randXYZ(expX + 2, expY, expZ, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);
    randXYZ(expX, expY + 2, expZ, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);
    randXYZ(expX, expY, expZ + 2, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);

    randXYZ(expX - 2, expY, expZ, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);
    randXYZ(expX, expY - 2, expZ, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);
    randXYZ(expX, expY, expZ - 2, phase2*phaseColor[0], phase2*phaseColor[1], phase2*phaseColor[2]);

  }
}

// strobo screensaver
void strobo() {
  if(true){ //TODO: make it working
    wholeCube(255, 255, 255);
  }else{
    wholeCube(0, 0, 0);
  }
}

//TODO
void colorFade() {

}

//TODO
void equalizer() {

}
