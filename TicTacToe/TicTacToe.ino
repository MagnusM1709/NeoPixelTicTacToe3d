/*
   This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License



   Exception for Friedrichs-Gymnasium Herford:

   This work is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License only for Friedrichs-Gymnasium Herford

   Author: Magnus Müller
   e-Mail: magnusm1709@icloud.com
*/

int counter = 0;


// IP Reset Pin (Used for Clearing IP Blocking)
#define IPRESETPIN D3


//SETUP
void setup() {
  pinMode(IPRESETPIN, INPUT_PULLUP);
  setUpWifi();
  setUpServer();
  setUpCube();
}

//LOOP
void loop() {
  unsigned long oM = millis();
  checkSession();

  //Check if Session is runnning (if ip == "" -> no session is running)
  if (!isSession()) {
    // if no session is running show Screensaver
    showScreenSaver();
//    pixels.clear();
  } else {
    // if session is running show gameField
    showField();
  }
  
  // Handle Server
  handleServer();

  // Used for ESP Wifi (If not called for long time (about 250 ms) ESP will reboot)
  yield();
  if(counter > 1000){
    cubeShow();
    counter = 0;
  }
  counter++;
  if(int gM = millis() - oM > 5){
    //Serial.println(millis() - gM);
  }
}
