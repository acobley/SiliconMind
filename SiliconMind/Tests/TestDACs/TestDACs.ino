
#include "SPI.h"

int DACS[2] = {9,10};
boolean LED1 =false;

float rad=0;
const float Pi = 3.14159;
float step = Pi/(2.0*64.0);
int Value=0;
float Range = 4094.0;



void writeOut(float Value){
  int Out=(int)(Range*Value);  
  mcpWrite(Out,0,0);
}

void setup() {
  // put your setup code here, to run once:
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  pinMode(DACS[0],OUTPUT);
  pinMode(DACS[1],OUTPUT);
  digitalWrite(DACS[0], HIGH);
  digitalWrite(DACS[1], HIGH);
    
}



void loop() {
  // put your main code here, to run repeatedly:
  
  rad=rad+step;
  if (rad>2.0*Pi)
     rad=0.0;
  float wave=0.5*sin(rad)+0.5; 
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
