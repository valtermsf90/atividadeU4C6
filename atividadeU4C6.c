#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "funcoes.c"


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

//PROTOTIPOS
void  inicializacao_dos_pinos();
void inicializacao_maquina_pio();

int64_t bo(alarm_id_t id, void *user_data)
{
    // Put your timeout handler code in here
    return 0;
}

//-----FUNÇÕES COMPLEMENTARES-----
static void interrupcao(uint gpio, uint32_t events);
ssd1306_t ssd;   
bool cor = true;
static volatile uint32_t last_time = 0;
bool led_azul_ON = false;
bool led_verde_ON = false;
char new_caractere;
int main()
{
  
    stdio_init_all();
    inicializacao_maquina_pio(PINO_MATRIZ_LED);
    inicializacao_dos_pinos();

    limpar_o_buffer();
  
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
                                                 // Inicializa a estrutura do display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); 
    ssd1306_send_data(&ssd); // Envia os dados para o display

  // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);                                        // Configura o display
    
    // Configura o botão 0 para interromper a execução e chamar a função gpio_irq_handler quando o botão 0 for pressionado
    gpio_set_irq_enabled_with_callback(BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &interrupcao);
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &interrupcao);
  
    //add_alarm_in_ms(2000, alarm_callback, NULL, false);
    limpar_o_buffer();
    escrever_no_buffer();
    while (true)
    {
    sleep_ms(2000);
    printf("DIGITE UM CARACTERE:\n");
    if(scanf("%c", &new_caractere) == 1){
      printf("O CARACTER DIGITADO FOI:\n%c.", new_caractere);
      interpretacao_do_caractere(new_caractere);
            if(new_caractere >= 48 && new_caractere <= 57){
                desenho(new_caractere);
                escrever_no_buffer();
            }else{
                limpar_o_buffer();
                escrever_no_buffer();
            }
        }
        sleep_ms(100);

    
    }
    return 0;
}

void interrupcao(uint gpio, uint32_t events)
{

	// Obtém o tempo atual em microssegundos
	uint32_t current_time = to_us_since_boot(get_absolute_time());
	// Verifica se passou tempo suficiente desde o último evento
	if (current_time - last_time > 500000) // 500 ms de debouncing
	{
		last_time = current_time; // Atualiza o tempo do último evento
		// Verifica qual botão foi pressionado	

		//
		if (gpio == BOTAO_A)
		{
      if( led_verde_ON == false){
        led_verde_ON = true;      
      gpio_put(LED_VERDE, led_verde_ON);
      cor = !cor;
      // Atualiza o conteúdo do display com animações
      ssd1306_fill(&ssd, !cor); // Limpa o display
      ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo
      ssd1306_draw_string(&ssd, "    Botao A", 8, 10); // Desenha uma string
      ssd1306_draw_string(&ssd, "Pressionado", 20, 30); // Desenha uma string
      ssd1306_draw_string(&ssd, "LED Verde ON", 15, 48); // Desenha uma string      
      ssd1306_send_data(&ssd); // Atualiza o display
      printf("Botão A pressionado e Led Verde ligado\n");
    
    }else{
      led_verde_ON = false;
      gpio_put(LED_VERDE, led_verde_ON);
      cor = !cor;
      // Atualiza o conteúdo do display com animações
      ssd1306_fill(&ssd, !cor); // Limpa o display
      ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo
      ssd1306_draw_string(&ssd, "    Botao A", 8, 10); // Desenha uma string
      ssd1306_draw_string(&ssd, "Pressionado", 20, 30); // Desenha uma string
      ssd1306_draw_string(&ssd, "LED Verde OFF", 15, 48); // Desenha uma string      
      ssd1306_send_data(&ssd); // Atualiza o display
      printf("Botão A pressionado e Led Verde desligado\n");

    }
  }
		if (gpio == BOTAO_B)
		{
      if( led_azul_ON == false){
        led_azul_ON = true;
        gpio_put(LED_AZUL, led_azul_ON);;
        cor = !cor;
        // Atualiza o conteúdo do display com animações
        ssd1306_fill(&ssd, !cor); // Limpa o display
        ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo
        ssd1306_draw_string(&ssd, "    Botao B", 8, 10); // Desenha uma string
        ssd1306_draw_string(&ssd, "Pressionado", 20, 30); // Desenha uma string
        ssd1306_draw_string(&ssd, "LED Azul ON", 15, 48); // Desenha uma string      
        ssd1306_send_data(&ssd); // Atualiza o display
        printf("Botão B pressionado e Led Verde ligado\n");
        }else{
          led_azul_ON = false;
          gpio_put(LED_AZUL, led_azul_ON);
        cor = !cor;
        // Atualiza o conteúdo do display com animações
        ssd1306_fill(&ssd, !cor); // Limpa o display
        ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo
        ssd1306_draw_string(&ssd, "    Botao B", 8, 10); // Desenha uma string
        ssd1306_draw_string(&ssd, "Pressionado", 20, 30); // Desenha uma string
        ssd1306_draw_string(&ssd, "LED Azul OFF", 11, 48); // Desenha uma string      
        ssd1306_send_data(&ssd); // Atualiza o display
        printf("Botão B pressionado e Led Verde desligado\n");
        }
    }
	}
}
void ler_caractere(char caractere){
  char mensagem[2][20] = {"Caractere ", "Numero "};
  uint registro_de_tipo;
  if(caractere >= 'A' && caractere <= 'Z' || caractere >= 'a' && caractere <= 'z'){
      registro_de_tipo = 0;
      mensagem[0][10] = caractere;
  }else if(caractere >= '0' && caractere <= '9'){
      registro_de_tipo = 1;
      mensagem[1][7] = caractere;
  }
  ssd1306_fill(&ssd, false);
  ssd1306_rect(&ssd, 3, 3, 122, 58, true, false);
  ssd1306_draw_string(&ssd, mensagem[registro_de_tipo], 8, 10);
  ssd1306_send_data(&ssd);
}
void interpretacao_do_caractere(char caractere){
  char mensagem[2][20] = {"Caractere ", "Numero "};
  uint registro_de_tipo;
  if(caractere >= 'A' && caractere <= 'Z' || caractere >= 'a' && caractere <= 'z'){
      registro_de_tipo = 0;
      mensagem[0][10] = caractere;
  }else if(caractere >= '0' && caractere <= '9'){
      registro_de_tipo = 1;
      mensagem[1][7] = caractere;
  }
  ssd1306_fill(&ssd, false);
  ssd1306_rect(&ssd, 3, 3, 122, 58, true, false);
  ssd1306_draw_string(&ssd, mensagem[registro_de_tipo], 8, 10);
  ssd1306_send_data(&ssd);
}
