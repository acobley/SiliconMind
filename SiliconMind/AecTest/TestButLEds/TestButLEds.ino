

int ButLED1=8;
int ButLED2=7;





void setup() {
  // put your setup code here, to run once:
   pinMode(ButLED1,OUTPUT);
   pinMode(ButLED2,OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(ButLED1,true);
  digitalWrite(ButLED2,true);
  delay (500);
  digitalWrite(ButLED1,false);
  delay(500);
   digitalWrite(ButLED1,true);
  digitalWrite(ButLED2,false);
  delay(500);
   digitalWrite(ButLED1,false);
  digitalWrite(ButLED2,false);
  delay(500);
  
}
