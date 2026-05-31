#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "MatrixKeyboard/stm32g4_matrix_keyboard.h"

// appelle l'initialisation du clavier matriciel et attache la fonction de callback
void initKeyboard(void);

// récupère la touche appuyée
char keyboardMain(void);

#endif /* KEYBOARD_H_ */
