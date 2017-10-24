int PIN = 9;
int state = 0;
int preState = state;

void setup() {
  Serial.begin(9600);
  pinMode(PIN, INPUT_PULLUP);
}

void loop() {
  state = digitalRead(PIN);
  if(state != preState){
    Serial.println(state);
  }
  preState = state;
  delay(50);
}
