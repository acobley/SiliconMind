
int ENOUT=7;
int DAOUT=1;
int DBOUT=0;
int A0OUT=4;
int A1OUT=5;
int A2OUT=6;

/*****************
 * T0=S8A =Enout + 7
 * T1=S7A= Enout + 6
 */

void writeOut(){
 
  
  while(true){
  for (byte i=0;i<8;i++){
     digitalWrite(DAOUT,true);
    WriteAdd((byte)1);
   
     digitalWrite(ENOUT,true);

     delay(1);
     
     digitalWrite(ENOUT,false);
     
  
     delay(2);
  }
  }
  
  

  
}

void setup() {
  // put your setup code here, to run once:

  pinMode(ENOUT,OUTPUT);
   pinMode(DAOUT,OUTPUT);
    pinMode(DBOUT,OUTPUT);
  pinMode(A0OUT,OUTPUT);
  pinMode(A1OUT,OUTPUT);
  pinMode(A2OUT,OUTPUT);
  digitalWrite(ENOUT,true);
  for (byte i=0;i<9;i++){
      WriteAddress(i);
      digitalWrite(DBOUT,false);
      digitalWrite(DAOUT,false);
  }
  digitalWrite(ENOUT,false);
   writeOut();
}



void loop() {
 
  

}
/************************************
 * This routine takes a Byte, it writes the bottom Three bits so PD4,PD5,PD6
 * as binary
 * 
 * PD4=A0
 * PD5=A1
 * PD6=A2
 */
int AOUT[]={4,5,6};
void WriteAdd(byte address){
   for (byte A=4;A<7;A++){   
      digitalWrite(A, (boolean)(address & 0x01));
      address>>1;
      
   }
  
}

void WriteAddress(byte address){
    byte bit = address & 0x01;
    if (bit >0)
       digitalWrite(A0OUT,true);
    else
        digitalWrite(A0OUT,false);
    bit = address & 0x02;
    if (bit >0)
       digitalWrite(A1OUT,true);
    else
        digitalWrite(A1OUT,false);  
    bit = address & 0x04;
    if (bit >0)
       digitalWrite(A2OUT,true);
    else
        digitalWrite(A2OUT,false);
         
}

