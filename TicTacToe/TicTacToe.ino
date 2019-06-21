// Color Definitons for Cube
#define BLACK 0,0,0
#define GREEN 0,255,0
#define RED   255,0,0
#define BLUE  0,0,255
#define ORANGE  204, 102, 0
#define LIGHT_ORANGE  255, 153, 0

// Colordefinitions for Players
#define ColorNO1  GREEN
#define ColorNO2  BLUE
#define ColorHIGHLIGHT  RED

// IP Reset Pin (Used for Clearing IP Blocking)
#define IPRESETPIN D3


// Used Librarys
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>

// WiFi AP Credentials
const char* ssid = "TicTacToe Wuerfel";


// Setup for Webserver (80: Standard Port)
ESP8266WebServer server(80);


// Setup for NeoPixels (4*4*4 == 64)
Adafruit_NeoPixel pixels(64, D4, NEO_GRB + NEO_KHZ800);

// Size of Buttons on Wepsite (in PX)
const int buttonSize = 80;


// Coordinates which are Marked (5 == not Valid Value)
byte xGlobal = 5,yGlobal = 5,zGlobal = 5;

// Buffer for active Website name
String actSite = "0";

// Definiton of website for OK_Button
String setVal = "setVal";

// Allowed Client IP for making changes ("" == no IP is set)
String allowedIP  = "";

// Buffer for lastSeen of Client (used to determin if allowed IP should be reset)
unsigned long lastSeen;

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


// Variables used for ScreenSaver as Buffer
byte activePhase = 0;
byte activeScreenSaver = 0;
byte maxScreenSaver = 3;

// for Phase changing
unsigned long previousPhaseMillis = 0, phaseTime = 200;

// for Saver changing
unsigned long previousScreenSaverMillis = 0, screenSaverTime = 20000;

// for defining maximum Phase for each ScreenSaver
byte maxPhase[] = {5,5,5,3,5,5,5,5,5,5,5,5};

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

// Debug Function, which Prints the GameField
void printArray(){
  for(byte i = 0; i < 4; i++){
     for(byte j = 0; j < 4; j++){
         for(byte k = 0; k < 4; k++){
            Serial.print(gameField[i][j][k]);
         }
         Serial.println("");
     }
     Serial.println("");
  }
  Serial.println("");
}


//SETUP
void setup() {
  pinMode(IPRESETPIN, INPUT_PULLUP);
  setUpWifi();
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
}

//LOOP
void loop() {
  // IF IP reset button is pressed reset IP
  if (!digitalRead(IPRESETPIN)){
    allowedIP = "";
  }
  
  // IF lastSeen is 2 minutes ago reset IP
  if(lastSeen + 120000 >= millis()){
    allowedIP = "";
  }

  //Check if Session is runnning (if ip == "" -> no session is running)
  if(allowedIP == ""){
    // if no session is running show Screensaver
    showScreenSaver();
  }else{
    // if session is running show gameField
    showField();
  }
  


  // Handle Server
  server.handleClient();

  // Used for ESP Wifi (If not called for long time (about 100 ms) ESP will reboot)
  yield();
  Serial.println(".");
  // TODO delete delay
  //delay(10);
}










//WEBSERVER
void handleNormal() {
  // If no session running start new Session with IP
  if(allowedIP == ""){
    
    allowedIP = server.client().remoteIP().toString();
  
  }
  
  
  if(allowedIP != server.client().remoteIP().toString()){
    // If Session is running with other IP generate Warning
    server.send(200, "text/html", "Ein anderer Spieler ist bereits am Spielen. <br> Um ein neues Ger&aumlt zuzulassen dr&uumlcke bitte den Knopf am W&uumlrfel.");
  
  }else{
    // If session is running with same IP do Magic :)

    // Set last seen Value (Used for automatic session ending)
    lastSeen = millis();
    
    // Check if new XYZ Coordinates
    if(split(server.uri(), '/', 1) == setVal)
    {
      // Extract XYZ Data
      String toHandle = split(server.uri(), '/', 2);

      
      // Save XYZ Data as byte
      xGlobal = byte(toHandle[0] - '0');
      yGlobal = byte(toHandle[1] - '0');
      zGlobal = byte(toHandle[2] - '0');
      
    }
    
    // if OK Command 
    else if (split(server.uri(), '/', 1) == "OK")
    {
      // do Move
      setStone(xGlobal,yGlobal,zGlobal);
      
      
      // Reset XYZ Data (5 is not a valid number)
      xGlobal = 5;
      yGlobal = 5;
      zGlobal = 5;
      
      
    }

    // After every Website reloding check End of Game Condition
    byte EOFCondition = checkEOGCondition();
    
    // If Draw
    if(EOFCondition == 255){
      server.send(200, "text/html", "<center><H1>UNENTSCHIEDEN</H1>" + resetButton() + "</center>");
    }
    //If winner return Congratulations
    else if(EOFCondition >= 1){
      server.send(200, "text/html", "<center><H1>Spieler " + String(EOFCondition) + " hat Gewonnen</H1> <br> Gr&uumln: Spieler 1 <br> Blau: Spieler 2 <br><br>" + resetButton() + "</center>");
    }
    // If nothing return Input Form
    else{
      server.send(200, "text/html", buildHTML());
    }
  }
}

