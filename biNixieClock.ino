
#include <Wire.h>
#include "RTClib.h";

// ********* BEGIN USER CONFIG AREA **********

// millisecs to keep digits off
#define INTER_DIGIT_OFF_TIME 70

// how long to keep digits powered during normal operation
#define DIGIT_ON_TIME 1000

// how slow the increasing count should go when setting HH or MM
#define SET_BUTTON_DELAY 500

// delay between display cycles
#define LOOP_OFF_TIME 3300

// *********  END USER CONFIG AREA  **********

RTC_DS1307 rtc;

DateTime now;

#define NBR_ANODE_PINS 1
#define NBR_DECADE_PINS 4
#define BUTTON_HH 15
#define BUTTON_MM 18


int anodePins[NBR_ANODE_PINS] = {
  10};
int bigDecadePins[NBR_DECADE_PINS] = {
  9,7,6,8}; //LSB first
int smallDecadePins[NBR_DECADE_PINS] = {
  5,16,14,4}; //LSB first

int bigTime[NBR_ANODE_PINS]; // hrs and tens of minutes from RTC
int smallTime[NBR_ANODE_PINS]; // minutes and seconds from RTC

void setup(){

  for(int i=0; i < NBR_ANODE_PINS; i++)
    pinMode( anodePins[i],OUTPUT);    
  for(int i=0; i < NBR_DECADE_PINS; i++){
    pinMode( bigDecadePins[i],OUTPUT);  
    pinMode( smallDecadePins[i],OUTPUT);
  }

  allOFF();
  
  pinMode( BUTTON_HH,INPUT_PULLUP);
  pinMode( BUTTON_MM,INPUT_PULLUP);

  Serial.begin(9600);   // you never know ;-)

  delay(1000);   
  dePoison();
  delay(1000);   

  Wire.begin();   
  rtc.begin();

  if (! rtc.isrunning()) {
    // display a static "99" error code
    // ERROR99: no RTC!
    writeDecade(true,99);
    writeDecade(false,99);
    //Serial.println("RTC is NOT running!");

    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(__DATE__, __TIME__));
    delay(3000);
  }


} // END SETUP


void dePoison() {
  byte kk;

  allOFF();
  
//  for(int k=0; k<100; k=k+11){
//    kk = decToBcd(k);
//    writeDecade(true,kk);
//    writeDecade(false,kk);
//    Serial.println(kk);
//    delay(150);
//  }
  
  for(int k=0; k<100; k=k+10){
    kk = decToBcd(k);
    writeDecade(true,kk);
    delay(150);
  }
  
  allOFF();
  
  for(int k=0; k<100; k=k+11){
    kk = decToBcd(k);
    writeDecade(false,kk);
    delay(150);
  }

  allOFF();    
}

// function adapted from
// http://www.codingcolor.com/microcontrollers/an-arduino-lcd-clock-using-a-chronodot-rtc/
void checkButtons()
{
  int hourButtonState = digitalRead(BUTTON_HH);
  int minButtonState = digitalRead(BUTTON_MM);
  // Note: reading both buttons is an overkill but maybe
  // in the future we want to trigger a function with both
  // buttons pressed, so let it be.

  if(hourButtonState == LOW){ // we have input pullup enabled, so a pressed button is low
    setHours();
    delay(INTER_DIGIT_OFF_TIME);
  }

  if(minButtonState == LOW){ // we have input pullup enabled, so a pressed button is low
    setMinutes();
    delay(INTER_DIGIT_OFF_TIME);
  }
}


// set minutes starting from current value
void setHours()
{
  int minButtonState;

  byte myhour = now.hour(); // get a local copy of the current minute

  do {
      
    myhour++; // +1
    if (myhour > 23) myhour = 0; // wrap around :-)
    
    byte hourBCD = decToBcd(myhour);
    writeDecade(true, hourBCD);
    writeDecade(false, hourBCD);

    delay(SET_BUTTON_DELAY);
    minButtonState = digitalRead(BUTTON_HH);
    
    // if the button is still pressed  
  } 
  while (minButtonState == LOW);

  // write the TIME value to RTC
  // but first re-read the current hour!!
  now = rtc.now();
  rtc.adjust(DateTime(now.year(), now.month(), now.day(), myhour, now.minute(), now.second() )); // set the hours!
  
} // end of setHours


