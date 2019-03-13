#include "SPI.h"

byte count = 0;
byte AIN[] = {A2, A3, A4};

int DACS[2] = {10, 10};

int DAIN = A1;
int DBIN = A0;
const int MaxPoly = 4;
int ScannedKeys [MaxPoly];
boolean States[MaxPoly + 1];
int CurrentPoly = 1;
byte Octave = -1;
byte Note = -1;
int outValue = 0;
float Range = 1365.333; // (2^12/3)
int GateOut[MaxPoly] = {A5, 0, 1, 3};
const int MaxCurve=250;
int fCurve[MaxCurve];

int LEDS[3] = {8, 7, 9};
int LED3 = 9;

void setup() {

  // put your setup code here, to run once:
  for (byte A = 4; A < 7; A++) {
    pinMode(A, OUTPUT);
  }

  for (byte A = 0; A < 3; A++) {
    pinMode(AIN[A], OUTPUT);
  }

  pinMode(DAIN, INPUT);
  pinMode(DBIN, INPUT);
  pinMode(GateOut[0],OUTPUT);

  pinMode(DACS[0], OUTPUT);
  pinMode(DACS[1], OUTPUT);
  digitalWrite(DACS[0], HIGH);
  digitalWrite(DACS[1], HIGH);
  for (byte A = 0; A < 3; A++) {
    pinMode(LEDS[A], OUTPUT);
    digitalWrite(LEDS[A], true);
  }
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  double scale = 4095.0 / pow(0.1, (-1.0 / 2.0));
  int i = 0;
  for (double f = 0.1; f < (MaxCurve/10.0); f = f + 0.1) {
    double ans = scale * pow(f, (-1.0 / 2.0));
    fCurve[i] = (int)ans;
    i++;
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  ScanKeyboard();
  WriteNotesOut();

}


void WriteInAdd(byte address) {

  for (byte A = 0; A < 3; A++) {
    digitalWrite(AIN[A], (boolean)(address & 0x01));
    address >>= 1;

  }
}
void WriteLED(byte address) {
  for (byte A = 0; A < 3; A++) {
    digitalWrite(LEDS[A], (boolean)(address & 0x01));
    address >>= 1;

  }
}

void WriteAdd(byte address) {

  for (byte A = 4; A < 7; A++) {
    digitalWrite(A, (boolean)(address & 0x01));
    address >>= 1;

  }

}

boolean in1 = true;
boolean in2 = true;

byte sum = 0;
byte last = 0;
int VCount = 0;
boolean VelWritten = false;
void ScanKeyboard() {
  for (int i = 0; i < MaxPoly; i++) {  //reset scan

    States[i] = false;
    ScannedKeys[i] = -1;
  }
  int Key = -1;
  for (int i = 0; i < 8; i++) { //scan keyboard and order them low to high
    WriteAdd(i);
    for (int j = 0; j < 5; j++) {

      WriteInAdd(j);

      in1 = digitalRead(DAIN);
      in2 = digitalRead(DBIN);

      if ((in1 == true) or (in2 == true))   {
        Key = (8 * j) + i;
        sum = in1 + (in2 <<= 1);
        WriteLED(sum);
        switch (sum) {
          case 1: VCount++;
            if (VCount >MaxCurve) {
              VCount = MaxCurve;
            }
             mcpWrite(fCurve[MaxCurve-VCount], 0, 1);
            break;
          case 2:
            if (VelWritten == false) {
              VCount = VCount - 5;
              if (VCount < 0)
                VCount = 0;

              mcpWrite(fCurve[VCount], 0, 1);
              VelWritten = true;


            }

            break;
          default: break;
        }
        for (int l = 0; l < MaxPoly; l++) {//This is an sort routine in case the notes do not come in order
          if (Key > ScannedKeys[l]) {
            if (ScannedKeys[l] == -1) {
              ScannedKeys[l] = Key;
              l = MaxPoly;
            }
          } else {
            //Insert it by moving everything up one
            for (int m = (MaxPoly - 2); m >= l; m--) {
              ScannedKeys[m + 1] = ScannedKeys[m];
            }
            ScannedKeys[l] = Key;
            l = MaxPoly;
          }

        }
      }
    }

  }
  if (Key == -1) {
    WriteLED(0);
    VCount = 0;
    VelWritten = false;
  }
}


void mcpWrite(int value, int DAC, int Channel) {
  //CS
  if (DAC == 0)
    digitalWrite(DACS[0], LOW);
  else
    digitalWrite(DACS[0], HIGH);
  //DAC1 write
  //set top 4 bits of value integer to data variable
  byte data = value >> 8;
  data = data & B00001111;
  if (Channel == 0)
    data = data | B00110000; //DACA Bit 15 Low
  else
    data = data | B10110000; //DACB Bit 15 High
  SPI.transfer(data);
  data = value;
  SPI.transfer(data);
  // Set digital pin DACCS HIGH
  if (DAC == 0)
    digitalWrite(DACS[0], HIGH);
  else
    digitalWrite(DACS[0], LOW);

}


void WriteNotesOut() {

  int CurrentFinger = 0;
  int Key = -1;
  for (int i = 0; i < 1; i++) { //Write gate and voltage
    CurrentFinger = i;
    Key = ScannedKeys[i];
    if (Key != -1) {
      Octave = (byte)(Key / 12);
      Note = (byte)(Key % 12);
      outValue = (int)(Range * (Octave + (float)Note / 12));
      digitalWrite(LED3, HIGH);
      mcpWrite(outValue, 0, 0);

      digitalWrite(GateOut[CurrentFinger], true);

    }
    else {
      digitalWrite(LED3, LOW);
      digitalWrite(GateOut[CurrentFinger], false);
    }
  }
}

