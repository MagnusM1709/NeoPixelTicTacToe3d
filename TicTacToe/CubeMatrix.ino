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
Adafruit_NeoPixel pixels(xPixel*yPixel*zPixel, D4, NEO_GRB + NEO_KHZ800);


void setUpCube(){
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
}


boolean setPixel(byte x, byte y, byte z, byte r, byte g, byte b) {
  if (x >= xPixel || y >= yPixel || z >= zPixel) {
    return false;
  }
  char pixel = z + y * xPixel + x * (zPixel*yPixel);
  pixels.setPixelColor(pixel, pixels.Color(r, g, b));
  return true;
}

void cubeClear(){
  pixels.clear();
}

void cubeShow(){
  pixels.show();
}
