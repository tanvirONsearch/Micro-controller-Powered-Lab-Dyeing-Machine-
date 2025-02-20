#include "EasyNextionLibrary.h"
#include<avr/io.h>
#include<EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int i2c_addr = 0x27;
LiquidCrystal_I2C lcd(i2c_addr, 20,4);

EasyNex monitor(Serial);
const int TIME_OUT = 1000;           // time to refresh the Nextion data every 1000 ms
unsigned long timer = millis();  // timer for refreshing Nextion's page
//const int pageTimeOut = 10*1000;
//unsigned long PageTimer = millis();

enum state{
  IDLE,
  MULTI,
  SINGLE,
  RUN
};
state now=IDLE;

// Program related values
char entireProgram[880];
bool isSaved = 0;
String CRProg=""; //current running prog
int8_t CRProgNo = -3; // will get triggered from trigger fnctions from monitor
const char* text[] = {
    "t13.txt", "t14.txt", "t15.txt", "t16.txt",
    "t17.txt", "t18.txt", "t19.txt", "t20.txt",
    "t21.txt", "t22.txt", "t23.txt", "t24.txt",
    "t25.txt", "t26.txt", "t27.txt", "t28.txt",
    "t29.txt", "t30.txt", "t31.txt", "t32.txt"
};

void setup(){
  monitor.begin(9600);

  lcd.begin(20, 4);
  lcd.backlight();
  lcd.setCursor(0, 0); //col,row
  lcd.print("This is a test");

  memset(entireProgram, 'n', sizeof(entireProgram)); //initializing array

  for(int i=0;i<880;i++){
    entireProgram[i]=(char)EEPROM.read(i);
    delay(1);
  }

  //delay(300);
  lcd.clear();
  lcd.print("Ready to go");
  //Serial.begin(9600);
  //fetch from eeprom
  
}

void loop(){
  lcd.setCursor(0, 0);
  monitor.NextionListen();
  switch(now){
    case IDLE: //trigger with 50
    //lcd.print("IDLE mode");
    break;
    case MULTI: //triggered with 49
    multiProg();
    //lcd.print("batch mode");
    break;
    case SINGLE: //triggered with 48
    //lcd.print("Single mode");
    break;
    case RUN:
    break;  }
}

void multiProg(){
  if(CRProgNo<=0)return;
  showProg(&CRProgNo);
    }

void showProg(int8_t *val) {
  char program[4];  
  int baseIndex = 80 + ((*val - 1) * 80);  

  for (int8_t i = 0; i < 20; i++) {
    memcpy(program, entireProgram + baseIndex + (i * 4), 4);
    monitor.writeStr(text[i], program);  
  }
}


/////////////////////////////////////////mode switxh triggers
void trigger48(){ //signle mode trigger
  now = SINGLE;
}
void trigger49(){ //multi mode trigger
  now = MULTI;
}
void trigger50(){ //idle mode trigger
  now = IDLE;
}
//////////////////////////////////////////// trigger to load progs
void trigger1(){
  CRProgNo=1;
}
void trigger2(){
  CRProgNo=2;
}
void trigger3(){
  CRProgNo=3;
}
void trigger4(){
  CRProgNo=4;
}
void trigger5(){
  CRProgNo=5;
}
void trigger6(){
  CRProgNo=6;
}
void trigger7(){
  CRProgNo=7;
}
void trigger8(){
  CRProgNo=8;
}
void trigger9(){
  CRProgNo=9;
}
void trigger10(){
  CRProgNo=10;
}
////////////////////////////////////////////EOF trigger to load progs

String getVal(String s){// write command here
  timer = millis();
  String x="";
  while((millis()-timer) < TIME_OUT)
  {
    x = monitor.readStr(s);
    delay(20);
    if(x!="ERROR"){
      break;
    }
  }
  return x;
}

int getNum(String s){
  timer = millis();
  int x=777777;
  while((millis()-timer) < TIME_OUT)
  {
    x = monitor.readNumber(s);
    delay(20);
    if(x!=777777){
      break;
    }
  }
  return x;

}

/////////////////////////////read command and prog number and edit trigger
void trigger47(){
  CRProg = getVal("page3.command.txt");
  CRProgNo = getNum("page2.ProgNo.val"); // if not minus -1 then saved progs
  editProgram();
}

void editProgram(){
  if(CRProgNo<-1) return;
  else if(CRProgNo==-1){
    storeProgram(CRProg.c_str(),0);
  }
  else{
    storeProgram(CRProg.c_str(),CRProgNo);
  }
}

void storeProgram(const char* program, uint8_t programIndex) {
  if (programIndex >= 11) return; // Prevent overflow

  uint16_t startIdx = programIndex * 80; // Start position of the 80-byte chunk
  uint8_t bufferPos = 0; 

  for (uint8_t i = 0; program[i] != '\0'; ) {
    // Store the command letter
    entireProgram[startIdx + bufferPos++] = program[i++];

    // Store up to 3 digit numbers, if available
    uint8_t numLen = 0;
    while (isdigit(program[i]) && numLen < 3) {
      entireProgram[startIdx + bufferPos++] = program[i++];
      numLen++;
    }

    // Pad with 'x' to make total length = 4
    while (numLen++ < 3) {
      entireProgram[startIdx + bufferPos++] = '_';
    }
  }

  // Fill remaining space in the 80-byte chunk with 'x'
  while (bufferPos < 80) {
    entireProgram[startIdx + bufferPos++] = '_';
  }

  for (uint8_t i = 0; i < 80; i++) {
    EEPROM.update(startIdx + i, entireProgram[startIdx + i]);
  }
}



/*void showVal(){
  lcd.setCursor(0, 0);
  lcd.print("temp: ");
  lcd.print(temp[0]);
  lcd.print(" ");
  lcd.print(temp[1]);
  lcd.print(" ");
  lcd.print(temp[2]);
  lcd.print(" ");
  lcd.print(temp[3]);
  lcd.print(" ");
  lcd.print(temp[4]);

  lcd.setCursor(0, 1);
  lcd.print("time: ");
  lcd.print(time[0]);
  lcd.print(" ");
  lcd.print(time[1]);
  lcd.print(" ");
  lcd.print(time[2]);
  lcd.print(" ");
  lcd.print(time[3]);
  lcd.print(" ");
  lcd.print(time[4]);
  
  lcd.setCursor(0, 2);
  lcd.print("grade: ");
  lcd.print(grade[0]);
  lcd.print(" ");
  lcd.print(grade[1]);
  lcd.print(" ");
  lcd.print(grade[2]);
  lcd.print(" ");
  lcd.print(grade[3]);
  lcd.print(" ");
  lcd.print(grade[4]);

  lcd.setCursor(0, 3);
  lcd.print("speed: ");
  lcd.print(speed[0]);
  lcd.print(" ");
  lcd.print(speed[1]);
  lcd.print(" ");
  lcd.print(speed[2]);
  lcd.print(" ");
  lcd.print(speed[3]);
  lcd.print(" ");
  lcd.print(speed[4]);


}*/