// Function for generating reset Button
String resetButton(){
  return "<a href=\"../../reset\"><button class=\"button\" style=\"height:" + String(buttonSize) + "px;width:" + String(2*buttonSize) +"px;BACKGROUND-COLOR: Yellow;\"><font size=\"5\">Neustarten</font></button></a>";
}


// Function that generates HTML for Input Form
String buildHTML(){

  // Center everything
  String html = "<center>";

  //Do for every element of an "3d Array" 
  for(byte i = 0; i < 4; i++){
     for(byte j = 0; j < 4; j++){
         for(byte k = 0; k < 4; k++){
            //returns a button with an link to /setVal/ijk
            html += getButton(i,j,k);
            yield();
         }
         //Formatting
         html += "<br>";
     }
     //Formatting
     html += "<br>";
  }

  //Add a reset Button
  html += String(resetButton());

  // Add a OK button
  html += String("<a href=\"../../../OK/\"><button class=\"button\" style=\"height:" + String(buttonSize) +"px;width:" + String(2*buttonSize) +"px;BACKGROUND-COLOR: Red;\"><font size=\"5\">Best&aumltigen</font></button></a>");
  
  return html+= "</center>";
}


//Function that generates HTML Buttons with an link to /actSite/xyz
String getButton(byte x, byte y, byte z){
  String subSite = String(x, DEC) + String(y, DEC) + String(z, DEC);
  String color = "black";
  if(x == xGlobal && y == yGlobal && z == zGlobal){
    color = "red";
  }
  if(gameField[x][y][z] == byte(1)){
    color = "green";
  }else if(gameField[x][y][z] == byte(2)){
    color = "blue";
  }
  
  //Serial.println("SetStone: " + subSite + " gF: " + String(gameField[x][y][z], DEC) + " color: " + String(color));
  return String("<a href=\"../../" + setVal + "/" +  subSite + "/\"><button class=\"button\" style=\"height:" + String(buttonSize) + "px;width:" + String(buttonSize) + "px;BACKGROUND-COLOR:" + String(color) + ";\">" + "" + "</button></a>");
            
}

// Funtion that returns an blank HTML Site
void no(){server.send(200, "text/html", "<html></html>");}


//Setup for WIFI and Server
void setUpWifi(){
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  
  WiFi.softAP(ssid);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  server.on("/", handleNormal);
  server.on("/reset", resetGame);
  server.on("/favicon.ico", no);
  server.onNotFound(handleNormal);
  
  server.begin();
  Serial.println("HTTP server started");
}

String split(String s, byte parser, int index) {
  String rs="";
  int parserIndex = index;
  int parserCnt=0;
  int rFromIndex=0, rToIndex=-1;
  while (index >= parserCnt) {
    rFromIndex = rToIndex+1;
    rToIndex = s.indexOf(parser,rFromIndex);
    if (index == parserCnt) {
      if (rToIndex == 0 || rToIndex == -1) return "";
      return s.substring(rFromIndex,rToIndex);
    } else parserCnt++;
  }
  return rs;
}







/*
{
  {
    {
      {
        {
          {
            {
              {
                {
                  ;
                }
              }
            }
          }
        }
      }
    }
  }
}

*/







//GAME

