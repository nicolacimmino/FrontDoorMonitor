
// Copy "WiFiCredentials.example" and rename it to ".h", then set your
// WiFi SSID and password.
//
#include "WiFiCredentials.h"

#define DST_IP "192.168.0.250"

void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200); 
  
  //while(!Serial);
  delay(3000);
  Serial1.println("AT+RST");
  delay(1000);
  if (Serial1.find("ready"))
  {
    Serial.println("ESP8266 Init OK");
  }
  else
  {
    Serial.println("ESP8266 Init FAIL");
    while (1);
  }
  
  for(int c=0;c<3;c++)
  {  
    if(connectWiFi())
    {
      break;
    }
    delay(1000);
  }
}

void loop()
{
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += DST_IP;
  cmd += "\",80";
  Serial1.println(cmd);
  Serial.println(cmd);
  if (Serial1.find("Error")) return;
  cmd = "GET /door.php HTTP/1.0\r\n\r\n";
  Serial1.print("AT+CIPSEND=");
  Serial1.println(cmd.length());
  delay(2000);
  if (Serial1.find(">"))
  {
    Serial.print(">");
  } else
  {
    Serial1.println("AT+CIPCLOSE");
    Serial.println("connect timeout");
    delay(1000);
    while(1);
  }
  Serial1.print(cmd);
  delay(2000);
  //Serial.find("+IPD");
  while (Serial1.available())
  {
    char c = Serial1.read();
    Serial.write(c);
    if (c == '\r') Serial.print('\n');
  }
  Serial.println("====");
  
  while(1);
  
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
