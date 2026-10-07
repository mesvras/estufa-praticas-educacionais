# Controle da fita de LED pelo LDR (ESP32)

## Ligações

| Fio do LDR | Função    | Pino do ESP32 |
|------------|-----------|---------------|
| Preto      | GND       | GND           |
| Vermelho   | VCC       | 3V3           |
| Azul       | AO (analógico) | GPIO 34  |

O fio verde (DO, digital) não é usado. A fita de LED recebe o sinal PWM no **GPIO 25**.

## Funcionamento

Quanto mais escuro o ambiente, mais forte a fita. Não há Wi-Fi nem controle por horário:
a fita responde ao LDR o tempo todo.

## Por que estes pinos

* **GPIO 34**: entrada do ADC1. O ADC2 (GPIO 0, 2, 4, 12 a 15, 25 a 27) para de funcionar
  com o Wi-Fi ligado. Este sketch não usa Wi-Fi, mas o pino continua compatível com a
  versão que usa.
* **GPIO 25**: saída PWM.
* Evite GPIO 34 a 39 para saídas: são somente entrada e não têm resistor interno.

## Conferir a ordem dos fios

Abra o Monitor Serial em 115200 baud e tape o sensor com a mão:

* A coluna `bruto` deve mudar bastante. Se ficar parada, verde e azul estão trocados.
* A coluna `pwm` deve subir quando o sensor fica tapado.

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
