#include "SPI.h"
#define MONO 1
#define SPLIT 2
#define POLY 3

const int MaxPoly = 2;

int CurrentPoly = 2;
int ScanOut[8] = {2, 3, 4, 5, 6, 7, 8, 9};
int ScanIn[5] = {A0, A1, A2, A3, A4};
int DACS[2] = {10, A5};


int Range = 819; // (2^12/5)
int Vss = 5;

int KeyPressed[MaxPoly];


byte Octave = -1;
byte Note = -1;
int outValue = 0;

int mode = POLY;
boolean States[MaxPoly];
int GateOut[MaxPoly] = {0, 1};

void setup() {
 
  for (int i = 0; i < 8; i++) {
    pinMode(ScanOut[i], OUTPUT);
    digitalWrite(ScanOut[i], false);
  }
  for (int i = 0; i < 5; i++) {
    pinMode(ScanIn[i], INPUT);
  }
  for (int i = 0; i < MaxPoly; i++) {
    pinMode(GateOut[i], OUTPUT);
    digitalWrite(GateOut[i], false);
    pinMode(DACS[i], OUTPUT); //DAC Chip Select
    digitalWrite(DACS[i], HIGH);
    States[i] = false;
    KeyPressed[i] = -1;
  }
  if (mode == MONO)
    CurrentPoly = 1;
  else if (mode == SPLIT)
    CurrentPoly = 2;
  else if (mode == POLY)
    CurrentPoly = MaxPoly;
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  delay(500);
}

void loop() {


  int CurrentKeys [MaxPoly];

  for (int i = 0; i < MaxPoly; i++) {  //reset scan
    States[i] = false;
    CurrentKeys[i] = -1;
  }
  int Key = -1;
  for (int i = 0; i < 8; i++) { //scan keyboard and order them low to high
    digitalWrite(ScanOut[i], true);
    for (int j = 0; j < 5; j++) {
      int in = digitalRead(ScanIn[j]);
      if (in == 1)   {
        Key = (8 * j) + i;

        for (int l = 0; l < MaxPoly; l++) {//
          if (Key > CurrentKeys[l]) {
            if (CurrentKeys[l] == -1) {
              CurrentKeys[l] = Key;
              l = MaxPoly;
            }
          } else {
            //Insert it by moving everything up one
            for (int m = (MaxPoly - 2); m >= l; m--) {
              CurrentKeys[m + 1] = CurrentKeys[m];
            }
            CurrentKeys[l] = Key;
            l = MaxPoly;
          }

        }
      }
    }
    digitalWrite(ScanOut[i], false);
  }
  int CurrentFinger = 0;

  
  for (int i = 0; i < CurrentPoly; i++) { //Assign voices
    
    Key = CurrentKeys[i];
  
    if (Key != -1) {

      
      States[CurrentFinger] = true;
      KeyPressed[CurrentFinger] = Key;       //Record this Key
      CurrentFinger++;

    }
  }

  
  for (int i = 0; i < CurrentPoly; i++) { //Write gate and voltage
    CurrentFinger = i;
     Serial.println(States[CurrentFinger] );
    if (States[CurrentFinger] == false) {  //No Key was pressed this time round
      KeyPressed[CurrentFinger] = -1;        //Set the KeyPressed to a default value
      digitalWrite(GateOut[CurrentFinger], false);
    } else {
      digitalWrite(GateOut[CurrentFinger], true);
      Key = KeyPressed[CurrentFinger];
      Octave = (byte)(Key / 12);
      Note = (byte)(Key % 12);
      outValue = Range * (Octave + (float)Note / 12);
      mcpWrite(outValue, CurrentFinger); //Send the value to the  DAC
    }
  }

}


//Function for writing value to DAC. 0 = Off 4095 = Full on.

void mcpWrite(int value, int DAC) {
  //CS

  digitalWrite(DACS[DAC], LOW);
  //DAC1 write
  //set top 4 bits of value integer to data variable
  byte data = value >> 8;
  data = data & B00001111;
  data = data | B00110000;
  SPI.transfer(data);
  data = value;
  SPI.transfer(data);
  // Set digital pin DACCS HIGH
  digitalWrite(DACS[DAC], HIGH);

}
