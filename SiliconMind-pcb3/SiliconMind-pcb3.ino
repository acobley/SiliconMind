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


int DACS[2] = {10, 10};
byte AIN[] = {A2, A3, A4};
int ButLED1 = 8;
int ButLED2 = 7;
int ButLED3 =9;
int LEDS[3] = {8, 7, 9};

volatile int LedFlashCount1 = 0;
volatile int LedFlashCount2 = 0;
int But1 = 6;
int But2 = 7;
boolean But1State = false;
boolean But2State = false;
int RecordMode = NONE;
int LowKeyOffset=0;
int mode = POLY;
int CurrentPoly = 4;
int DAIN = A1;
int DBIN = A0;
float PortRate = 0.25; //Initial Portemantau rate
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
int LastKeys[MaxPoly + 1];

byte Octave = -1;
byte Note = -1;
int outValue = 0;


int GateOut[MaxPoly] = {A5, 0, 1, 3};
int MAXSEQ = 64;
volatile int SequenceNotes[64];
volatile int SequenceGates[64];
volatile int SequenceGateLength[64];
volatile int SequenceLength = 0;
volatile int CurrentSequenceNum = 0;
volatile int GateLengthPtr=0;

const int MaxCurve=250;
int fCurve[MaxCurve];
int PortPot=A6;
int GatePot=A7;



//gets the portamento rate
void getPortRate() {
  float Val = (float)(analogRead(PortPot) / 1024.0);
  Val = Val * Val * Val; //Create a cubic curve
  PortRate = (float)(Val) * 250;

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
   pinMode(DACS[0], OUTPUT);
  pinMode(DACS[1], OUTPUT);
  digitalWrite(DACS[0], HIGH);
  digitalWrite(DACS[1], HIGH);
  if (mode == MONO)
    CurrentPoly = 1;
  else if (mode == SPLIT)
    CurrentPoly = 2;
  else if (mode == POLY)
    CurrentPoly = MaxPoly;
  pinMode(ButLED1, OUTPUT);
  pinMode(ButLED2, OUTPUT);
  pinMode(ButLED3, OUTPUT);
  digitalWrite(ButLED1, HIGH);
  digitalWrite(ButLED2, HIGH);
  digitalWrite(ButLED3, HIGH);
  pinMode(DAIN, INPUT);
   pinMode(DBIN, INPUT);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  delay(500);
   double scale = 4095.0 / pow(0.1, (-1.0 / 2.0));
  int i = 0;
  for (double f = 0.1; f < (MaxCurve/10.0); f = f + 0.1) {
    double ans = scale * pow(f, (-1.0 / 2.0));
    fCurve[i] = (int)ans;
    i++;
  }
  digitalWrite(ButLED1, LOW);
  digitalWrite(ButLED2, LOW);
  digitalWrite(ButLED3, LOW);
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
      houtValue = (int)(Range * (hOctave + (float)hNote / 12))+LowKeyOffset;
      digitalWrite(GateOut[0], SequenceGates[CurrentSequenceNum]);
      mcpWrite(houtValue, 0, 0); //Send the value to the  DAC
      CurrentSequenceNum++;
      if (CurrentSequenceNum >= SequenceLength) {
        CurrentSequenceNum = 0;
      }
      return;
    } else { // Record mode
      //digitalWrite(ButLED2, LedState);
      LedFlashCount2 = 1;
      SequenceNotes[CurrentSequenceNum] = AssignedKeyPressed[0];
      SequenceGates[CurrentSequenceNum] = CurrentGates[0];
      SequenceGateLength[CurrentSequenceNum]=0;
      GateLengthPtr=CurrentSequenceNum;
      CurrentSequenceNum++;
      SequenceLength = CurrentSequenceNum ;
      
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
    if (RecordMode == PLAY) {
       digitalWrite(GateOut[0],false); //Take the gate into a low state.
    }
  }
  if (LedFlashCount2 != 0) {
    digitalWrite(ButLED2, HIGH);
    LedFlashCount2++;
    if (RecordMode == RECORD){
       if (AssignedKeyPressed[0] !=-1){
          SequenceGateLength[GateLengthPtr]=LedFlashCount2;
       }
    }
    if (LedFlashCount2 > SequenceGateLength[GateLengthPtr]+1) //In record mode don't turn off until key released
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
  if (mode == MONO) {
    AssignMonoVoices();
  } 
   if (mode == SPLIT) {
    AssignSplitVoices();
  } 
  if (mode == POLY){
    AssignVoices();
  }
  WriteNotesOut();
}



void AssignMonoVoices() {
  //Find notes this time the same as last and keep a list
  //Which KeyPressed location are they
  //for each unassigned note find a location for it.
  int CurrentFinger = 0;

  int Key = -1;
  for (int i = 0; i < CurrentPoly; i++) { //Assign voices
    Key = ScannedKeys[i];
    //if (Key != -1) {
      States[CurrentFinger] = true;
      AssignedKeyPressed[CurrentFinger] = Key;       //Record this Key
      CurrentFinger++;
    //}
  }
}

