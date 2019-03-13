#include "SPI.h"

byte count = 0;
byte AIN[] = {A2, A3, A4};

int DACS[2] = {10, 10};
int ButLED1 = 8;
int ButLED2 = 7;
int DAIN = A1;
int DBIN=A0;
const int MaxPoly = 4;
int ScannedKeys [MaxPoly];
boolean States[MaxPoly + 1];
int CurrentPoly = 1;
byte Octave = -1;
byte Note = -1;
int outValue = 0;
float Range = 1365.333; // (2^12/3)
int GateOut[MaxPoly] = {A5, 0, 1, 3};

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

  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  pinMode(DACS[0], OUTPUT);
  pinMode(DACS[1], OUTPUT);
  digitalWrite(DACS[0], HIGH);
  digitalWrite(DACS[1], HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  ScanKeyboard();
  WriteNotesOut();
}


void WriteNotesOut() {

  int CurrentFinger = 0;
  int Key = -1;
  int PlayOffset = 0;
  int MaxLoop = CurrentPoly;
  for (int i = 0; i < MaxLoop; i++) { //Write gate and voltage
    CurrentFinger = i;
    Key = ScannedKeys[i];
    if (Key != -1) {
      Octave = (byte)(Key / 12);
      Note = (byte)(Key % 12);
      outValue = (int)(Range * (Octave + (float)Note / 12));
      
      mcpWrite(outValue,0,0);
  mcpWrite(outValue,1,0);
  mcpWrite(outValue,0,1);
  mcpWrite(outValue,1,1);
      digitalWrite(GateOut[CurrentFinger], true);
   
    }
  }
}



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
        boolean in = true;
        in = digitalRead(DAIN);
        if (in == true)   {
          Key = (8 * j) + i;

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
