#include "SPI.h"
#define MONO 1;
#define SPLIT 2;

int ScanOut[8] = {2, 3, 4, 5, 6, 7, 8, 9};
int ScanIn[5] = {A0, A1, A2, A3, A4};
int GateOut[2] = {0, 1};

int Range = 819; // (2^12/5)
int Vss = 5;

int KeyPressed[4] = { -1, -1, -1, -1};
int DACS[2] = {10, 1};
byte Key = -1;
byte Octave = -1;
byte Note = -1;
int outValue = 0;

int mode = SPLIT;
boolean States[2];

void setup() {
  //Serial.begin(9600);
  for (int i = 0; i < 8; i++) {
    pinMode(ScanOut[i], OUTPUT);
    digitalWrite(ScanOut[i], false);
  }
  for (int i = 0; i < 5; i++) {
    pinMode(ScanIn[i], INPUT);
  }
  for (int i = 0; i < 2; i++) {
    pinMode(GateOut[i], OUTPUT);
    digitalWrite(GateOut[i], false);
    pinMode(DACS[i], OUTPUT); //DAC Chip Select
    States[i] = false;
  }
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
}

void loop() {

  for (int i = 0; i < 8; i++) {
    digitalWrite(ScanOut[i], true);
    for (int j = 0; j < 5; j++) {
      int in = digitalRead(ScanIn[j]);
      if (in == 1) {  // If any key is pressed set the gate out to true and set a flag (State)
        States[0] = true;
        digitalWrite(GateOut[0], true); // this only works in monophonic mode
      }
      Key = 8 * j + i;
      if ((in != 0) &&    //A key is pressed
          (Key != KeyPressed[0]) ) { //It's not equal to the current key (we only need to write out once)

        KeyPressed[0] = Key;       //Record this Key

      }
    }
    digitalWrite(ScanOut[i], false);
  }
  if (States[0] == false) {  //No Key was pressed this time round
    KeyPressed[0] = -1;        //Set the Gate to a default value
    digitalWrite(GateOut[0], false);
  } else {
    Key = KeyPressed[0];
    Octave = (byte)(Key / 12);
    Note = (byte)(Key % 12);
    outValue = Range * (Octave + (float)Note / 12);
    mcpWrite(outValue, 0); //Send the value to the  DAC
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
