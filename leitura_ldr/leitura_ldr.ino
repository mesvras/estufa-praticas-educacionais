#include <Arduino.h>
#include <WiFi.h>
#include <esp_sntp.h>
#include <time.h>

constexpr char WIFI_SSID[] = "SUA_REDE";
constexpr char WIFI_SENHA[] = "SUA_SENHA";
constexpr uint32_t INTERVALO_RECONEXAO_MS = 10000;

constexpr char FUSO_HORARIO[] = "HST10";
constexpr char SERVIDOR_NTP_1[] = "a.st1.ora.br";
constexpr char SERVIDOR_NTP_2[] = "pool.ntp.org";

constexpr uint8_t HORA_INICIO_MATINAL = 5;
constexpr uint8_t HORA_FIM_MATINAL = 17;

constexpr uint8_t PINO_LDR = 34;
constexpr uint8_t PINO_FITA = 25;

constexpr uint16_t LEITURA_ESCURO = 3300;
constexpr uint16_t LEITURA_CLARO = 300;

constexpr uint8_t AMOSTRAS = 16;
constexpr uint16_t INTERVALO_MS = 500;
constexpr uint16_t ADC_MAXIMO = 4095;
constexpr uint8_t PWM_MAXIMO = 255;

enum class Modo { DESATIVADO, ADAPTATIVO };

uint16_t menorLeitura = ADC_MAXIMO;
uint16_t maiorLeitura = 0;
uint32_t ultimaTentativaWiFi = 0;

void conectarWiFi() {
  Serial.printf("Wi-Fi: conectando a %s...\n", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_SENHA);
  ultimaTentativaWiFi = millis();
}

void manterWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  if (millis() - ultimaTentativaWiFi < INTERVALO_RECONEXAO_MS) return;
  WiFi.disconnect();
  conectarWiFi();
}

void aoEventoWiFi(WiFiEvent_t evento, WiFiEventInfo_t info) {
  switch (evento) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.printf("Wi-Fi: conectado, IP %s\n", WiFi.localIP().toString().c_str());
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.printf("Wi-Fi: desconectado (motivo %u)\n", info.wifi_sta_disconnected.reason);
      break;
    default:
      break;
  }
}

void aoSincronizarRelogio(struct timeval *) {
  struct tm agora;
  getLocalTime(&agora, 0);
  char texto[20];
  strftime(texto, sizeof(texto), "%d/%m/%Y %H:%M:%S", &agora);
  Serial.printf("Relogio: sincronizado, %s (%s)\n", texto, FUSO_HORARIO);
}

bool obterHora(struct tm &agora) {
  return getLocalTime(&agora, 0);
}

Modo escolherModo(bool horaValida, const struct tm &agora) {
  if (!horaValida) return Modo::DESATIVADO;
  bool matinal = agora.tm_hour >= HORA_INICIO_MATINAL && agora.tm_hour < HORA_FIM_MATINAL;
  return matinal ? Modo::ADAPTATIVO : Modo::DESATIVADO;
}

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

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(false);
  WiFi.onEvent(aoEventoWiFi);

  sntp_set_time_sync_notification_cb(aoSincronizarRelogio);
  configTzTime(FUSO_HORARIO, SERVIDOR_NTP_1, SERVIDOR_NTP_2);

  conectarWiFi();

  Serial.println(F("hora\t\tmodo\t\tbruto\tluz(%)\tpwm\tmin\tmax"));
}

void loop() {
  manterWiFi();

  struct tm agora;
  bool horaValida = obterHora(agora);
  Modo modo = escolherModo(horaValida, agora);

  uint16_t bruto = lerMediaAnalogica();
  if (bruto < menorLeitura) menorLeitura = bruto;
  if (bruto > maiorLeitura) maiorLeitura = bruto;

  uint8_t intensidade = modo == Modo::ADAPTATIVO ? calcularIntensidade(bruto) : 0;
  analogWrite(PINO_FITA, intensidade);

  char hora[9] = "--:--:--";
  if (horaValida) strftime(hora, sizeof(hora), "%H:%M:%S", &agora);

  Serial.printf("%s\t%s\t%u\t%u%%\t%u\t%u\t%u\n",
                hora,
                modo == Modo::ADAPTATIVO ? "adaptativo" : "desativado",
                bruto, calcularLuz(bruto), intensidade, menorLeitura, maiorLeitura);

  delay(INTERVALO_MS);
}
