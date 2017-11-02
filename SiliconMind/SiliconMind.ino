int Out[8]={4,5,6,7,8,9,10,11};
int In=A0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  for (int i=0;i<8;i++){
   pinMode(Out[i], OUTPUT);
  }
  pinMode(In,INPUT);
    Serial.println("setup");
}

void loop() {
  Serial.println("Loop");
  // put your main code here, to run repeatedly:
  
 
  for (int i=0;i<8;i++){
     boolean State=false;
     digitalWrite(Out[i],true);
     Serial.print(i);
     Serial.print("  ");
     Serial.println(digitalRead(In));
      //digitalWrite(Out[i],false);
  }
  delay(200);
  
}
