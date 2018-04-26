#include "SPI.h"
#include <EEPROM.h>

#define MONO 3
#define SPLIT 2
#define POLY 1

#define NONE 0
#define PLAY 1
#define RECORD 2

const int MaxPoly = 4;
int SPLITKEY = 16;
const byte GateInInterrupt = 2;
boolean LedState = false;

int CurrentPoly = 4;
int DACS[2] = {10, 9};
byte AIN[] = {A2, A3, A4};
int ButLED1 = 8;
int ButLED2 = 7;
volatile int LedFlashCount1 = 0;
volatile int LedFlashCount2 = 0;
int But1 = 6;
int But2 = 7;
boolean But1State = false;
boolean But2State = false;
int RecordMode = NONE;

int mode = POLY;
int DAIN = A1;
float PortRate = 0.25; //Initial Port
float Range = 1365.333; // (2^12/3)

/*
   Calculation is Range * (Octave + (float)Note / 12);
   So Range is steps per Octave
*/

int AssignedKeyPressed[MaxPoly + 1];
float CurrentOutValue[MaxPoly + 1];
int CurrentTarget[MaxPoly + 1];
boolean States[MaxPoly + 1];
boolean CurrentGates[MaxPoly + 1];
int ScannedKeys [MaxPoly];

byte Octave = -1;
byte Note = -1;
int outValue = 0;


int GateOut[MaxPoly] = {A5, 0, 1, 3};
int MAXSEQ = 64;
volatile int SequenceNotes[64];
volatile int SequenceGates[64];
volatile int SequenceLength = 0;
volatile int CurrentSequenceNum = 0;


/*
   This is only needed if no 3V ref is used.
*/
void calcRange() {
  Range = 4096.0 / (4.0 + (float)analogRead(A0) / 1024.0);

}

void getPortRate() {//  This needs some work to get a full range
  PortRate = (float)(analogRead(A0) / 1024.0) * 250;

}

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
    AssignedKeyPressed[i] = -1;
    CurrentOutValue[i] = 0;
    CurrentTarget[i] = 0;
    CurrentGates[i] = false;
  }
  for (int i = 0; i < 2; i++) {
    pinMode(DACS[i], OUTPUT); //DAC Chip Select
    digitalWrite(DACS[i], HIGH);
  }
  if (mode == MONO)
    CurrentPoly = 1;
  else if (mode == SPLIT)
    CurrentPoly = 2;
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
  ReadEEProm();
  attachInterrupt(digitalPinToInterrupt(GateInInterrupt), HandleClock, FALLING);
}

volatile int hKey ;
volatile int hOctave;
volatile int hNote;
volatile int houtValue;
void HandleClock() {

  if (RecordMode != NONE) {
    if (RecordMode == PLAY) {

      LedFlashCount1 = 1;

      hKey = SequenceNotes[CurrentSequenceNum];
      hOctave = (byte)(hKey / 12);
      hNote = (byte)(hKey % 12);
      houtValue = (int)(Range * (hOctave + (float)hNote / 12));
      digitalWrite(GateOut[0], SequenceGates[CurrentSequenceNum]);
      mcpWrite(houtValue, 0, 0); //Send the value to the  DAC
      CurrentSequenceNum++;
      if (CurrentSequenceNum >= SequenceLength) {
        CurrentSequenceNum = 0;
      }



      return;

    } else {
      //digitalWrite(ButLED2, LedState);
      LedFlashCount2 = 1;
      SequenceNotes[CurrentSequenceNum] = AssignedKeyPressed[0];
      SequenceGates[CurrentSequenceNum] = CurrentGates[0];
      SequenceLength = CurrentSequenceNum + 1;
      CurrentSequenceNum++;
      if (CurrentSequenceNum > MAXSEQ) {
        CurrentSequenceNum = MAXSEQ;
      }
    }
  }
}

void FlashLeds() {
  if (LedFlashCount1 != 0) {
    digitalWrite(ButLED1, HIGH);
    LedFlashCount1++;
    if (LedFlashCount1 > 100)
      LedFlashCount1 = 0;
  } else {
    digitalWrite(ButLED1, LOW);
  }
  if (LedFlashCount2 != 0) {
    digitalWrite(ButLED2, HIGH);
    LedFlashCount2++;
    if (LedFlashCount2 > 100)
      LedFlashCount2 = 0;
  } else {
    digitalWrite(ButLED2, LOW);
  }
}