void resetGame(){
  if(allowedIP == ""){
    
    allowedIP = server.client().remoteIP().toString();
  
  }
  
  // Some sort of Security (Only Session can reset)
  if(allowedIP != server.client().remoteIP().toString()){
    server.send(200, "text/html", "<html>Nice try</html>");
    return;
  }
  
  Serial.println("RESET");
  for(byte i = 0; i < 4; i++){
     for(byte j = 0; j < 4; j++){
         for(byte k = 0; k < 4; k++){
            gameField[i][j][k] = 0;
         }
     }
  }
  xGlobal = 5;
  yGlobal = 5;
  zGlobal = 5;
  
  changeCurrentPlayerNumber(); 
  handleNormal();
}
byte getCurrentPlayerNumber(){
  if(currentPlayer) return 1;
  return 2;
}
void setCurrentPlayerNumber(byte i){
  if(i == 1){
    currentPlayer = true;
  }else if (i == 2){
    currentPlayer = false;
  }
}
void changeCurrentPlayerNumber(){
  currentPlayer = !currentPlayer;
}
boolean setStone(byte x, byte y, byte z){
  if(x > 3 || y > 3 || z>3)return false;
  Serial.println("SetStone: CPN: " + String(getCurrentPlayerNumber()));
  Serial.println("GameField XYZ: " + String(gameField[x][y][z]));
  if(gameField[x][y][z] == 0 || gameField[x][y][z] == 3){
     gameField[x][y][z] = byte(getCurrentPlayerNumber());
     Serial.println("GameField XYZ: " + String(gameField[x][y][z]));
     changeCurrentPlayerNumber();
     printArray();
     return true;
  }
  return false;
}


byte checkEOGCondition(){
  
  //Code smell
  byte field [4][4][4];
  for(byte i = 0; i < 4; i++){
     for(byte j = 0; j < 4; j++){
         for(byte k = 0; k < 4; k++){
            field[i][j][k] = gameField[i][j][k];
        }
     }
  }
  //EO code smell
  
  for(byte i = 0; i < 4; i++) {
    for(byte j = 0; j < 4; j++) {
      byte temp[4];
      for(byte k = 0; k < 4; k++) {
        temp[k] = field[i][j][k];
      }
      if(temp[0] == temp[1] && temp[0] == temp[2] && temp[0] == temp[3] && temp[0] != 0) {
        if(temp[0] == 1)return 1;
        if(temp[0] == 2)return 2;
      }
    }
  }
  for(byte i = 0; i < 4; i++) {
    for(byte j = 0; j < 4; j++) {
      byte temp[4];
      for(byte k = 0; k < 4; k++) {
        temp[k] = field[i][k][j];
      }
      if(temp[0] == temp[1] && temp[0] == temp[2] && temp[0] == temp[3] && temp[0] != 0) {
        if(temp[0] == 1)return 1;
        if(temp[0] == 2)return 2;
      }
    }
  }
  for(byte i = 0; i < 4; i++) {
    for(byte j = 0; j < 4; j++) {
      byte temp[4];
      for(byte k = 0; k < 4; k++) {
        temp[k] = field[k][i][j];
      }
      if(temp[0] == temp[1] && temp[0] == temp[2] && temp[0] == temp[3] && temp[0] != 0) {
        if(temp[0] == 1)return 1;
        if(temp[0] == 2)return 2;
      }
    }
  }
  for(byte i = 0; i < 4; i++) {
    byte temp[4];
    for(byte k = 3; k >= 4; k--) {
      temp[k] = field[i][i][k];
    }
    if(temp[0] == temp[1] && temp[0] == temp[2] && temp[0] == temp[3] && temp[0] != 0) {
      if(temp[0] == 1)return 1;
      if(temp[0] == 2)return 2;
    }
  }
  for(byte i = 0; i < 4; i++){
    for(byte j = 0; j < 4; j++){
      for(byte k = 0; k < 4; k++){
        if(field[i][i][k] == 0){
          return 0;
        }
      }
    }
  }
  return 255; // Draw
}


boolean setPixel(byte x, byte y, byte z, byte r, byte g, byte b){
  if(x > 3 || y > 3 || z > 3){
    return false;
  }
  char pixel = z+y*4+z*16;
  pixels.setPixelColor(pixel, pixels.Color(r, g, b));
  return true;
}
void showField(){
  //Code smell
  byte field [4][4][4];
  for(byte i = 0; i < 4; i++){
     for(byte j = 0; j < 4; j++){
         for(byte k = 0; k < 4; k++){
            byte temp = gameField[i][j][k];
            if(temp == 1) setPixel(i,j,k, ColorNO1);
            if(temp == 2) setPixel(i,j,k, ColorNO2);
            if(temp == 3) setPixel(i,j,k, ColorHIGHLIGHT);
            if(temp == 0) setPixel(i,j,k, BLACK);
        }
     }
  }
  //EO code smell
}

