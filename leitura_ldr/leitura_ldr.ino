#include <Arduino.h>

const int PINO_LED = 25;
const int PINO_SENSOR = 34;

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(PINO_LED, OUTPUT);
}

void loop() {
  int leituraSensor = analogRead(PINO_SENSOR);

  Serial.print("Valor do Sensor no D34: ");
  Serial.println(leituraSensor);

  int brilho = map(leituraSensor, 0, 4095, 0, 255);

  analogWrite(PINO_LED, brilho);

  delay(100);
}
