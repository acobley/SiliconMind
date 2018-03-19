
#include "SPI.h"

/*************************************
 * Test program looking at waveform generations
 */

int DACS[2] = {9,10};
boolean LED1 =false;

float rad=0;
float sRad=0;
const float Pi = 3.14159;
float step = Pi/(2.0*8024.0);
int Value=0;
float Range = 4094.0;



void writeOut(float Value){
  int Out=(int)(Range*Value);  
  mcpWrite(Out,0,0);
}

void setup() {
  
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  pinMode(DACS[0],OUTPUT);
  pinMode(DACS[1],OUTPUT);
  digitalWrite(DACS[0], HIGH);
  digitalWrite(DACS[1], HIGH);
    
}



void loop() {
 
  float sStep= step *(0.5*cos(rad)+0.5);
  sRad=sRad+sStep;
  rad=rad+step;
  if (sRad>2.0*Pi)
     sRad=0.0;
  if (rad>2.0*Pi)
     rad=0.0;
  float wave=0.5*sin(sRad)+0.5; 
  writeOut(wave);
  


  

}


void mcpWrite(int value, int DAC,int Channel) {
  //CS

  digitalWrite(DACS[DAC], LOW);
  //DAC1 write
  //set top 4 bits of value integer to data variable
  byte data = value >> 8;
  data = data & B00001111;
  if (Channel=0)
     data = data | B00110000; //DACA Bit 15 Low
  else
     data = data | B10110000; //DACB Bit 15 High
  SPI.transfer(data);
  data = value;
  SPI.transfer(data);
  // Set digital pin DACCS HIGH
  digitalWrite(DACS[DAC], HIGH);



}
