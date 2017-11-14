#include "SPI.h"

int Out[8] = {2, 3, 4, 5, 6, 7, 8, 9};
int In[5] = {A0, A1, A2, A3, A4};
int GateOut = 0;
int DACCS = 10;

float keyVolts[12] = {
  0,
  0.083496094,
  0.166992188,
  0.249755859,
  0.333251953,
  0.416748047,
  0.500244141,
  0.583007813,
  0.666503906,
  0.75,
  0.833496094,
  0.916992188
};

int Range = 819; // (2^12/5)
int Vss = 5;

int Gate[4] = { -1, -1, -1, -1};
byte Key = -1;
byte Octave = -1;
byte Note = -1;
int outValue = 0;


void setup() {
  //Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(GateOut, OUTPUT);
  for (int i = 0; i < 8; i++) {
    pinMode(Out[i], OUTPUT);
  }

  for (int i = 0; i < 5; i++) {
    pinMode(In[i], INPUT);
  }
  for (int i = 0; i < 8; i++) {
    digitalWrite(Out[i], false);
  }
  digitalWrite(GateOut, false);

  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  pinMode(DACCS, OUTPUT); //DAC Chip Select

}

void loop() {
  boolean State = false;
  for (int i = 0; i < 8; i++) {
    digitalWrite(Out[i], true);
    for (int j = 0; j < 5; j++) {
      int in = digitalRead(In[j]);
      if (in == 1) {  // Ifa ny key is pressed set the gate out to true and set a flag (State)
        State = true;
        digitalWrite(GateOut, true); // this only works in monophonic mode
      }
      Key = 8 * j + i;
      if ((in != 0) &&    //A key is pressed
          (Key != Gate[0]) &&  //It's not equal to the current key (we only need to write out once)
          (Gate[0]==-1)) {    //No other key has been pressed before.
        Gate[0] = Key;       //Record this Key
        Octave = (byte)(Key / 12);
        Note = (byte)(Key % 12);
 
        outValue = Range * (Octave + (float)Note/12);  

        mcpWrite(outValue);  //Send the value to the  DAC
      }
    }
    digitalWrite(Out[i], false);
  }
  if (State == false) {  //No Key was pressed this time round
    Gate[0]=-1;          //Set the Gate to a default value
    digitalWrite(GateOut, false); 
  }
}


//Function for writing value to DAC. 0 = Off 4095 = Full on.

void mcpWrite(int value) {
  //CS
  digitalWrite(DACCS, LOW);
  //DAC1 write
  //set top 4 bits of value integer to data variable
  byte data = value >> 8;
  data = data & B00001111;
  data = data | B00110000;
  SPI.transfer(data);
  data = value;
  SPI.transfer(data);
  // Set digital pin DACCS HIGH
  digitalWrite(DACCS, HIGH);

}
