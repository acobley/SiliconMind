
int LEDS[3]={8,7,9};
int count=0;
void setup() {
  for (byte A=0;A<3;A++){  
      pinMode(LEDS[A],OUTPUT);
      digitalWrite(LEDS[A],true);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  WriteAdd(count);
  count++;
  if (count >7)
     count =0;
  delay(1000);   
}

void WriteAdd(byte address){
   for (byte A=0;A<3;A++){   
      digitalWrite(LEDS[A], (boolean)(address & 0x01));
      address>>=1;
      
   }
}
