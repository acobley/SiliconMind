


int ENOUT = 7;
int ENIN = A7;
int DAOUT = 1;
int DBOUT = 0;
int DAIN = A2;
int DBIN = A3;
int AOUT[] = {4, 5, 6};
int AIN[] = {A4, A5, A6};

int BUTLED1 = 5;
int BUTLED2 = 6;

int BUT1 = 6;
int BUT2 = 7;
/*****************
   T0=S8A =Enout + 7
   T1=S7A= Enout + 6
   BUTLED1=SB5
   BUTLED2=SB6
*/
/************************************
   This routine takes a Byte, it writes the bottom Three bits so PD4,PD5,PD6
   as binary

   PD4=A0
   PD5=A1
   PD6=A2
*/


void WriteOutAdd(byte address) {
  address = address - 1;
  for (byte A = 0; A < 3; A++) {
    digitalWrite(AOUT[A], (boolean)(address & 0x01));
    address >>= 1;

  }

}

void WriteInAdd(byte address) {
  address = address - 1;
  for (byte A = 0; A < 3; A++) {
    digitalWrite(AIN[A], (boolean)(address & 0x01));
    address >>= 1;

  }

}


boolean digitalReadPort(int channel, int port) {
  boolean state = false;
  WriteInAdd(port);
  if (channel = DAIN) {
    digitalWrite(ENIN, true);
    if (digitalRead(DAIN) == true)
      state = true;
    digitalWrite(ENIN, false);
  } else {
    digitalWrite(ENIN, true);
    if (digitalRead(DBIN) == true)
      state = true;
    digitalWrite(ENIN, false);

  }
  return state;
}



void setup() {
  // put your setup code here, to run once:

  pinMode(ENOUT, OUTPUT);
  pinMode(ENIN, OUTPUT);

  pinMode(DAOUT, OUTPUT);
  pinMode(DBOUT, OUTPUT);
  pinMode(DAIN, INPUT);
  pinMode(DBIN, INPUT);

  for (byte A = 0; A < 3; A++) {
    pinMode(AOUT[A], OUTPUT);
    pinMode(AIN[A], OUTPUT);
  }
  digitalWrite(ENOUT, false);
  digitalWrite(DBOUT, true);
  //Do some LED flashing
  for (int i = 0; i < 10; i++) {
    digitalWrite(ENOUT, true);
    for (int j = 0; j < 100; j++) {
      WriteOutAdd(BUTLED1);
      delay(1);
      WriteOutAdd(BUTLED2);
      delay(1);
    }

    digitalWrite(ENOUT, false);
    delay(300);
  }
  digitalWrite(DBOUT, false);

}



void loop() {

  boolean sw = digitalReadPort(DAIN, BUT1);
  if (sw == true) {
    WriteOutAdd(BUTLED1);
    digitalWrite(DBOUT, true);
    digitalWrite(ENOUT, true);
  }
  else {
    WriteOutAdd(BUTLED1);
    digitalWrite(DBOUT, false);
    digitalWrite(ENOUT, false);
  }


}



