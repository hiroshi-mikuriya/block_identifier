const int PIN = 9;
const int LIMIT = 100;
const int DELAY = 1;
int count = 0;
int state = 0;
int preState = state;

void setup() {
  Serial.begin(9600);
  pinMode(PIN, INPUT_PULLUP);
}

void loop() {
  int d = digitalRead(PIN);
  if(d == 0 && count < LIMIT){
    ++count;
  }else if(d == 1 && -LIMIT < count){
    --count;
  }else{
    state = d;
  }
  if(state != preState){
    Serial.print(state);
  }
  preState = state;
  delay(DELAY);
}
