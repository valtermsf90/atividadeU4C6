#include <stdio.h>
#include "pico/stdlib.h"
#include "funcoes.h"
#include "ws2818b.pio.h"

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

// Estrutura para representar um pixel GRB
struct pixel_t
{
	uint8_t G, R, B;
};
typedef struct pixel_t LED_da_matriz;

// Buffer de pixels para a matriz de LEDs
LED_da_matriz leds[CONTADOR_LED];

// Variáveis para controle da máquina PIO
PIO maquina_pio;
uint variavel_maquina_de_estado;
int tamanho_matriz = 5;

// Mapeamento dos LEDs na matriz 5x5
uint matrizint[5][5] = {
		{24, 23, 22, 21, 20},
		{15, 16, 17, 18, 19},
		{14, 13, 12, 11, 10},
		{5, 6, 7, 8, 9},
		{4, 3, 2, 1, 0}};

// Intensidade padrão dos LEDs
uint8_t _intensidade_ = 64;
int num = 0;
ssd1306_t ssd;

// Protótipos de funções
void inicializacao_maquina_pio(uint pino);
void iniciar_pino_gpio();
void atribuir_cor_ao_led(const uint indice, const uint8_t r, const uint8_t g, const uint8_t b, uint8_t intensidade);
void limpar_o_buffer();
void escrever_no_buffer();
void desenho(char num);
void piscar_led();

// Inicializa os pinos GPIO e I2C
void inicializacao_dos_pinos()
{
	gpio_init(LED_AZUL);
	gpio_init(LED_VERDE);
	gpio_set_dir(LED_AZUL, GPIO_OUT);
	gpio_set_dir(LED_VERDE, GPIO_OUT);
	gpio_init(BOTAO_A);
	gpio_init(BOTAO_B);
	gpio_set_dir(BOTAO_A, GPIO_IN);
	gpio_set_dir(BOTAO_B, GPIO_IN);
	gpio_pull_up(BOTAO_A);
	gpio_pull_up(BOTAO_B);

	i2c_init(I2C_PORT, 400 * 1000);
	gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
	gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
	gpio_pull_up(I2C_SDA);
	gpio_pull_up(I2C_SCL);
}

// Matrizes de caracteres para desenhar números na matriz de LEDs
char matriz_0[5][5] = {
		{'*', 'Y', 'Y', 'Y', '*'},
		{'*', 'Y', '*', 'Y', '*'},
		{'*', 'Y', '*', 'Y', '*'},
		{'*', 'Y', '*', 'Y', '*'},
		{'*', 'Y', 'Y', 'Y', '*'}};
char matriz_1[5][5] = {
		{'*', '*', 'W', '*', '*'},
		{'*', 'W', 'W', '*', '*'},
		{'*', '*', 'W', '*', '*'},
		{'*', '*', 'W', '*', '*'},
		{'*', 'W', 'W', 'W', '*'}};
char matriz_2[5][5] = {
		{'*', 'B', 'B', 'B', '*'},
		{'*', '*', '*', 'B', '*'},
		{'*', 'B', 'B', 'B', '*'},
		{'*', 'B', '*', '*', '*'},
		{'*', 'B', 'B', 'B', '*'}};
char matriz_3[5][5] = {
		{'*', 'R', 'R', 'R', '*'},
		{'*', '*', '*', 'R', '*'},
		{'*', '*', 'R', 'R', '*'},
		{'*', '*', '*', 'R', '*'},
		{'*', 'R', 'R', 'R', '*'}};
char matriz_4[5][5] = {
		{'*', 'G', '*', 'G', '*'},
		{'*', 'G', '*', 'G', '*'},
		{'*', 'G', 'G', 'G', '*'},
		{'*', '*', '*', 'G', '*'},
		{'*', '*', '*', 'G', '*'}};
char matriz_5[5][5] = {
		{'*', 'P', 'P', 'P', '*'},
		{'*', 'P', '*', '*', '*'},
		{'*', 'P', 'P', 'P', '*'},
		{'*', '*', '*', 'P', '*'},
		{'*', 'P', 'P', 'P', '*'}};
char matriz_6[5][5] = {
		{'*', 'C', 'C', 'C', '*'},
		{'*', 'C', '*', '*', '*'},
		{'*', 'C', 'C', 'C', '*'},
		{'*', 'C', '*', 'C', '*'},
		{'*', 'C', 'C', 'C', '*'}};
char matriz_7[5][5] = {
		{'*', 'Y', 'Y', 'Y', '*'},
		{'*', '*', '*', 'Y', '*'},
		{'*', '*', '*', 'Y', '*'},
		{'*', '*', '*', 'Y', '*'},
		{'*', '*', '*', 'Y', '*'}};
char matriz_8[5][5] = {
		{'*', 'W', 'W', 'W', '*'},
		{'*', 'W', '*', 'W', '*'},
		{'*', 'W', 'W', 'W', '*'},
		{'*', 'W', '*', 'W', '*'},
		{'*', 'W', 'W', 'W', '*'}};
