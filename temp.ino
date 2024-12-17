/*
analogInPin-GND間に10kΩの接続推奨
voltageが0.5以下の時はRGB全てLOW、オンボードのLEDが点灯。=センサー接続？
実機でやって上だったらコード見直し
*/

const int analogInPin = A0;
const int red = 2;
const int blue = 3;
const int green = 4;
const int led = 13;

int sensorValue = 0;
int outputValue = 0;
float voltage = 0;
float temp = 0;

void setup() {
  Serial.begin(9600);
  pinMode(red,OUTPUT);
  pinMode(blue,OUTPUT);
  pinMode(green,OUTPUT); 
  pinMode(led,OUTPUT);
}

void loop() {
  sensorValue = analogRead(analogInPin);
  
  //tmp36の仕様
  voltage = sensorValue*5.0/1023.0;
  temp = (voltage-0.5)*100.0;
  
  if(temp>=25.0){
    digitalWrite(red,HIGH);
    digitalWrite(blue,LOW);
    digitalWrite(green,LOW);
    digitalWrite(led,LOW);
  }else if(temp<25.0&&temp>=21.0){
    digitalWrite(red,HIGH);
    digitalWrite(blue,LOW);
    digitalWrite(green,HIGH);
    digitalWrite(led,LOW);
  }else if(temp<21&&voltage>=0.5){
    digitalWrite(red,LOW);
    digitalWrite(blue,LOW);
    digitalWrite(green,HIGH);
    digitalWrite(led,LOW);
  }else{
    digitalWrite(red,LOW);
    digitalWrite(blue,LOW);
    digitalWrite(green,LOW);
    digitalWrite(led,LOW);
  }
  
  if(voltage<=0.5){
    digitalWrite(led,HIGH);
  }else{
    digitalWrite(led,LOW);
  }

  Serial.print("temp = ");
  Serial.print(temp);
  Serial.print("\nvoltage = ");
  Serial.print(voltage);
  Serial.print("\n");
  Serial.print(sensorValue);
  Serial.print("\n");

  delay(5000);
}