#include "clavier.h"
#include "MatrixKeyboard/stm32g4_matrix_keyboard.h"
#include "stm32g4_systick.h"
#include <stdio.h>

static volatile uint32_t t = 0;

const char keyboard[16]  =  {
										'1','2','3','A',
										'4','5','6','B',
										'7','8','9','C',
										'*','0','#','D' };

static char getPressedChar(void) {
	char press_key_event, release_key_event;
	uint32_t all_touch_pressed;
	BSP_MATRIX_KEYBOARD_press_and_release_events(&press_key_event, &release_key_event, &all_touch_pressed);
	switch(press_key_event)
	{
		case NO_KEY:
			return("");
			break;
		case MANY_KEYS:
			return("");
			break;
		default:
			return(press_key_event);
			break;
	}
}

void counter(void)
{
	if(t)		//Si le chronomètre est "chargé", on décompte... (comme un minuteur de cuisine !)
		t--;
}

void initKeyboard(void) {
	BSP_systick_add_callback_function(counter);
	BSP_MATRIX_KEYBOARD_init(keyboard);
	printf("Clavier initialisé");
}


char keyboardMain(void) {
	char caractere;
	if(!t) {
		t = 10;
		caractere = getPressedChar();
		return(caractere);
	}
}
