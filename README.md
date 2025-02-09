# Projeto: Controle de Matriz de LED e Display OLED com Raspberry Pi Pico W

## Descrição
Este projeto implementa o controle de uma matriz de LED e um display OLED utilizando um Raspberry Pi Pico W. Ele permite interação através de botões físicos para acionar LEDs e exibir informações no display OLED.

## Componentes Utilizados
- Raspberry Pi Pico W
- Display OLED SSD1306 via I2C
- Matriz de LED
- LEDs individuais (Verde e Azul)
- Botões de entrada

## Bibliotecas Utilizadas
- `pico/stdlib.h`
- `hardware/i2c.h`
- `hardware/timer.h`
- `inc/ssd1306.h`
- `inc/font.h`
- `funcoes.c`

## Pinagem
| Componente      | Pino Raspberry Pi Pico |
|---------------|----------------------|
| I2C SDA (OLED) | GP14 |
| I2C SCL (OLED) | GP15 |
| Matriz de LED  | GP7  |
| LED Verde      | GP11 |
| LED Azul       | GP12 |
| Botão A       | GP5  |
| Botão B       | GP6  |

## Funcionalidades
- Inicializa e configura os periféricos necessários (I2C, GPIO, PIO para matriz de LED).
- Detecta pressionamento de botões para alternar LEDs e exibir mensagens no OLED.
- Permite a entrada de caracteres via terminal e os exibe na matriz de LED.
- Implementa debounce para evitar acionamentos indesejados nos botões.

## Como Compilar e Executar
1. Certifique-se de ter o SDK do Raspberry Pi Pico configurado corretamente.
2. Compile o projeto utilizando `cmake` e `make`:
   ```sh
   mkdir build
   cd build
   cmake ..
   make
   ```
3. Envie o arquivo `.uf2` gerado para o Raspberry Pi Pico W.
4. Conecte-se ao terminal para visualizar as saídas e interagir com o sistema:
   ```sh
   minicom -b 115200 -o -D /dev/ttyUSB0
   ```

## Uso
- Pressione `Botão A` para alternar o LED Verde e exibir mensagens no OLED.
- Pressione `Botão B` para alternar o LED Azul e exibir mensagens no OLED.
- Digite um caractere no terminal para exibi-lo na matriz de LED.

link repositorio: https://github.com/valtermsf90/atividadeU4C6

## Autor
Valter Machado Silva Filho

