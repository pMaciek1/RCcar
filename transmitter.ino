#include <WiFi.h>
#include <esp_now.h>
#include <LiquidCrystal.h>

#define X_AXIS 32
#define Y_AXIS 33

uint8_t recMac[] = {0x3C,0x8A,0x1F,0x9A,0x6D,0x90}; //3C:8A:1F:9A:6D:90
LiquidCrystal lcd(19,23,18,17,16,15);

struct PacketDataSend{
  byte xAxisV;
  byte yAxisV;
};
PacketDataSend dataSend;

struct PacketDataRecv{
  byte distance;
};
PacketDataRecv rData;

int mapJoystick(int value){
  if (value >= 2800)
  {
    value = map(value, 2200, 4095, 127, 254);
  }
  else if (value <= 1200)
  {
    value = map(value, 1800, 0, 127, 0);  
  }
  else
  {
    value = 127;
  }

  return value;
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t ");
  Serial.println(status);
  if (status == ESP_NOW_SEND_SUCCESS){
    Serial.println("Message sent");
  }
  else{
    Serial.println("Message failed");
    lcd.setCursor(0,1);
    lcd.print("Connection fail");
  }
}

void OnDataRecv(const esp_now_recv_info * mac, const unsigned char *incomingData, int len){
  if(len==0){
    return;
  }
  memcpy(&rData, incomingData, sizeof(rData));
  String inputData;
  inputData = inputData + "values " + rData.distance;
  Serial.println(inputData);

  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("Distance: ");
  lcd.print(rData.distance);
  lcd.print("CM");
  
}


void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin();

  lcd.begin(16, 2);

  

  pinMode(X_AXIS, INPUT);
  pinMode(Y_AXIS, INPUT);

   if (esp_now_init() != ESP_OK) 
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  else
  {
    Serial.println("Succes: Initialized ESP-NOW");
  }

  esp_now_register_recv_cb(OnDataRecv);

  esp_now_register_send_cb(OnDataSent);

  esp_now_peer_info_t peerInfo = {};

  memcpy(peerInfo.peer_addr, recMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
  else
  {
    Serial.println("Succes: Added peer");
  }

}
 
void loop() {
  dataSend.xAxisV = mapJoystick(analogRead(X_AXIS));
  dataSend.yAxisV = mapJoystick(analogRead(Y_AXIS));
  
  esp_err_t result = esp_now_send(recMac, (uint8_t *) &dataSend, sizeof(dataSend));

  if (result == ESP_OK) 
  {
    Serial.println("Sent with success");
  }
  else 
  {
    Serial.println("Error sending the data");
  }

  delay(50);
}