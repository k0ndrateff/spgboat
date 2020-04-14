/** 
    SPGBoat 2020
    This file contains source code for the controller 
    Original author: @k0ndrateff
    Mechanical engineer: @bobr137
    Version: 4.0 14/04/20
*/

#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SPI.h>                                               
#include <nRF24L01.h>                                          
#include <RF24.h>    

// 3-D Joystick constants
const int8_t Xaxis  = A1;
const int8_t Yaxis  = A2;  
const int8_t Button = 4;
uint16_t     XborderMIN = 505; 
uint16_t     XborderMAX = 515; 
uint16_t     YborderMIN = 505;   
uint16_t     YborderMAX = 525;
uint16_t     Xvol = 0, Yvol = 0; 

//Radio module define
RF24     radio(7, 10);
int      data[5];

//LCD define
LiquidCrystal_I2C lcd(0x3F, 16, 2);

//Parameters
int spd = 0;
int state = 0;
int dist = 0;
bool isAuto = 0;

void setup() {
  pinMode(Button, INPUT);

  //LCD intro
  lcd.init();
  lcd.backlight(); 
  lcd.print("SPGboat 2020");
  lcd.setCursor(0, 1);
  lcd.print("k0ndrateff");
  delay(2200);
  lcd.setCursor(0, 1);
  lcd.print("bobr137    ");
  delay(2200);
  lcd.setCursor(0, 1);
  lcd.print("Loading...");

  //Radio module init
  radio.begin           ();                                  
  radio.setChannel      (27);                                
  radio.setDataRate     (RF24_1MBPS);                        
  radio.setPALevel      (RF24_PA_MAX);                       
  radio.openWritingPipe (0xAABBCCDD11LL);
  
  delay(1500);
}

void loop() {
  // Reading joystick information
  Xvol = analogRead(Xaxis);
  Yvol = analogRead(Yaxis);
  state = computeState(Xvol, Yvol);

  //LCD Update
  lcdUpdate(state);

  //Forming data package
  if (isAuto) {
    data[0] = 0;
  }
  else if (state != 0) {
    data[0] = state;
  }
  
  //Data broadcast
  radio.write( &data , sizeof(data) );      
                                            
  delay(200);
}

/**
    Computes state of the joystick and boat direction.
*/
int computeState(Xvol, Yvol) {
  if (Xvol < XborderMIN) {                
    if (Yvol < YborderMIN) {              
      //Reverse-left 
      return 2;     
    }
    else if (Yvol > YborderMAX) {         
      //Forward-left
      return 3;         
    }
    else {
      //Left   
      return 4;        
    }
  }
  else if (Xvol > XborderMAX) {           
    if (Yvol < YborderMIN) {              
      //Reverse-right
      return 5;      
    }
    else if (Yvol > YborderMAX) {         
      //Forward-right
      return 6;
    }
    else {
      //Right 
      return 7;          
    }
  }
  else {                                  
    if (Yvol < YborderMIN) {              
      //Reverse
      return 8;            
    }
    else if (Yvol > YborderMAX) {         
      //Forward 
      return 9;             
    }
    else {
      //Centre  
      return 0;        
    }
  }
  if (digitalRead(Button)) {              
    delay(1);                             
    //Button clicked  
    return 1;   
    while (digitalRead(Button)) {}        
    delay(10);
    if (isAuto) {
      isAuto = 0;                            
    }
    else {
      isAuto = 1;
    }
  }
}

void lcdUpdate(state) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("S:"); //Speed output
  lcd.setCursor(3, 0);
  lcd.print(spd);
  lcd.setCursor(0, 1);
  lcd.print("D:"); //Distance output
  lcd.setCursor(3, 1);
  lcd.print(dist);

  lcd.setCursor(8, 1);
  if (isAuto) {
    lcd.setCursor(8, 0);
    lcd.print("AUTO");
  }
  else if (state == 2) {
    lcd.print("<<");
    lcd.setCursor(8, 0);
    lcd.print("Rev");
  }
  else if (state == 3) {
    lcd.print("<<");
    lcd.setCursor(8, 0);
    lcd.print("Forw");
  }
  else if (state == 4) {
    lcd.print("<<");
  }
  else if (state == 5) {
    lcd.print(">>");
    lcd.setCursor(8, 0);
    lcd.print("Rev");
  }
  else if (state == 6) {
    lcd.print(">>");
    lcd.setCursor(8, 0);
    lcd.print("Forw");
  }
  else if (state == 7) {
    lcd.print(">>");
  }
  else if (state == 8) {
    lcd.setCursor(8, 0);
    lcd.print("Rev");
  }
  else if (state == 9) {
    lcd.setCursor(8, 0);
    lcd.print("Forw");
  }
}
