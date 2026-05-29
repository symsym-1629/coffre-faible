/**
 *******************************************************************************
 * @file 	main.c
 * @author 	jjo
 * @date 	Mar 29, 2024
 * @brief	Fichier principal de votre projet sur carte Nucléo STM32G431KB
 *******************************************************************************
 */

#include "config.h"
#include "stm32g4_sys.h"
// #include "MatrixKeyboard/stm32g4_matrix_keyboard.h"
#include "stm32g4_systick.h"
#include "stm32g4_gpio.h"
#include "stm32g4_uart.h"
#include "stm32g4_utils.h"
#include "WS2812/stm32g4_ws2812.h"
#include <stdio.h>

#include "clavier.h"

#define BIP_DELAY		100	//ms
#define GPIO_BUZZER GPIOA
#define PIN_BUZZER GPIO_PIN_5
#define GPIO_RUPTEUR GPIOA
#define PIN_RUPTEUR GPIO_PIN_0
#define GPIO_RUPTEUR GPIOA
#define PIN_GACHE GPIO_PIN_10
#define GPIO_GACHE GPIOA

// matrix def
//#define PIN_INPUT_1 GPIO_PIN_7
//#define PIN_INPUT_2 GPIO_PIN_0
//#define PIN_INPUT_3 GPIO_PIN_12

//#define PIN_OUTPUT_1 GPIO_PIN_11
//#define PIN_OUTPUT_2 GPIO_PIN_8
//#define PIN_OUTPUT_3 GPIO_PIN_6
//#define PIN_OUTPUT_4 GPIO_PIN_15

void write_LED(bool b)
{
	HAL_GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, b);
}

void write_BUZZER(bool b)
{
	HAL_GPIO_WritePin(GPIO_BUZZER, PIN_BUZZER, b);
}

bool read_RUPTEUR(void) {
	return(HAL_GPIO_ReadPin(GPIO_RUPTEUR,PIN_RUPTEUR));
}

bool char_received(uart_id_t uart_id)
{
	if( BSP_UART_data_ready(uart_id) )	/* Si un caractère est reçu sur l'UART 2*/
	{
		/* On "utilise" le caractère pour vider le buffer de réception */
		BSP_UART_get_next_byte(uart_id);
		return true;
	}
	else
		return false;
}

void heartbeat(void)
{
	while(! char_received(UART2_ID) )
	{
		write_LED(true);
		HAL_Delay(50);
		write_LED(false);
		HAL_Delay(1500);
	}
}

const char keyboard_conf[16]  =  {
										'1','2','3','A',
										'4','5','6','B',
										'7','8','9','C',
										'*','0','#','D' };

const char valid_code[4] = {'3','0','1','2'};

/**
  * @brief  Point d'entrée de votre application
  */
int main(void)
{
	/* Cette ligne doit rester la première de votre main !
	 * Elle permet d'initialiser toutes les couches basses des drivers (Hardware Abstraction Layer),
	 * condition préalable indispensable à l'exécution des lignes suivantes.
	 */
	HAL_Init();

	/* Initialisation des périphériques utilisés dans votre programme */
	BSP_GPIO_enable();
	BSP_UART_init(UART2_ID,115200);
	BSP_SYS_set_std_usart(UART2_ID, UART2_ID, UART2_ID);
	BSP_WS2812_init();
	initKeyboard();

	// initialisation des variables
	int valid = 0;
	char number;
	int code_length = 0;
	char code_input[4];
	/* Initialisation des ports utilisés */
	BSP_GPIO_pin_config(LED_GREEN_GPIO, LED_GREEN_PIN, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH,GPIO_NO_AF);
	BSP_GPIO_pin_config(GPIO_BUZZER, PIN_BUZZER, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH,GPIO_NO_AF);
	BSP_GPIO_pin_config(GPIO_RUPTEUR, PIN_RUPTEUR, GPIO_MODE_INPUT,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH,GPIO_NO_AF);
	BSP_GPIO_pin_config(GPIO_GACHE, PIN_GACHE, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH,GPIO_NO_AF);

	// test gâche
	HAL_GPIO_WritePin(GPIO_GACHE, PIN_GACHE, 1);
	//HAL_GPIO_WritePin(GPIO_GACHE, PIN_GACHE, 0);

	// initialisation terminée
	printf("Carte initialisée \n");
	while (1)
	{

		/* write_LED(true); /* write_LED? Faites un ctrl+clic dessus pour voir...
		HAL_Delay(BLINK_DELAY);	/* ... ça fonctionne aussi avec les macros, les variables. C'est votre nouveau meilleur ami
		write_LED(false);
		HAL_Delay(BLINK_DELAY);

		if (read_RUPTEUR()) {
			write_BUZZER(true);
		} else {
			write_BUZZER(false);
		} */

		// boucle de vérification du code
		while(!valid) {
			number = keyboardMain();
			if (number != "") {
				code_input[code_length] = number;
				code_length++;
				matrice_display(code_input);
				if (code_length == 4) {
					if (code_input == valid_code) {
						valid = 1;
					} else {
						for (i=0;i<4;i++) {
							code_input[i] = "x";
						}
						code_length=0;
						matrice_display(code_input);
					}
				}
			}
		}
		// code bon : ouverture du coffre
		HAL_GPIO_WritePin(GPIO_GACHE, PIN_GACHE, 0);
		// bip buzzer de 0.1 sec
		write_BUZZER(true);
		HAL_Delay(BIP_DELAY);
		write_BUZZER(false);
		matrice_color(green);

		while (read_RUPTEUR()) // attente de l'ouverture de la porte
		while (!read_RUPTEUR()) // attente de la fermeture de la porte

		HAL_GPIO_WritePin(GPIO_GACHE, PIN_GACHE, 1); // fermeture du coffre
		matrice_init();
		// keyboardMain() ? printf("%c", keyboardMain()) : 0;
	}
}