void showScreenSaver(){
  
  
  // Do every screenSaverTime ms
  if (millis() - previousScreenSaverMillis >= screenSaverTime) {
        // change activeScreensaver
        activeScreenSaver++;

        // If max Screensaver is reached start at 0
        if(activeScreenSaver > maxScreenSaver)activeScreenSaver = 0;
        
        previousScreenSaverMillis = millis();
  }
  if (millis() - previousPhaseMillis >= phaseTime) {
        // change activeScreensaver
        activePhase++;

        // If max Screensaver is reached start at 0
        if(activePhase > maxPhase[activeScreenSaver])activePhase = 0;
        phaseChanged = true;
        previousPhaseMillis = millis();
  }


  pixels.clear();
  switch (activeScreenSaver){
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
  pixels.show();
  phaseChanged = false;
}

void changePhaseColor(){
  
}

// Wave screensaver in X-Direction
void screenSaver0(){
  for(byte i = 0; i < 4; i++){
    for(byte j = 0; j < 4; j++){
      byte temp = screenSaverWave[activePhase][i][j];
      for(byte k = 0; k < 4; k++){
          if(temp == 1)setPixel(i, j, k, phaseColor[0], phaseColor[1], phaseColor[2]);
      }
    }
  }
}
// Wave screensaver in Y-Direction
void screenSaver1(){
  for(byte i = 0; i < 4; i++){
    for(byte j = 0; j < 4; j++){
      byte temp = screenSaverWave[activePhase][i][j];
      for(byte k = 0; k < 4; k++){
          if(temp == 1)setPixel(j, i, k, phaseColor[0], phaseColor[1], phaseColor[2]);
      }
    }
  }
}



// Some Sort of Random explosion
void screenSaver2(){
  if(phaseChanged){
    if(activePhase == 0 || activePhase == 1){
      expX = random(0, 4);
      expY = random(0, 4);
      expZ = random(0, 4);
      
      setPixel(expX, expY, expZ, RED);
    }
    
    if(activePhase == 1 || activePhase == 2){
      setPixel(expX + 1, expY, expZ, ORANGE);
      setPixel(expX, expY + 1, expZ, ORANGE);
      setPixel(expX, expY, expZ + 1, ORANGE);

      setPixel(expX - 1, expY, expZ, ORANGE);
      setPixel(expX, expY - 1, expZ, ORANGE);
      setPixel(expX, expY, expZ - 1, ORANGE);
      
    }
    if(activePhase == 2 || activePhase == 3){
      setPixel(expX + 1, expY + 1, expZ, LIGHT_ORANGE);
      setPixel(expX + 1, expY - 1, expZ, LIGHT_ORANGE);

      setPixel(expX - 1, expY + 1, expZ, LIGHT_ORANGE);
      setPixel(expX - 1, expY - 1, expZ, LIGHT_ORANGE);

      
      setPixel(expX, expY + 1, expZ + 1, LIGHT_ORANGE);
      setPixel(expX, expY + 1, expZ - 1, LIGHT_ORANGE);

      setPixel(expX, expY + 1, expZ + 1, LIGHT_ORANGE);
      setPixel(expX, expY + 1, expZ - 1, LIGHT_ORANGE);
      
      
      setPixel(expX + 1, expY, expZ + 1, LIGHT_ORANGE);
      setPixel(expX - 1, expY, expZ + 1, LIGHT_ORANGE);

      setPixel(expX + 1, expY, expZ - 1, LIGHT_ORANGE);
      setPixel(expX - 1, expY, expZ - 1, LIGHT_ORANGE);
      
      
      
      
      setPixel(expX + 2, expY, expZ, LIGHT_ORANGE);
      setPixel(expX, expY + 2, expZ, LIGHT_ORANGE);
      setPixel(expX, expY, expZ + 2, LIGHT_ORANGE);

      setPixel(expX - 2, expY, expZ, LIGHT_ORANGE);
      setPixel(expX, expY - 2, expZ, LIGHT_ORANGE);
      setPixel(expX, expY, expZ - 2, LIGHT_ORANGE);
      
    }
  }
  
}
//END
void screenSaver3(){
  
}
void screenSaver4(){
  
}
