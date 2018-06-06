

const int aaPin = 3;
const int abPin = 5;
const int ampereSensorPin = A3;
const float sensitivity = 11.5; // 11.5 analog steps per mA, for 30 Amp version

bool forward = true;
int speed = 256;

void setup() {
  pinMode(aaPin, OUTPUT);
  pinMode(abPin, OUTPUT);
  Serial.begin(115200);
  
}

void loop() {
  long m = millis();
  double md = double(m);
  double s = sin(md / 5000);


  digitalWrite(aaPin, forward ? 0 : 1);  
  analogWrite(abPin, forward ? speed : 256 - speed);
}

float sampleAmpsFor(int milliseconds, int ampereSensorPin) {
  uint32_t start_time = millis();
  float total = 0;
  int count = 0;
  while ((millis() - start_time) < milliseconds) {
    total += readAmps(ampereSensorPin);
    count++;
  }
  return total / count;
}

float readAmps(int pin) {
 int value = analogRead(ampereSensorPin);
 return abs(float(value - midPoint) / sensitivity);  
}

