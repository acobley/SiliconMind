const byte GateInInterrupt = 2;
byte OutputLed=7;
boolean LedState=false;
void setup() {
  // put your setup code here, to run once:
    attachInterrupt(digitalPinToInterrupt(GateInInterrupt), HandleClock, RISING);

  pinMode(OutputLed, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

}

void HandleClock() {
  LedState = !LedState;
  digitalWrite(OutputLed, LedState);
}


