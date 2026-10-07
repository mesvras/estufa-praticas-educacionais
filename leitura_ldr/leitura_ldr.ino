#include <Arduino.h>

const int PINO_LED = 25;
const int PINO_SENSOR = 34;

const int AMOSTRAS = 16;
const int FAIXA_MINIMA = 100;

int menorLeitura = 4095;
int maiorLeitura = 0;

int lerMedia() {
  long soma = 0;
  for (int i = 0; i < AMOSTRAS; i++) {
    soma += analogRead(PINO_SENSOR);
    delay(2);
  }
  return soma / AMOSTRAS;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(PINO_LED, OUTPUT);
  analogWrite(PINO_LED, 0);

  Serial.println("Tape o sensor e depois ilumine para calibrar");
  Serial.println("leitura\tmin\tmax\tbrilho");
}

void loop() {
  int leitura = lerMedia();
  if (leitura < menorLeitura) menorLeitura = leitura;
  if (leitura > maiorLeitura) maiorLeitura = leitura;

  int brilho = 0;
  if (maiorLeitura - menorLeitura >= FAIXA_MINIMA) {
    brilho = map(leitura, menorLeitura, maiorLeitura, 0, 255);
    brilho = constrain(brilho, 0, 255);
  }

  analogWrite(PINO_LED, brilho);

  Serial.printf("%d\t%d\t%d\t%d\n", leitura, menorLeitura, maiorLeitura, brilho);

  delay(100);
}