void AssignSplitVoices() {
  //Find notes this time the same as last and keep a list
  //Which KeyPressed location are they
  //for each unassigned note find a location for it.
  int Key = -1;
  if (RecordMode != PLAY) {
     AssignedKeyPressed[0]=-1;
  }
  AssignedKeyPressed[1]=-1;
  for (int i = 0; i < CurrentPoly; i++) { //Assign voices
    Key = ScannedKeys[i];
    if ((Key <= SPLITKEY) && (Key>=0)){
      AssignedKeyPressed[0] = Key;       //Record this Key
      
    }
    if (Key > SPLITKEY){
      AssignedKeyPressed[1] = Key;       //Record this Key
    }
  }
}

//This is for polyphonic mode

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

        newKeys[ptrNew] = i;

        ptrNew++;


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
  int PlayOffset=0;
  int MaxLoop=CurrentPoly;
  if (RecordMode == PLAY) {//step over the sequenced note.
      PlayOffset=1;
      MaxLoop=CurrentPoly+PlayOffset;
      if (MaxLoop>MaxPoly)
         MaxLoop=MaxPoly;
    }
    
  for (int i = 0; i < MaxLoop; i++) { //Write gate and voltage
    CurrentFinger = i;
    Key = AssignedKeyPressed[CurrentFinger];
    if (Key != -1) {
      LastKeys[CurrentFinger]= AssignedKeyPressed[CurrentFinger];
      Octave = (byte)(Key / 12);
      Note = (byte)(Key % 12);
      outValue = (int)(Range * (Octave + (float)Note / 12));
      digitalWrite(ButLED1, HIGH);
      /* Most of this deals with Portamento   */
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
      if ( mode != SPLIT) {
        
        mcpWrite(CurrentOutValue[CurrentFinger], (CurrentFinger+PlayOffset) / 2, (CurrentFinger+PlayOffset) & 0x01); //Send the value to the  DAC
        digitalWrite(GateOut[CurrentFinger+PlayOffset], true);
        CurrentGates[CurrentFinger+PlayOffset] = true;
        //in Mono mode, can we write to the other notes to ?
        //if ( mode == MONO) {
        //   mcpWrite(CurrentOutValue[CurrentFinger],0,1);
        //    mcpWrite(CurrentOutValue[CurrentFinger],1,0);
        //     mcpWrite(CurrentOutValue[CurrentFinger],1,1);
        //}
      } else {
        
        if (Key <= SPLITKEY) {
          LowKeyOffset=outValue;
          if (RecordMode!=PLAY){
             digitalWrite(GateOut[0], true);
             mcpWrite(outValue, 0, 0); //Send the value to the  Out 0
          } //We are going to send the value to out 4 as well so the base note is available as well as a played note
             mcpWrite(outValue, 1, 1); //Send the value to the  Out 4
          
        } else {
          digitalWrite(GateOut[1], true);
          mcpWrite(CurrentOutValue[CurrentFinger], 0, 1); //Send the value to the  Out 1
        }
         
      }

    }

    else {
      Key= LastKeys[CurrentFinger];
      CurrentGates[CurrentFinger+PlayOffset] = false;
      digitalWrite(ButLED1, LOW);
      if ( mode != SPLIT) {
        digitalWrite(GateOut[CurrentFinger+PlayOffset], false);

      } else {
        if ((Key <= SPLITKEY) && (Key >=0)){
          if (RecordMode!=PLAY){
             digitalWrite(GateOut[0], false);
          }
        }
        if ((Key > SPLITKEY) && (Key >=0)){
          digitalWrite(GateOut[1], false);
          }
        
        LastKeys[CurrentFinger]=-1;
      }
    }
  }
}

//Function for writing value to DAC. 0 = Off 4095 = Full on.

void mcpWrite(int value, int DAC, int Channel) {
  //set top 4 bits of value integer to data variable
  if (DAC == 0)
    digitalWrite(DACS[0], LOW);
  else
    digitalWrite(DACS[0], HIGH);
  byte data = value >> 8;
  data = data & B00001111;
  if (Channel == 0)
    data = data | B00110000; //DACA Bit 15 Low
  else
    data = data | B10110000; //DACB Bit 15 High

  cli(); //Disable Interupts

  SPI.transfer(data);
  data = value;
  SPI.transfer(data);
  // Set digital pin DACCS HIGH
   // Set digital pin DACCS HIGH

  sei();
  if (DAC == 0)
    digitalWrite(DACS[0], HIGH);
  else
    digitalWrite(DACS[0], LOW);

}

void WriteLED(byte address) {
  for (byte A = 0; A < 3; A++) {
    digitalWrite(LEDS[A], (boolean)(address & 0x01));
    address >>= 1;

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

