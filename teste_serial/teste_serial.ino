#include <Arduino.h>

constexpr uint8_t PINO_LED = 2;

uint32_t contador = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);
  pinMode(PINO_LED, OUTPUT);
}

void loop() {
  contador++;

  Serial.print("teste ");
  Serial.println(contador);

  digitalWrite(PINO_LED, contador % 2);
  delay(1000);
}
