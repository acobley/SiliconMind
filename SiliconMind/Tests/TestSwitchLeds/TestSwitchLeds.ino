


boolean LED1 =false;
int count=0;
int ENOUT=7;
int DAOUT=1;
int DBOUT=0;
int A0OUT=4;
int A1OUT=5;
int A2OUT=6;

void writeOut(){
  digitalWrite(ENOUT,true);
  WriteAddress(5);
  if (LED1==false){
      digitalWrite(DBOUT,false);
      digitalWrite(DAOUT,false);
      
  }    
  else {
     digitalWrite(DBOUT,true);
     digitalWrite(DAOUT,true);
  }
  digitalWrite(ENOUT,false);
  

  
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
  for (byte i=0;i<9;i++){
      WriteAddress(i);
      digitalWrite(DBOUT,false);
      digitalWrite(DAOUT,false);
  }
  digitalWrite(ENOUT,false);
  
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

void WriteAddress(byte address){
    byte bit = address & B00000001;
    if (bit >0)
       digitalWrite(A0OUT,true);
    else
        digitalWrite(A0OUT,false);
    bit = address & B00000010;
    if (bit >0)
       digitalWrite(A1OUT,true);
    else
        digitalWrite(A1OUT,false);  
    bit = address & B00000100;
    if (bit >0)
       digitalWrite(A2OUT,true);
    else
        digitalWrite(A2OUT,false);
         
}

