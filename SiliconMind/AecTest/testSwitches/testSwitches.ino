byte count = 0;

byte AIN[] = {A2, A3, A4};
int ButLED1 = 8;
int ButLED2 = 7;
int DAIN=A1;

void WriteInAdd(byte address) {
  for (byte A = 0; A < 3; A++) {
    digitalWrite(AIN[A], (boolean)(address & 0x01));
    address >>= 1;

  }
}
void setup() {
  // put your setup code here, to run once:
  for (byte A = 0; A < 3; A++) {
    pinMode(AIN[A], OUTPUT);
  }
  pinMode(ButLED2, OUTPUT);
  pinMode(DAIN,INPUT);
  //digitalWrite(ButLED2,false);
    WriteInAdd(7);
  
  
}

void loop() {
  // put your main code here, to run repeatedly:

 /*
  if (digitalRead(DAIN) ==true)
     digitalWrite(ButLED2,true);
  else
     digitalWrite(ButLED2,false);
  */

}

