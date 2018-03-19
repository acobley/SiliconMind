
int ENOUT = 7;
int DAOUT = 1;
int DBOUT = 0;
int A0OUT = 4;
int A1OUT = 5;
int A2OUT = 6;

int count = 0;

/************************************
   This routine takes a Byte, it writes the bottom Three bits so PD4,PD5,PD6
   as binary

   PD4=A0
   PD5=A1
   PD6=A2
*/
int AOUT[] = {4, 5, 6};
void WriteAdd(byte address) {
  for (byte A = 0; A < 3; A++) {
    digitalWrite(AOUT[A], (boolean)(address & 0x01));
    address >>= 1;

  }

}


void setup() {
  // put your setup code here, to run once:

  pinMode(ENOUT, OUTPUT);
  pinMode(DAOUT, OUTPUT);
  pinMode(DBOUT, OUTPUT);
  for (byte A = 0; A < 3; A++) {
    pinMode(AOUT[A], OUTPUT);


  }
  digitalWrite(DAOUT, true);
WriteAdd((byte)6);

  digitalWrite(ENOUT, true);
delay(1000);

digitalWrite(ENOUT, false);
}

/*****************
   T0=S8A =Enout + 7
   T1=S7A= Enout + 6
*/

void loop() {

  WriteAdd((byte)count);

  digitalWrite(ENOUT, true);
  delay(500);
   digitalWrite(ENOUT, false);
delay(200);
  

  count++;
  if (count > 7)
    count = 0;
}




