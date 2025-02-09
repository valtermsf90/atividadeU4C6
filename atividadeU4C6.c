#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "funcoes.c"

// Definições de pinos e constantes
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define CONTADOR_LED 25
#define PINO_MATRIZ_LED 7
#define LED_VERDE 11
#define LED_AZUL 12
#define BOTAO_A 5
#define BOTAO_B 6

// Protótipos de funções
void inicializacao_dos_pinos();
void inicializacao_maquina_pio();

int64_t bo(alarm_id_t id, void *user_data)
{
  return 0;
}

// Variáveis globais
static void interrupcao(uint gpio, uint32_t events);
ssd1306_t ssd;
bool cor = true;
static volatile uint32_t last_time = 0;
bool led_azul_ON = false;
bool led_verde_ON = false;
char new_caractere;

int main()
{
  // Inicializações
  stdio_init_all();
  inicializacao_maquina_pio(PINO_MATRIZ_LED);
  inicializacao_dos_pinos();

  limpar_o_buffer();

  // Configuração do I2C
  i2c_init(I2C_PORT, 400 * 1000);
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);

  // Inicialização do display OLED
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
  ssd1306_config(&ssd);
  ssd1306_send_data(&ssd);

  // Limpa o display
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);

  // Configura interrupções para os botões
  gpio_set_irq_enabled_with_callback(BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &interrupcao);
  gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &interrupcao);

  // Loop principal
  limpar_o_buffer();
  escrever_no_buffer();
  while (true)
  {
    sleep_ms(1000);
    printf("DIGITE UM CARACTERE:\n");
    if (scanf("%c", &new_caractere) == 1)
    {
      printf("O CARACTER DIGITADO FOI:\n%c\n", new_caractere);
      ler_caractere(new_caractere);
      if (new_caractere >= 48 && new_caractere <= 57) // Verifica se é um número
      {
        desenho(new_caractere);
        escrever_no_buffer();
      }
      else
      {
        limpar_o_buffer();
        escrever_no_buffer();
      }
    }
    sleep_ms(100);
  }
  return 0;
}

// Função de interrupção para os botões
void interrupcao(uint gpio, uint32_t events)
{
  uint32_t current_time = to_us_since_boot(get_absolute_time());
  if (current_time - last_time > 500000) // Debouncing de 500ms
  {
    last_time = current_time;

    if (gpio == BOTAO_A) // Botão A pressionado
    {
      led_verde_ON = !led_verde_ON;
      gpio_put(LED_VERDE, led_verde_ON);
      cor = !cor;
      ssd1306_fill(&ssd, !cor);
      ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor);
      ssd1306_draw_string(&ssd, "    Botao A", 8, 10);
      ssd1306_draw_string(&ssd, "Pressionado", 20, 30);
      ssd1306_draw_string(&ssd, led_verde_ON ? "LED Verde ON" : "LED Verde OFF", 15, 48);
      ssd1306_send_data(&ssd);
      printf("Botão A pressionado e Led Verde %s\n", led_verde_ON ? "ligado" : "desligado");
    }
    if (gpio == BOTAO_B) // Botão B pressionado
    {
      led_azul_ON = !led_azul_ON;
      gpio_put(LED_AZUL, led_azul_ON);
      cor = !cor;
      ssd1306_fill(&ssd, !cor);
      ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor);
      ssd1306_draw_string(&ssd, "    Botao B", 8, 10);
      ssd1306_draw_string(&ssd, "Pressionado", 20, 30);
      ssd1306_draw_string(&ssd, led_azul_ON ? "LED Azul ON" : "LED Azul OFF", 11, 48);
      ssd1306_send_data(&ssd);
      printf("Botão B pressionado e Led Azul %s\n", led_azul_ON ? "ligado" : "desligado");
    }
  }
}