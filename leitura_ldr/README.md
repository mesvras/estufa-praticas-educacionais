# Controle da fita de LED pelo LDR (ESP32)

## Ligações

| Fio do LDR | Função    | Pino do ESP32 |
|------------|-----------|---------------|
| Preto      | GND       | GND           |
| Vermelho   | VCC       | 3V3           |
| Azul       | AO (analógico) | GPIO 34  |

O fio verde (DO, digital) não é usado. A fita de LED recebe o sinal PWM no **GPIO 25**.

## Wi-Fi e relógio

Preencha `WIFI_SSID` e `WIFI_SENHA` no início do sketch. O ESP tenta conectar a cada
10 segundos enquanto estiver sem rede, e informa no Monitor Serial:

* `Wi-Fi: conectando a <rede>...`
* `Wi-Fi: conectado, IP <ip>`
* `Wi-Fi: desconectado (motivo <código>)`

Após conectar, o relógio é sincronizado por NTP e o sketch imprime
`Relogio: sincronizado, <data e hora>`.

## Modos

* **Adaptativo**, das 5h às 17h: quanto mais escuro o ambiente, mais forte a fita.
* **Desativado**, fora desse horário ou sem hora válida: a fita fica apagada.

O fuso é `HST10` (UTC−10). Assim, das 18h40 às 20h30 no horário de Brasília (UTC−3), o
relógio do ESP marca 11h40 a 13h30, dentro do modo adaptativo. No formato POSIX o sinal
é invertido: `HST10` significa UTC−10.

## Por que estes pinos

* **GPIO 34**: entrada do ADC1. O ADC2 (GPIO 0, 2, 4, 12 a 15, 25 a 27) para de funcionar
  quando o Wi-Fi está ligado, e a estufa vai usar Wi-Fi.
* **GPIO 25**: saída PWM. Ele é do ADC2, mas isso só afeta leitura analógica.
* Evite GPIO 34 a 39 para saídas: são somente entrada e não têm resistor interno.

## Conferir a ordem dos fios

Abra o Monitor Serial em 115200 baud e tape o sensor com a mão:

* A coluna `bruto` deve mudar bastante. Se ficar parada, verde e azul estão trocados.
* No modo adaptativo, a coluna `pwm` deve subir quando o sensor fica tapado.

## Calibração

O sensor é invertido: **quanto mais escuro, maior o valor bruto**. Por isso a escala usa
dois pontos de referência, e não o fundo de escala do ADC.

```cpp
constexpr uint16_t LEITURA_ESCURO = 3300;
constexpr uint16_t LEITURA_CLARO = 300;
```

Para ajustar aos seus valores, use as colunas `min` e `max` do Monitor Serial:

1. Tape o sensor bem tapado por uns 10 segundos. O `max` que aparecer é o seu
   `LEITURA_ESCURO`.
2. Aponte uma lanterna nele por uns 10 segundos. O `min` que aparecer é o seu
   `LEITURA_CLARO`.
3. Copie os dois números para as constantes e recarregue o sketch.

Depois disso, escuro total marca 0% de luz e PWM 255, e luz direta marca 100% e PWM 0. Valores fora da faixa ficam
presos nos extremos pelo `constrain`.

Refaça a calibração se mudar a alimentação de 3V3 para 5V, ou se trocar o módulo: a
divisão de tensão muda junto.