int HoldCount = 0;
void getRecordMode() {

  boolean State = GetSwitchState(But2);
  if ((But2State == false) && (State == true)) {
    if (RecordMode == NONE) {
      RecordMode = PLAY;
      But2State = State; //true
      CurrentSequenceNum = 0;
    } else {
      RecordMode = NONE;
      But2State = State;
      digitalWrite(ButLED2, false);
      SaveEEProm();
    }

  }
  if ((But2State == true) && (State == true)) {
    HoldCount++;
    if (HoldCount > 2000) {

      RecordMode = RECORD;
      HoldCount = 2001;
      flash(4, ButLED2);
      CurrentSequenceNum = 0;
    }

  }
  if (State == false) {
    But2State = false;
    HoldCount = 0;

  }

}

void SetPolyMode() {
  boolean State = GetSwitchState(But1);
  if ((But1State == false) && (State == true)) {
    //Change the mode
    mode++;
    if (mode > MONO) {
      mode = POLY;
    }
    flash(mode, ButLED1);
    if (mode == MONO)
      CurrentPoly = 1;
    else if (mode == SPLIT)
      CurrentPoly = 2;
    else if (mode == POLY)
      CurrentPoly = MaxPoly;
  }
  But1State = State;
}



void loop() {
  FlashLeds();
  SetPolyMode();
  getPortRate();
  getRecordMode();
  ScanKeyboard();
  AssignVoices();
  WriteNotesOut();
}

/*
   This needs some work, keys will move about voices as notes are added.
*/
void AssignVoices() {
  int newKeyPressed[] = { -1, -1, -1, -1};
  int CurrentFinger = -1;
  //Test out last keys to current one

  int Key = -1;
  //Find notes this time the same as last and keep a list
  int KeptFingers[] = { -1, -1, -1, -1};
  int newKeys[] = { -1, -1, -1, -1};

  FindKeptFingers(KeptFingers);

  FindNewNotes(newKeys);
  //report(KeptFingers,newKeys);

  SetKeptKeys(KeptFingers, newKeyPressed);
  SetNewKeys(newKeys, newKeyPressed);

  for (int k = 0; k < 4; k++) {
    AssignedKeyPressed[k] = newKeyPressed[k];
    if (newKeyPressed[k] != -1)
      States[k] = true;
  }

}


void SetNewKeys(int newKeys[], int newKeyPressed[]) {
  for (int k = 0; k < 4; k++) {
    if (newKeys[k] != -1) {
      for (int j = 0; j < 4; j++) { //find a free slot
        if (newKeyPressed[j] == -1) {
          newKeyPressed[j] = ScannedKeys[newKeys[k]];
          break;
        }
      }
    }
  }
}

void SetKeptKeys(int KeptFingers[], int newKeyPressed[]) {
  for (int k = 0; k < 4; k++) {
    if (KeptFingers[k] != -1) {
      newKeyPressed[KeptFingers[k]] = AssignedKeyPressed[KeptFingers[k]];
    }
  }
}

int LastNewNotes[] = { -1, -1, -1, -1};
void FindNewNotes(int newKeys[]) {
  //Find new notes
  int ptrNew = 0;
  int Key = -1;
  for (int i = 0; i < CurrentPoly; i++) {
    Key = ScannedKeys[i];
    if (Key != -1) {
      boolean Found = false;
      for (int j = 0; j < CurrentPoly; j++) {
        if (Key == AssignedKeyPressed[j]) {
          Found = true;
          break; //Go onto next key
        }

      }
      if (Found == false) {
        //System.out.println(ptrKept + "   Key " + Key);
        if (LastNewNotes[ptrNew] != -1) {
          newKeys[ptrNew] = i;
          LastNewNotes[ptrNew] = -1;
          ptrNew++;

        } else {
          LastNewNotes[ptrNew] = i;
          ptrNew++;
        }
      }
    }
  }
}

