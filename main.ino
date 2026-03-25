const int sampleTimeinMs = 200;
const int serialAdress = 9600;

const int waterSensor = 34;
const int warningLed = 32;

// not used yet
const int wlanLed = 33;


void setup() {
  Serial.begin(serialAdress);

  pinMode(waterSensor, INPUT);
  pinMode(warningLed, OUTPUT);
}

void loop() {
  // read sensor
  if (digitalRead(waterSensor) == LOW) {
    Serial.println("Water");
    digitalWrite(warningLed, HIGH);
  } else {
    Serial.println("Dry");
    digitalWrite(warningLed, LOW);
  }

  delay(sampleTimeinMs);
}
