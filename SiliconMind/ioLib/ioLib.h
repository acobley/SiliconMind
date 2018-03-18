#include "SPI.h"

int DACS[2] = {9,10};
int ENOUT=7;
int ENIN=A7;
int DAOUT=1;
int DBOUT=0;
int DAIN=A2;
int DBIN=A3;
int AOUT[] = {4, 5, 6};
int AIN[]={A4,A5,A6};

int BUTLED1=5;
int BUTLED2=6;

int BUT1=6;
int BUT2=7;

void setup() {
  // put your setup code here, to run once:
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
}

void loop() {
  // put your main code here, to run repeatedly:

}

void WriteOutAdd(byte address) {
    address=address-1;
  for (byte A = 0; A < 3; A++) {
    digitalWrite(AOUT[A], (boolean)(address & 0x01));
    address >>= 1;

  }

}

void WriteInAdd(byte address) {
    address=address-1;
  for (byte A = 0; A < 3; A++) {
    digitalWrite(AIN[A], (boolean)(address & 0x01));
    address >>= 1;

  }

}


boolean digitalReadPort(int channel, int port){
  boolean state=false;
   WriteInAdd(port);
  if (channel=DAIN){   
      digitalWrite(ENIN, true);
      if (digitalRead(DAIN)==true)
         state=true;
       digitalWrite(ENIN, false);
  }else{    
      digitalWrite(ENIN, true);
      if (digitalRead(DBIN)==true)
         state=true;
       digitalWrite(ENIN, false);
    
  }
  return state;
}


void mcpWrite(int value, int DAC,int Channel) {
  //CS

  digitalWrite(DACS[DAC], LOW);
  //DAC1 write
  //set top 4 bits of value integer to data variable
  byte data = value >> 8;
  data = data & B00001111;
  if (Channel==0)
     data = data | B00110000; //DACA Bit 15 Low
  else
     data = data | B10110000; //DACB Bit 15 High
  SPI.transfer(data);
  data = value;
  SPI.transfer(data);
  // Set digital pin DACCS HIGH
  digitalWrite(DACS[DAC], HIGH);



}
