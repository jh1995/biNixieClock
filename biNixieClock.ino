
#include <Wire.h>
#include "RTClib.h";

// millisecs to keep digits off
#define INTER_DIGIT_OFF_TIME 70 

RTC_DS1307 rtc;

#define NBR_ANODE_PINS 1
#define NBR_DECADE_PINS 4

// This pin mapping algorithm comes from the 5th post at
// http://forum.arduino.cc/index.php?topic=10841.0
int anodePins[NBR_ANODE_PINS] = {10};
int bigDecadePins[NBR_DECADE_PINS] = {9,7,6,8}; //LSB first
int smallDecadePins[NBR_DECADE_PINS] = {5,16,14,4}; //LSB first

int bigTime[NBR_ANODE_PINS]; // hrs and tens of minutes from RTC
int smallTime[NBR_ANODE_PINS]; // minutes and seconds from RTC

void setup(){

  for(int i=0; i < NBR_ANODE_PINS; i++)
      pinMode( anodePins[i],OUTPUT);    
   for(int i=0; i < NBR_DECADE_PINS; i++){
      pinMode( bigDecadePins[i],OUTPUT);  
      pinMode( smallDecadePins[i],OUTPUT);
   }

   Serial.begin(9600);   

   // bootstrap sanity test, symbols carousel
   byte kk;
   for(int k=0; k<100; k=k+11){
     kk = decToBcd(k);
     writeDecade(true,kk);
     writeDecade(false,kk);
     Serial.println(kk);
     delay(100);
   }
    allOFF();
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
    // rtc.adjust(DateTime(__DATE__, __TIME__));
    delay(3000);
  }

 
 
}

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

// I found this function online, but I have no idea
// who posted it first, and where. I apologize and
// I am grateful to the author.
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
  } else {  // or showing 1's
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
  
  DateTime now = rtc.now();
  
//   now.year() .month() .day() .hour() .minute() .second()

  byte hourBCD = decToBcd(now.hour());
  byte minBCD = decToBcd(now.minute());
  byte secBCD = decToBcd(now.second());
  writeDecade(true,hourBCD);
  writeDecade(false,hourBCD);
  delay(1000);
  allOFF();
  delay(INTER_DIGIT_OFF_TIME);
  writeDecade(true,minBCD);
  writeDecade(false,minBCD);
  delay(1000);
  allOFF();
  delay(INTER_DIGIT_OFF_TIME);  
  writeDecade(true,secBCD);
  writeDecade(false,secBCD);
  delay(1000);

  //all off
  allOFF();

  delay(3000);
  
   
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



