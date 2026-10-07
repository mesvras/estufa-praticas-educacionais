#include <Arduino.h>

constexpr uint8_t PINO_LDR = 34;
constexpr uint8_t PINO_FITA = 25;

constexpr uint16_t LEITURA_ESCURO = 3300;
constexpr uint16_t LEITURA_CLARO = 300;

constexpr uint8_t AMOSTRAS = 16;
constexpr uint16_t INTERVALO_MS = 500;
constexpr uint16_t ADC_MAXIMO = 4095;
constexpr uint8_t PWM_MAXIMO = 255;

uint16_t menorLeitura = ADC_MAXIMO;
uint16_t maiorLeitura = 0;

uint16_t lerMediaAnalogica() {
  uint32_t soma = 0;
  for (uint8_t i = 0; i < AMOSTRAS; i++) {
    soma += analogRead(PINO_LDR);
    delay(2);
  }
  return soma / AMOSTRAS;
}

uint8_t calcularLuz(uint16_t bruto) {
  uint16_t limitado = constrain(bruto, LEITURA_CLARO, LEITURA_ESCURO);
  return map(limitado, LEITURA_ESCURO, LEITURA_CLARO, 0, 100);
}

uint8_t calcularIntensidade(uint16_t bruto) {
  uint16_t limitado = constrain(bruto, LEITURA_CLARO, LEITURA_ESCURO);
  return map(limitado, LEITURA_CLARO, LEITURA_ESCURO, 0, PWM_MAXIMO);
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  analogWrite(PINO_FITA, 0);

  Serial.println();
  Serial.println(F("Estufa: controle da fita pelo LDR"));
  Serial.println(F("bruto\tluz(%)\tpwm\tmin\tmax"));
}

void loop() {
  uint16_t bruto = lerMediaAnalogica();
  if (bruto < menorLeitura) menorLeitura = bruto;
  if (bruto > maiorLeitura) maiorLeitura = bruto;

  uint8_t intensidade = calcularIntensidade(bruto);
  analogWrite(PINO_FITA, intensidade);

  Serial.printf("%u\t%u%%\t%u\t%u\t%u\n",
                bruto, calcularLuz(bruto), intensidade, menorLeitura, maiorLeitura);

  delay(INTERVALO_MS);
}
