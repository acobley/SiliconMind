byte count=0;

void setup() {
  // put your setup code here, to run once:
for (byte A=4;A<7;A++){  
  pinMode(A,OUTPUT);
}
}

void loop() {
  // put your main code here, to run repeatedly:
  WriteAdd(count);
  count++;
  if (count >7)
     count=0;
}


void WriteAdd(byte address){
   for (byte A=4;A<7;A++){   
      digitalWrite(A, (boolean)(address & 0x01));
      address>>=1;
      
   }
  
}