void FindKeptFingers(int KeptFingers[]) {
  int Key = -1;
  int ptrKept = 0;//Find notes this time the same as last and keep a list
  for (int i = 0; i < CurrentPoly; i++) {
    Key = AssignedKeyPressed[i];
    for (int j = 0; j < CurrentPoly; j++) {
      if ((Key == ScannedKeys[j]) && (ScannedKeys[j] != -1)) {
        KeptFingers[ptrKept] = i;
        ptrKept++;
        break; //Go onto next key
      }
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



void WriteNotesOut() {

  int CurrentFinger = 0;
  int Key = -1;
  for (int i = 0; i < CurrentPoly; i++) { //Write gate and voltage

    if ((RecordMode == PLAY) && (i == 0)) {
      i++;
    }

    CurrentFinger = i;

    if (States[CurrentFinger] == false) {  //No Key was pressed this time round Deal with Gates
      Key = AssignedKeyPressed[CurrentFinger] ;
      CurrentGates[CurrentFinger] = false;
      digitalWrite(ButLED1, LOW);
      if ( mode != SPLIT) {
        digitalWrite(GateOut[CurrentFinger], false);

      } else {
        if (Key <= SPLITKEY) {

          digitalWrite(GateOut[0], false);
        } else {
          digitalWrite(GateOut[1], false);
        }
      }
      AssignedKeyPressed[CurrentFinger] = -1;        //Set the KeyPressed to a default value
    } else {
      Key = AssignedKeyPressed[CurrentFinger];
      Octave = (byte)(Key / 12);
      Note = (byte)(Key % 12);
      outValue = (int)(Range * (Octave + (float)Note / 12));
      digitalWrite(ButLED1, HIGH);
      if ( mode != SPLIT) {
        /* Most of this deals with Portemantau

        */
        CurrentTarget[CurrentFinger] = outValue;
        int delta = outValue - CurrentOutValue[CurrentFinger];
        int Sign = 1;
        if (delta < 0)
          Sign = -1;
        CurrentOutValue[CurrentFinger] = CurrentOutValue[CurrentFinger] + Sign * PortRate;
        if (Sign > 0) {
          if (CurrentOutValue[CurrentFinger] >= CurrentTarget[CurrentFinger])
            CurrentOutValue[CurrentFinger] = CurrentTarget[CurrentFinger];
        }
        else {
          if (CurrentOutValue[CurrentFinger] <= CurrentTarget[CurrentFinger])
            CurrentOutValue[CurrentFinger] = CurrentTarget[CurrentFinger];
        }
        mcpWrite(CurrentOutValue[CurrentFinger], CurrentFinger / 2, CurrentFinger & 0x01); //Send the value to the  DAC
        digitalWrite(GateOut[CurrentFinger], true);
        CurrentGates[CurrentFinger] = true;
      } else {
        if (Key <= SPLITKEY) {
          digitalWrite(GateOut[0], true);
          mcpWrite(outValue, 0, 0); //Send the value to the  Out 0
        } else {
          digitalWrite(GateOut[1], true);
          mcpWrite(outValue, 0, 1); //Send the value to the  Out 1
        }
      }
    }
  }
}

//Function for writing value to DAC. 0 = Off 4095 = Full on.

void mcpWrite(int value, int DAC, int Channel) {
  //set top 4 bits of value integer to data variable
  byte data = value >> 8;
  data = data & B00001111;
  if (Channel == 0)
    data = data | B00110000; //DACA Bit 15 Low
  else
    data = data | B10110000; //DACB Bit 15 High

  cli();
  digitalWrite(DACS[DAC], LOW);
  SPI.transfer(data);
  data = value;
  SPI.transfer(data);
  // Set digital pin DACCS HIGH
  digitalWrite(DACS[DAC], HIGH);
  sei();


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

/* debug code */
void flash(int count, int Button) {
  for (int i = 0; i < count; i++) {
    digitalWrite(Button, HIGH);
    delay(200);
    digitalWrite(Button, LOW);
    delay(200);

  }
}

boolean GetSwitchState(int Switch) {
  WriteInAdd(Switch);
  if (digitalRead(DAIN) == false) {
    return true;
  }
  return false;

}

struct SaveStruct {
  int SequenceNotes[64];
  int SequenceGates[64];
  int SequenceLength = 0;
};

int eeAddress = 0;
SaveStruct SaveNotes ;

void SaveEEProm() {
  for (int i = 0; i < 64; i++) {
    SaveNotes.SequenceNotes[i] = SequenceNotes[i];
    SaveNotes.SequenceGates[i] = SequenceGates[i];

  }
  SaveNotes.SequenceLength = SequenceLength;
  EEPROM.put(eeAddress, SaveNotes);

}

void ReadEEProm() {
  EEPROM.get(eeAddress, SaveNotes);
  for (int i = 0; i < 64; i++) {
    SequenceNotes[i] = SaveNotes.SequenceNotes[i];
    SequenceGates[i] = SaveNotes.SequenceGates[i];

  }
  SequenceLength = SaveNotes.SequenceLength;
}

