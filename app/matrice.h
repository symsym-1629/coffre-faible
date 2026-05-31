#ifndef MATRICE_H_
#define MATRICE_H_

#include <stdint.h>


typedef struct {
    uint32_t pixels[7][4];
} Glyph_t;


/*
 * affiche une matrice vide
 */
void matrice_init(void);

/*
 * affiche une matrice pour un code donné
 *
 * code[] : le code à afficher
 * size : jusqu'à où afficher le code
 */
void matrice_display(char code[], int nb);

/*
 * affiche une matrice remplie d'une couleur
 *
 * color : la couleur à afficher
 */
void matrice_color(uint32_t color);

#endif /* MATRICE_H_ */
