void setup() {
  // put your setup code here, to run once:
  pinMode(A1,INPUT);
  Serial.begin(9600);
}

void loop() {
  int val = analogRead(A1);
  Serial.println(val);
  // put your main code here, to run repeatedly:

}
