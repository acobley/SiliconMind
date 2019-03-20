

byte AIN[] = {A2, A3, A4};
int LEDS[3] = {7,8, 9};
int DAIN = A1;
int DBIN = A0;

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
  for (byte A = 0; A < 3; A++) {
    pinMode(LEDS[A], OUTPUT);
    digitalWrite(LEDS[A], true);
  }
  pinMode(DAIN,INPUT);

  delay(1000);
  for (byte A = 0; A < 3; A++) {
    pinMode(LEDS[A], OUTPUT);
    digitalWrite(LEDS[A], false);
  }
  
  
  
}

void loop() {
  // put your main code here, to run repeatedly:

  WriteInAdd(7);
  if (digitalRead(DAIN) ==false)
     digitalWrite(LEDS[0],true);
  else
     digitalWrite(LEDS[0],false);
  WriteInAdd(6);
  if (digitalRead(DAIN) ==false)
     digitalWrite(LEDS[1],true);
  else
     digitalWrite(LEDS[1],false); 
  WriteInAdd(5);
  if (digitalRead(DAIN) ==false)
     digitalWrite(LEDS[2],true);
  else
     digitalWrite(LEDS[2],false); 
  

}

