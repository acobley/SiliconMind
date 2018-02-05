
#include "SPI.h"

boolean LED1 =false;
int count=0;
int ENOUT=7;
int DAOUT=1;
int DBOUT=0;
int A0OUT=4;
int A1OUT=5;
int A2OUT=6;

void writeOut(){
  
  if (LED1==false){
      digitalWrite(DBOUT,false);
      
  }    
  else {
     digitalWrite(DBOUT,true);
  }
  digitalWrite(A2OUT,true);
  digitalWrite(A1OUT,false);
  digitalWrite(A0OUT,true);
  

  
}

void setup() {
  // put your setup code here, to run once:
  LED1=true;
  pinMode(ENOUT,OUTPUT);
   pinMode(DAOUT,OUTPUT);
    pinMode(DBOUT,OUTPUT);
  pinMode(A0OUT,OUTPUT);
  pinMode(A1OUT,OUTPUT);
  pinMode(A2OUT,OUTPUT);
  digitalWrite(ENOUT,true);
}



void loop() {
  // put your main code here, to run repeatedly:
  count ++;
  if (count <10000){
    LED1=true;
    
  }else{
    LED1=false;
    if (count >20000){
      count=0;
    }
  }
  writeOut();
  

}
