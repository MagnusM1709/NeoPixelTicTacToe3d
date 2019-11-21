/*
   This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License



   Exception for Friedrichs-Gymnasium Herford:

   This work is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License only for Friedrichs-Gymnasium Herford

   Author: Magnus Müller
   e-Mail: magnusm1709@icloud.com
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// WiFi AP Credentials
const char* ssid = "TicTacToe Cube";


// Setup for Webserver (80: Standard Port)
ESP8266WebServer server(80);



// Size of Buttons on Wepsite (in PX)
const uint8_t buttonSize = 75;


// Coordinates which are Marked (5 == not Valid Value)
uint8_t xGlobal = 5, yGlobal = 5, zGlobal = 5;

// Buffer for active Website name
String actSite = "0";

// Definiton of website for OK_Button
String setVal = "setVal";

// Allowed Client IP for making changes ("" == no IP is set)
String allowedIP  = "";

// Buffer for lastSeen of Client (used to determin if allowed IP should be reset)
unsigned long lastSeen;

//WEBSERVER

//Setup for WIFI and Server
void setUpWifi() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");

  WiFi.softAP(ssid);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  
}

boolean isSession(){
  
  
  return !(allowedIP.equals(""));
}
boolean checkAllowed(){
  Serial.println((allowedIP == server.client().remoteIP().toString()));
  return (allowedIP == server.client().remoteIP().toString());
}
void checkSession(){
  // IF IP reset button is pressed reset IP
  if (!digitalRead(IPRESETPIN)) {
    allowedIP = "";
  }

  // IF lastSeen is 2 minutes ago reset IP
  if (lastSeen + 120000 >= millis()) {
    allowedIP = "";
  }
}
void handleServer(){
  server.handleClient();
}
void setUpServer(){
  server.on("/", handleNormal);
  server.on("/reset", resetSite);
  server.on("/favicon.ico", no);
  server.onNotFound(handleNormal);

  server.begin();
  Serial.println("HTTP server started");
}


void handleNormal() {
  // If no session running start new Session with IP
  if (!isSession()) {

    allowedIP = server.client().remoteIP().toString();

  }


  if (!checkAllowed()) {
    // If Session is running with other IP generate Warning
    server.send(200, "text/html", "Ein anderer Spieler ist bereits am Spielen. <br> Um ein neues Ger&aumlt zuzulassen dr&uumlcke bitte den Knopf am W&uumlrfel.");

  } else {
    // If session is running with same IP do Magic :)

    // Set last seen Value (Used for automatic session ending)
    lastSeen = millis();

    // Check if new XYZ Coordinates
    if (split(server.uri(), '/', 1) == setVal) {
      // Extract XYZ Data
      String toHandle = split(server.uri(), '/', 2);


      // Save XYZ Data as uint8_t
      xGlobal = uint8_t(toHandle[0] - '0');
      yGlobal = uint8_t(toHandle[1] - '0');
      zGlobal = uint8_t(toHandle[2] - '0');

    }

    // if OK Command
    else if (split(server.uri(), '/', 1) == "OK") {
      // do Move
      setStone(xGlobal, yGlobal, zGlobal);


      // Reset XYZ Data (5 is not a valid number)
      xGlobal = 5;
      yGlobal = 5;
      zGlobal = 5;


    }

    // After every Website reloding check End of Game Condition
    uint8_t EOGCondition = checkEOGCondition();
    //Serial.println("EOFCondition: " + checkEOGCondition());
    // If Draw
    if (EOGCondition == 255) {
      server.send(200, "text/html", "<center><H1>UNENTSCHIEDEN</H1>" + resetButton() + "</center>");
    }
    //If winner return Congratulations
    else if (EOGCondition >= 1) {
      server.send(200, "text/html", "<center><font size=\"5\"><H1>Spieler " + String(EOGCondition) + " hat Gewonnen</H1> <br> Gr&uumln: Spieler 1 <br> Blau: Spieler 2 <br><br>" + resetButton() + "</font></center>");
    }
    // If nothing return Input Form
    else {
      server.send(200, "text/html", buildHTML());
    }
  }
}

// Function for generating reset Button
String resetButton() {
  return "<a href=\"../../reset\"><button class=\"button\" style=\"height:" + String(buttonSize) + "px;width:" + String(2 * buttonSize) + "px;BACKGROUND-COLOR: Yellow;\"><font size=\"5\">Neustarten</font></button></a>";
}


// Function that generates HTML for Input Form
String buildHTML() {

  // Center everything
  String html = "<center>";

  //Do for every element of an "3d Array"
  for (uint8_t i = 0; i < 4; i++) {
    for (uint8_t j = 0; j < 4; j++) {
      for (uint8_t l = 0; l < j * 5; l++) {
        html += "&nbsp;";
      }
      for (uint8_t k = 0; k < 4; k++) {
        //returns a button with an link to /setVal/ijk
        html += getButton(i, j, k);

        //yield();
      }
      //Formatting

      html += " <br> ";
    }
    //Formatting
    html += "<br>";
  }

  //Add a reset Button
  html += String(resetButton());

  // Add a OK button
  html += String("<a href=\"../../../OK/\"><button class=\"button\" style=\"height:" + String(buttonSize) + "px;width:" + String(2 * buttonSize) + "px;BACKGROUND-COLOR: Red;\"><font size=\"5\">Best&aumltigen</font></button></a>");

  return html += "</center>";
}


//Function that generates HTML Buttons with an link to /actSite/xyz
String getButton(uint8_t x, uint8_t y, uint8_t z) {
  String subSite = String(x, DEC) + String(y, DEC) + String(z, DEC);
  String color = "black";
  if (x == xGlobal && y == yGlobal && z == zGlobal) {
    color = "red";
  }
  if (gameField[x][y][z] == uint8_t(1)) {
    color = "green";
  } else if (gameField[x][y][z] == uint8_t(2)) {
    color = "blue";
  }

  return String("<a href=\"../../" + setVal + "/" +  subSite + "/\"><button class=\"button\" style=\"height:" + String(buttonSize) + "px;width:" + String(buttonSize) + "px;BACKGROUND-COLOR:" + String(color) + ";\"></button></a>");

}

// Funtion that returns an blank HTML Site
void no() {
  server.send(200, "text/html", "<html></html>");
}

void resetSite(){
  if (!isSession) {
    allowedIP = server.client().remoteIP().toString();
  }

  // Some sort of Security (Only Session can reset)
  if (!checkAllowed()) {
    server.send(200, "text/html", "<html>Nice try</html>");
    return;
  }
  resetGame();
  xGlobal = 5;
  yGlobal = 5;
  zGlobal = 5;
  handleNormal();
}

// Helper Function for Stringpsplitting
String split(String s, uint8_t parser, int index) {
  String rs = "";
  int parserIndex = index;
  int parserCnt = 0;
  int rFromIndex = 0, rToIndex = -1;
  while (index >= parserCnt) {
    rFromIndex = rToIndex + 1;
    rToIndex = s.indexOf(parser, rFromIndex);
    if (index == parserCnt) {
      if (rToIndex == 0 || rToIndex == -1) return "";
      return s.substring(rFromIndex, rToIndex);
    } else parserCnt++;
  }
  return rs;
}
