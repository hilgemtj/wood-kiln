void setup() {
  digitalWrite(0, LOW);
  digitalWrite(1, LOW);
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  delay(1000); 
  
}

void loop() {
  digitalWrite(0, HIGH);
  delay(2000);
  digitalWrite(1, HIGH);
  delay(1000);
  digitalWrite(0, LOW);
  delay(1000);
  digitalWrite(1, LOW);
  delay(1000);
}
