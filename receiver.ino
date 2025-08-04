#include <esp_now.h>
#include <WiFi.h>


#define RIGHT1 27
#define RIGHT2 26
#define LEFT1 25
#define LEFT2 33

#define ECHO 13
#define TRIG 12

long dur;
float distCM;

uint8_t recMac[] = {0x20,0x43,0xA8,0x6B,0x07,0xB0}; //20:43:A8:6B:07:B0

#define MAX_SPEED 200

const int PWMFreq = 1000; /* 1 KHz */
const int PWMResolution = 8;

#define SIGNAL_TIMEOUT 1000
unsigned long lastRecvTime = 0;

struct PacketData{
  byte xAxisV;
  byte yAxisV;
};
PacketData rData;

struct PacketDataSend{
  byte distance;
};
PacketDataSend dataSend;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t ");
  Serial.println(status);
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Message sent" : "Message failed");
}

void OnDataRecv(const esp_now_recv_info * mac, const unsigned char *incomingData, int len){
  if(len==0){
    return;
  }
  memcpy(&rData, incomingData, sizeof(rData));
  String inputData;
  inputData = inputData + "values " + rData.xAxisV + " " + rData.yAxisV;
  Serial.println(inputData);



  int throttle = map(rData.yAxisV, 254, 0, -255, 255);
  int steering = map(rData.yAxisV, 0, 254, -255, 255);
  int motorDir = 1;
  if (throttle < 0){
    motorDir = -1;
  }

  if (rData.yAxisV <= 75)       //Move car Forward
  {
    rotateMotor(MAX_SPEED, MAX_SPEED);
  }
  else if (rData.yAxisV >= 175)   //Move car Backward
  {
    rotateMotor(-MAX_SPEED, -MAX_SPEED);
  }
  else if (rData.xAxisV >= 175)  //Move car Right
  {
    rotateMotor(MAX_SPEED, -MAX_SPEED);
  }
  else if (rData.xAxisV <= 75)   //Move car Left
  {
    rotateMotor(-MAX_SPEED, MAX_SPEED);
  }
  else                                      //Stop the car
  {
    rotateMotor(0, 0);
  }   

  lastRecvTime = millis();
}

void rotateMotor(int rightMotor, int leftMotor){
  if (rightMotor < 0){
    digitalWrite(RIGHT1, LOW);
    digitalWrite(RIGHT2, HIGH);
  }
  else if (rightMotor > 0){
    digitalWrite(RIGHT1, HIGH);
    digitalWrite(RIGHT2, LOW);
  }
  else{
    digitalWrite(RIGHT1, LOW);
    digitalWrite(RIGHT2, LOW);
  }

  if (leftMotor < 0){
    digitalWrite(LEFT1, LOW);
    digitalWrite(LEFT2, HIGH);
  }
  else if (leftMotor > 0){
    digitalWrite(LEFT1, HIGH);
    digitalWrite(LEFT2, LOW);
  }
  else{
    digitalWrite(LEFT1, LOW);
    digitalWrite(LEFT2, LOW);
  }
}

void measure(){
  // Clears the trigPin
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  dur = pulseIn(ECHO, HIGH);
  distCM = dur * 0.034/2;
  
  delay(100);
}

void setupPins(){
  pinMode(RIGHT1, OUTPUT);
  pinMode(RIGHT2, OUTPUT);

  pinMode(LEFT1, OUTPUT);
  pinMode(LEFT2, OUTPUT);
  
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  rotateMotor(0,0);
}

void setup() {
  setupPins();
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin();

  if(esp_now_init() != ESP_OK){
    Serial.println("Error initialazing ESP-NOW");
    return;
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
  measure();

  dataSend.distance = distCM;

  esp_err_t result = esp_now_send(recMac, (uint8_t *) &dataSend, sizeof(dataSend));

  if (result == ESP_OK) 
  {
    Serial.println("Sent with success");
  }
  else 
  {
    Serial.println("Error sending the data");
  }

  delay(100);

  
  unsigned long now = millis();
  if(now - lastRecvTime > SIGNAL_TIMEOUT){
    rotateMotor(0,0);
  }
}
