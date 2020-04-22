/**
  SPGBoat 2020
  Исходный код катера
  Автор: @k0ndrateff
  Инженер: @bobr137
  Версия: 4.1 22/04/20
*/

#include <TinyGPS.h>
#include <SoftwareSerial.h>  
#include <iarduino_I2C_Relay.h>      
#include <Servo.h> 
#include <SPI.h>                                               
#include <nRF24L01.h>                                          
#include <RF24.h>         

//Определение модулей                                             
SoftwareSerial nss(6, 7);
TinyGPS gps;
Servo servo;
RF24 radio(7, 10);

iarduino_I2C_Relay pwrkey(0x09);

//GPS данные
long lat, lon;
unsigned long fix_age, time, date, speed, course;
unsigned long chars;
unsigned short sentences, failed_checksum;
int data[5];

//Константы сервопривода
const int center = 45;
const int left = 5;
const int right = 80;
                                        
void setup() {       
  //Инициализация GPS                   
  Serial.begin(9600);                      
  nss.begin(9600);

  //Инициализация силового ключа
  pwrkey.begin();     
  pwrkey.digitalWrite(ALL_CHANNEL, LOW);   

  servo.attach(6);  

  //Инициализация радиомодуля
  radio.begin();                                             
  radio.setChannel      (27);                                
  radio.setDataRate     (RF24_1MBPS);                        
  radio.setPALevel      (RF24_PA_MAX);                       
  radio.openReadingPipe (1, 0xAABBCCDD11LL);    
  radio.startListening  ();        
}                                       
                                       
void loop() {      
  //GPS получение данных               
  if (nss.available())
  {
    int c = nss.read();
    if (gps.encode(c))
    {
      gps.get_position(&lat, &lon, &fix_age);
      gps.get_datetime(&date, &time, &fix_age);
      
      speed = gps.speed();
      course = gps.course();
    }
  } 
  
  //Получение данных с пульта
  if (radio.available())  {
    radio.read( &data, sizeof(data) );  
    Serial.print(data[0]);                     
  }

  //Ручное управление
  if (data[0] == 2) {
    servo.write(left);
    pwrkey.digitalWrite(1, HIGH);
  }
  else if (data[0] == 3) {
    servo.write(left);
    pwrkey.digitalWrite(1, LOW);
  }
  else if (data[0] == 4) {
    servo.write(left); 
    pwrkey.digitalWrite(1, LOW);
  }
  else if (data[0] == 5) {
    servo.write(right);
    pwrkey.digitalWrite(1, HIGH);
  }
  else if (data[0] == 6) {
    servo.write(right);
    pwrkey.digitalWrite(1, LOW);
  }
  else if (data[0] == 7) {
    servo.write(right); 
    pwrkey.digitalWrite(1, LOW);
  }
  else if (data[0] == 8) {
    servo.write(center);
    pwrkey.digitalWrite(1, LOW);
  }
  else if (data[0] == 9) {
    servo.write(center);
    pwrkey.digitalWrite(1, HIGH);
  }
  
  delay(100);
}                           

