#include "WiFi.h"
 
void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin();
  Serial.println(WiFi.macAddress());
}
 
void loop()
{

}