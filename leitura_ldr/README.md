# Leitura do LDR (ESP32)

## Ligações

| Fio do LDR | Função    | Pino do ESP32 |
|------------|-----------|---------------|
| Preto      | GND       | GND           |
| Vermelho   | VCC       | 3V3           |
| Verde      | DO (digital) | GPIO 27    |
| Azul       | AO (analógico) | GPIO 34  |

Se o módulo tiver só três fios úteis, use o analógico (azul) e deixe o verde desconectado.

## Por que estes pinos

* **GPIO 34**: entrada do ADC1. O ADC2 (GPIO 0, 2, 4, 12 a 15, 25 a 27) para de funcionar
  quando o Wi-Fi está ligado, e a estufa vai usar Wi-Fi.
* **GPIO 27**: entrada digital livre. Não usa pino de boot (0, 2, 12, 15) nem os pinos
  da flash (6 a 11).
* Evite GPIO 34 a 39 para saídas: são somente entrada e não têm resistor interno.

## Conferir a ordem dos fios

Abra o Monitor Serial em 115200 baud e tape o sensor com a mão:

* A coluna `bruto` deve mudar bastante. Se ficar parada, verde e azul estão trocados.
* A coluna `digital` deve alternar entre 0 e 1 no ponto de ajuste do trimpot.

A polaridade do DO depende do módulo. Na maioria, `0` significa claro e `1` significa
escuro. Gire o trimpot para ajustar o limiar.

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

Depois disso, escuro total marca 0% e luz direta marca 100%. Valores fora da faixa ficam
presos nos extremos pelo `constrain`.

Refaça a calibração se mudar a alimentação de 3V3 para 5V, ou se trocar o módulo: a
divisão de tensão muda junto.
