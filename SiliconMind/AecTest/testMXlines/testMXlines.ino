byte count = 0;
byte AIN[] = {A2, A3, A4};
int ButLED1 = 8;
int ButLED2 = 7;
int DAIN = A1;

void setup() {
  // put your setup code here, to run once:
  for (byte A = 4; A < 7; A++) {
    pinMode(A, OUTPUT);
  }

  for (byte A = 0; A < 3; A++) {
    pinMode(AIN[A], OUTPUT);
  }
  pinMode(ButLED2, OUTPUT);
  pinMode(ButLED1, OUTPUT);
  pinMode(DAIN, INPUT);
  digitalWrite(ButLED2, false);
}

void loop() {
  // put your main code here, to run repeatedly:
  WriteAdd(0);
  WriteInAdd(3);
  digitalWrite(ButLED1, true);
  if (digitalRead(DAIN) == true)
    digitalWrite(ButLED2, true);
  else
    digitalWrite(ButLED2, false);
  delay(10);
  digitalWrite(ButLED1, false);
  delay(10);
  count++;
  if (count > 7)
    count = 0;
}

void WriteInAdd(byte address) {
  for (byte A = 0; A < 3; A++) {
    digitalWrite(AIN[A], (boolean)(address & 0x01));
    address >>= 1;

  }
}

void WriteAdd(byte address) {
  for (byte A = 4; A < 7; A++) {
    digitalWrite(A, (boolean)(address & 0x01));
    address >>= 1;

  }

}
