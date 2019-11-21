/*
   This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License



   Exception for Friedrichs-Gymnasium Herford:

   This work is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License only for Friedrichs-Gymnasium Herford

   Author: Magnus Müller
   e-Mail: magnusm1709@icloud.com
*/

#include <Adafruit_NeoPixel.h>
#define xPixel 4
#define yPixel 4
#define zPixel 4


// Setup for NeoPixels (4*4*4 == 64)
Adafruit_NeoPixel pixels(xPixel*yPixel*zPixel, D3, NEO_RGB + NEO_KHZ800);


void setUpCube(){
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  delay(10);
  cubeClear(); 
  delay(10);
  
  delay(1000);
  for(uint8_t i = 0; i <= 4; i++){
    for(uint8_t j = 0; j <= 4; j++){
      for(uint8_t k = 0; k <= 4; k++){
        setPixel(i,j,k,100,0,0);
        delay(30);
      }
    }
  }
  
  delay(1000);
}


boolean setPixel(uint8_t x, uint8_t y, uint8_t z, uint8_t r, uint8_t g, uint8_t b) {
  if (x >= xPixel || y >= yPixel || z >= zPixel || x < 0 || y < 0 || z < 0) {
    return false;
  }
  uint8_t pixel = z + y * xPixel + x * (zPixel*yPixel); // calculates 
  //Serial.println((int)pixel);
  pixels.setPixelColor(pixel, pixels.Color(r, g, b));
  return true;
}

void cubeClear(){
  pixels.clear();
}

void cubeShow(){
  pixels.show();
}

void wholeCube(uint8_t r, uint8_t g, uint8_t b){
  for(uint8_t i = 0; i < (xPixel*yPixel*zPixel); i++ ){
      pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
}