char matriz_9[5][5] = {
		{'*', 'G', 'G', 'G', '*'},
		{'*', 'G', '*', 'G', '*'},
		{'*', 'G', 'G', 'G', '*'},
		{'*', '*', '*', 'G', '*'},
		{'*', 'G', 'G', 'G', '*'}};

// Inicializa a máquina PIO para controle da matriz de LEDs
void inicializacao_maquina_pio(uint pino)
{
	uint programa_pio, i;
	programa_pio = pio_add_program(pio0, &ws2818b_program);
	maquina_pio = pio0;

	variavel_maquina_de_estado = pio_claim_unused_sm(maquina_pio, false);
	if (variavel_maquina_de_estado < 0)
	{
		maquina_pio = pio1;
		variavel_maquina_de_estado = pio_claim_unused_sm(maquina_pio, true);
	}

	ws2818b_program_init(maquina_pio, variavel_maquina_de_estado, programa_pio, pino, 800000.f);

	// Limpa o buffer de LEDs
	for (i = 0; i < CONTADOR_LED; ++i)
	{
		leds[i].R = 0;
		leds[i].G = 0;
		leds[i].B = 0;
	}
}

// Atribui uma cor RGB a um LED específico
void atribuir_cor_ao_led(const uint indice, const uint8_t r, const uint8_t g, const uint8_t b, uint8_t intensidade)
{
	if (intensidade > 255)
		intensidade = 255;
	if (intensidade < 0)
		intensidade = 0;

	leds[indice].R = (r * intensidade);
	leds[indice].G = (g * intensidade);
	leds[indice].B = (b * intensidade);
}

// Escreve os dados do buffer nos LEDs
void escrever_no_buffer()
{
	for (uint i = 0; i < CONTADOR_LED; ++i)
	{
		pio_sm_put_blocking(maquina_pio, variavel_maquina_de_estado, leds[i].G);
		pio_sm_put_blocking(maquina_pio, variavel_maquina_de_estado, leds[i].R);
		pio_sm_put_blocking(maquina_pio, variavel_maquina_de_estado, leds[i].B);
	}
	sleep_us(100); // Tempo de reset para os LEDs
}

// Limpa o buffer de LEDs
void limpar_o_buffer()
{
	for (uint i = 0; i < CONTADOR_LED; ++i)
	{
		atribuir_cor_ao_led(i, 0, 0, 0, 0);
	}
}

// Desenha um número na matriz de LEDs
void desenho(char num)
{
	char(*matriz)[5];
	switch (num)
	{
	case '0':
		matriz = matriz_0;
		break;
	case '1':
		matriz = matriz_1;
		break;
	case '2':
		matriz = matriz_2;
		break;
	case '3':
		matriz = matriz_3;
		break;
	case '4':
		matriz = matriz_4;
		break;
	case '5':
		matriz = matriz_5;
		break;
	case '6':
		matriz = matriz_6;
		break;
	case '7':
		matriz = matriz_7;
		break;
	case '8':
		matriz = matriz_8;
		break;
	case '9':
		matriz = matriz_9;
		break;
	}

	// Atualiza a matriz de LEDs com base na matriz de caracteres
	for (int x = 0; x < tamanho_matriz; x++)
	{
		for (int y = 0; y < tamanho_matriz; y++)
		{
			switch (matriz[x][y])
			{
			case 'R':
				atribuir_cor_ao_led(matrizint[x][y], 255, 0, 0, _intensidade_);
				break;
			case 'G':
				atribuir_cor_ao_led(matrizint[x][y], 0, 255, 0, _intensidade_);
				break;
			case 'B':
				atribuir_cor_ao_led(matrizint[x][y], 0, 0, 255, _intensidade_);
				break;
			case 'Y':
				atribuir_cor_ao_led(matrizint[x][y], 255, 255, 0, _intensidade_);
				break;
			case 'P':
				atribuir_cor_ao_led(matrizint[x][y], 255, 0, 255, _intensidade_);
				break;
			case 'C':
				atribuir_cor_ao_led(matrizint[x][y], 0, 255, 255, _intensidade_);
				break;
			case 'W':
				atribuir_cor_ao_led(matrizint[x][y], 255, 255, 255, _intensidade_);
				break;
			case '*':
				atribuir_cor_ao_led(matrizint[x][y], 0, 0, 0, _intensidade_);
				break;
			}
		}
		escrever_no_buffer();
		sleep_ms(10); // Delay para atualização suave
	}
}

// Exibe uma mensagem no display OLED com base no caractere recebido
void ler_caractere(char caractere)
{
	char mensagem[2][20] = {"Caractere ", "Numero "};
	uint registro_de_tipo;
	if (caractere >= 'A' && caractere <= 'Z' || caractere >= 'a' && caractere <= 'z')
	{
		registro_de_tipo = 0;
		mensagem[0][10] = caractere;
	}
	else if (caractere >= '0' && caractere <= '9')
	{
		registro_de_tipo = 1;
		mensagem[1][7] = caractere;
	}
	ssd1306_fill(&ssd, false);
	ssd1306_rect(&ssd, 3, 3, 122, 58, true, false);
	ssd1306_draw_string(&ssd, mensagem[registro_de_tipo], 8, 10);
	ssd1306_send_data(&ssd);
}