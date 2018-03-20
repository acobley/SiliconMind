#include "SPI.h"
#define MONO 1
#define SPLIT 2
#define POLY 3

const int MaxPoly = 4;

int CurrentPoly = 4;
int DACS[2] = {10,9};
byte AIN[] = {A2, A3, A4};
int ButLED1 = 8;
int ButLED2 = 7;
int DAIN = A1;

//int Range = 819; // (2^12/5)
int Range = 988; // (2^12/4.147)


int KeyPressed[MaxPoly];


byte Octave = -1;
byte Note = -1;
int outValue = 0;

int mode = MONO;

boolean States[MaxPoly];
int GateOut[MaxPoly] = {A5, 0, 1, 3};

void setup() {
  for (byte A = 4; A < 7; A++) {
    pinMode(A, OUTPUT);
  }

  for (byte A = 0; A < 3; A++) {
    pinMode(AIN[A], OUTPUT);
  }

  for (int i = 0; i < MaxPoly; i++) {
    pinMode(GateOut[i], OUTPUT);
    digitalWrite(GateOut[i], false);

    States[i] = false;
    KeyPressed[i] = -1;
  }
  for (int i = 0; i < 2; i++) {
    pinMode(DACS[i], OUTPUT); //DAC Chip Select
    digitalWrite(DACS[i], HIGH);
  }
  if (mode == MONO)
    CurrentPoly = 1;
  else if (mode == SPLIT)
    CurrentPoly = 4;
  else if (mode == POLY)
    CurrentPoly = MaxPoly;
  pinMode(ButLED1, OUTPUT);
  pinMode(ButLED2, OUTPUT);
  digitalWrite(ButLED1, HIGH);
  digitalWrite(ButLED2, HIGH);
  pinMode(DAIN, INPUT);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  delay(500);
  digitalWrite(ButLED1, LOW);
  digitalWrite(ButLED2, LOW);
}

void loop() {


  int CurrentKeys [MaxPoly];

  for (int i = 0; i < MaxPoly; i++) {  //reset scan
    States[i] = false;
    CurrentKeys[i] = -1;
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

    if (States[CurrentFinger] == false) {  //No Key was pressed this time round
      KeyPressed[CurrentFinger] = -1;        //Set the KeyPressed to a default value
      digitalWrite(GateOut[CurrentFinger], false);
      digitalWrite(ButLED1, LOW);
    } else {
      digitalWrite(ButLED1, HIGH);
      digitalWrite(GateOut[CurrentFinger], true);
      Key = KeyPressed[CurrentFinger];
      Octave = (byte)(Key / 12);
      Note = (byte)(Key % 12);
      outValue = Range * (Octave + (float)Note / 12);
      
      //mcpWrite(outValue, CurrentFinger); //Send the value to the  DAC
      
      mcpWrite(outValue, CurrentFinger/2, CurrentFinger &0x01); //Send the value to the  DAC
  
    }
  }

}


//Function for writing value to DAC. 0 = Off 4095 = Full on.

void mcpWrite(int value, int DAC, int Channel) {
  //CS

  digitalWrite(DACS[DAC], LOW);
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
  digitalWrite(DACS[DAC], HIGH);



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


void flash(int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(ButLED2, HIGH);
    delay(200);
    digitalWrite(ButLED2, LOW);
    delay(200);

  }
}

