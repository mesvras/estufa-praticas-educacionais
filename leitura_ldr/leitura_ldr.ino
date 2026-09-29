#include <Arduino.h>

constexpr uint8_t PINO_LDR_ANALOGICO = 34;
constexpr uint8_t PINO_LDR_DIGITAL = 27;

constexpr uint8_t PINO_LED_VERDE = 16;
constexpr uint8_t PINO_LED_VERMELHO = 17;
constexpr uint8_t PINO_LED_AZUL = 18;
constexpr uint8_t PINO_FITA = 4;

constexpr uint16_t LEITURA_ESCURO = 3300;
constexpr uint16_t LEITURA_CLARO = 300;
constexpr uint16_t LIMIAR = (LEITURA_ESCURO+LEITURA_CLARO)/2;

constexpr uint8_t AMOSTRAS = 16;
constexpr uint16_t INTERVALO_MS = 500;
constexpr uint16_t ADC_MAXIMO = 4095;
constexpr float TENSAO_REFERENCIA = 3.3f;

uint16_t menorLeitura = ADC_MAXIMO;
uint16_t maiorLeitura = 0;

uint16_t lerMediaAnalogica() {
  uint32_t soma = 0;
  for (uint8_t i = 0; i < AMOSTRAS; i++) {
    soma += analogRead(PINO_LDR_ANALOGICO);
    delay(2);
  }
  return soma / AMOSTRAS;
}

uint8_t calcularLuz(uint16_t bruto) {
  uint16_t limitado = constrain(bruto, LEITURA_CLARO, LEITURA_ESCURO);
  return map(limitado, LEITURA_ESCURO, LEITURA_CLARO, 0, 100);
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  pinMode(PINO_LDR_DIGITAL, INPUT);

  pinMode(PINO_LED_VERMELHO, OUTPUT);
  pinMode(PINO_LED_VERDE, OUTPUT);
  pinMode(PINO_LED_AZUL, OUTPUT);
  pinMode(PINO_FITA, OUTPUT);

  Serial.println();
  Serial.println(F("Leitura do LDR"));
  Serial.println(F("bruto\ttensao\tluz(%)\tdigital\tmin\tmax"));
}

void loop() {
  uint16_t bruto = lerMediaAnalogica();

  if (bruto < menorLeitura) menorLeitura = bruto;
  if (bruto > maiorLeitura) maiorLeitura = bruto;

  float tensao = (bruto * TENSAO_REFERENCIA) / ADC_MAXIMO;
  uint8_t luz = calcularLuz(bruto);
  int digital = digitalRead(PINO_LDR_DIGITAL);

  uint8_t SAIDA = bruto >= LIMIAR ? HIGH : LOW;
  digitalWrite(PINO_LED_VERDE, SAIDA);
  digitalWrite(PINO_LED_VERMELHO, SAIDA);
  digitalWrite(PINO_LED_AZUL, SAIDA);

  digitalWrite(PINO_FITA, HIGH);

  Serial.printf("%u\t%.2f V\t%u%%\t%d\t%u\t%u\n",
                bruto, tensao, luz, digital, menorLeitura, maiorLeitura);

  delay(INTERVALO_MS);
}