// set minutes starting from current value
void setMinutes()
{

  int minButtonState;
  
  byte mymin = now.minute(); // get a local copy of the current minute

  do {
      
    mymin++; // +1
    if (mymin > 59) mymin = 0; // wrap around :-)
    
    byte minBCD = decToBcd(mymin);
    writeDecade(true,minBCD);
    writeDecade(false,minBCD);

    delay(SET_BUTTON_DELAY);
    minButtonState = digitalRead(BUTTON_MM);

    // if the button is still pressed  
  } 
  while (minButtonState == LOW);

  // write the TIME value to RTC
  // but first re-read the current hour!!
  now = rtc.now();
  rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), mymin, 0 )); // set the minutes!
  
} // end of setMinutes

void writeAnode(int tube){  
  // turn the given tube on, all others off  
  for(int i=0; i < NBR_ANODE_PINS; i++)
    digitalWrite(anodePins[i],LOW); // turn off all pins
  digitalWrite(tube,HIGH);  // turn on given tube      
}


// send an invalid combination to the BCD decoder
// so that all lines go high and nothing is shown
// better replaced with a TBD anodeOFF() function
// that needs a HW mod as well
void allOFF(){
  for(int i=0;i<4; i++){
    digitalWrite(bigDecadePins[i],1);
    digitalWrite(smallDecadePins[i],1);
  }
}

byte decToBcd(byte val)
{
  return ( ((val/10)*16) + (val%10) );
}

void writeDecade(boolean bigDecade, int value){  
  // set pins to reflect the given decade value, write to the big Decade if first argument is true or small Decade if false

    unsigned int mybit;
  unsigned int toPrint;
  unsigned int pos = 1;

  if(bigDecade) { // if showing 10's
    toPrint = value >> 4;
  } 
  else {  // or showing 1's
    toPrint = value; // not really needed for LSD since we take only first 4 bits, but code is cleaner
  }

  for(int i=0; i < 4; i++){
    mybit = toPrint & pos;
    if(bigDecade)
      digitalWrite(bigDecadePins[i],mybit);
    else  
      digitalWrite(smallDecadePins[i],mybit);
    pos = pos << 1;
  }    
}

void loop(){

  now = rtc.now();

  //    Serial.print(now.year(), DEC);
  //    Serial.print('/');
  //    Serial.print(now.month(), DEC);
  //    Serial.print('/');
  //    Serial.print(now.day(), DEC);
  //    Serial.print(' ');
  //    Serial.print(now.hour(), DEC);
  //    Serial.print(':');
  //    Serial.print(now.minute(), DEC);
  //    Serial.print(':');
  //    Serial.print(now.second(), DEC);
  //    Serial.println();
  //
  //    Serial.print(now.year());
  //    Serial.print('/');
  //    Serial.print(now.month());
  //    Serial.print('/');
  //    Serial.print(now.day());
  //    Serial.print(' ');
  //    Serial.print(now.hour());
  //    Serial.print(':');
  //    Serial.print(now.minute());
  //    Serial.print(':');
  //    Serial.print(now.second());
  //    Serial.println();


  //   now.year() .month() .day() .hour() .minute() .second()


  byte hourBCD = decToBcd(now.hour());
  byte minBCD = decToBcd(now.minute());
  byte secBCD = decToBcd(now.second());
  writeDecade(true,hourBCD);
  writeDecade(false,hourBCD);

  delay(DIGIT_ON_TIME);
  allOFF();
  delay(INTER_DIGIT_OFF_TIME);

  writeDecade(true,minBCD);
  writeDecade(false,minBCD);

  delay(DIGIT_ON_TIME);
  allOFF();
  delay(INTER_DIGIT_OFF_TIME);  

  writeDecade(true,secBCD);
  writeDecade(false,secBCD);
  delay(DIGIT_ON_TIME);

  //all off
  allOFF();

  if ((now.minute() == 10) && (now.second() < 6)) dePoison(); // do cathode depoison loop once an hour

  delay(LOOP_OFF_TIME);

  // hello! anyone out there willing to change the time?!
  checkButtons();

  //   for(int k=0; k<10; k++){
  //     writeDecade(true,k);
  //     for(int l=0; l<10; l++){
  //       writeDecade(false,l);
  //       delay(300);
  //     }
  //   }
  // - call the writeAnode function with the anode number
  // - call the writeDecade functions for the big and little decades with the values in the time arrays     
}





