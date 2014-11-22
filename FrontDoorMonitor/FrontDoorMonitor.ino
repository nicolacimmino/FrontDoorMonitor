// FrontDoorMonitor implements logic for a WiFi enabled front door monitor.
//  Copyright (C) 2014 Nicola Cimmino
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see http://www.gnu.org/licenses/.
//
// Copy "WiFiCredentials.example" and rename it to ".h", then set your
// WiFi SSID and password.
//
#include "WiFiCredentials.h"

#include <LowPower_Teensy3.h>
TEENSY3_LP LP = TEENSY3_LP();

#define DST_IP "192.168.0.250"

// Max attempts to join the AP
#define MAX_CONNECT_ATTEMPTS 3

#define ESP8266_PWR_PIN 23

#define EXT_DOOR_PIN 22

#define INT_DOOR_PIN 21

#define OPEN 1

#define CLOSED 0

void setup()
{
  Serial.begin(115200);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  pinMode(EXT_DOOR_PIN, INPUT_PULLUP);
  pinMode(INT_DOOR_PIN, INPUT_PULLUP);
 
  // This might be reset following power up.
  // Go immediately to sleep.  
  if(digitalRead(EXT_DOOR_PIN)==CLOSED)
  {
    goToSleep();  
  }
  
  char transitDiection='i';
  if(digitalRead(INT_DOOR_PIN)==OPEN)
  {
    transitDiection='o';
  }
    
  Serial1.begin(115200); 
  pinMode(ESP8266_PWR_PIN, OUTPUT);

  // Power up the ESP8266
  // We need to introduce
  digitalWrite(ESP8266_PWR_PIN, LOW);
  delay(5000);
  
  Serial1.println("AT+RST");
  delay(1000);
  if (!Serial1.find("ready"))
  {
    // ESP8266 failed to initialize.
    goToSleep();
  }
   
  // Try to get connected to WiFi. 
  int attempt=0;
  for(; attempt<MAX_CONNECT_ATTEMPTS;attempt++)
  {
    if(connectWiFi()) break; 
  
    delay(1000);
  }
  
  // Pointless to continue unless we have WiFi connection.
  if(attempt==MAX_CONNECT_ATTEMPTS)
  {
    goToSleep();  
  }
 
  delay(2000);
  sendEvent(transitDiection);
  
  goToSleep();
  
}

void loop()
{
  // We actually never un this as we have a sigle 
  // flow after reset and then go back to sleep. 
}

/*
 * Wait until the specified door configuration is reached.
 * If timeout is >0 the function might return false if
 * the desired configuration is not achieved in the supplied
 * timeout (in mS).
 */
boolean waitDoorConfiguration(byte external, byte internal,long timeout)
{
  long startTime=millis();
  while(timeout==0 || millis()-startTime<timeout)
  {
    if(digitalRead(EXT_DOOR_PIN)==external && digitalRead(INT_DOOR_PIN)==internal)
      return true;  
  }
  
  // We reached time out, wait was not sucessfull.
  return false;
}

void goToSleep()
{
  // Power down ESP8266
  digitalWrite(ESP8266_PWR_PIN, HIGH);
  pinMode(ESP8266_PWR_PIN, INPUT);
  
  digitalWrite(LED_BUILTIN, LOW);
  
  LP.Hibernate(GPIO_WAKE, PIN_22);
  
}


boolean connectWiFi()
{
  Serial.println("AT+CWMODE=1");
  String cmd = "AT+CWJAP=\"";
  cmd += SSID;
  cmd += "\",\"";
  cmd += PASS;
  cmd += "\"";
  Serial.println(cmd);
  Serial1.println(cmd);
  delay(2000);
  if (Serial1.find("OK"))
  {
    Serial.println("OK, Connected to WiFi.");
    return true;
  } else
  {
    Serial.println("Can not connect to the WiFi.");
    return false;
  }
}

boolean sendEvent(char event)
{
  // Open HTTP port on destination address
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += DST_IP;
  cmd += "\",80";
  Serial1.println(cmd);
  Serial.println(cmd);
  delay(1000);
  
  // Perform HTTP GET request. This is the header.
  cmd = "GET /door.php?ev=";
  cmd += event;
  cmd += " HTTP/1.0\r\n\r\n";
  Serial1.print("AT+CIPSEND=");
  Serial1.println(cmd.length());
  
  // We should get a ">" prompt 
  if (!waitESPReply(">",2000))
  {
    Serial.println("CIPSEND timeout");
    Serial1.println("AT+CIPCLOSE");
    return false;
  }
  delay(1000);
  Serial1.print(cmd);
  Serial.println(cmd);
  
  delay(2000);
  Serial1.println("AT+CIPCLOSE");
  Serial.println("done http");
    
  return true;
}

/*
 * Waits untils the supplied reply string is found
 * in in the input buffer. If timeout>0 the function
 * will return false should the string not be received
 * by the specified time (in mS).
 */
boolean waitESPReply(char* reply, long timeout)
{
  long startTime=millis();
  while(timeout==0 || millis()-startTime<timeout)
  {
    if(Serial1.find(reply)) return true;
  }
  
  // We reached time out, wait was not sucessfull.
  return false;  
}
