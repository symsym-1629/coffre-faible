#include "matrice.h"
#include "WS2812/stm32g4_ws2812.h"
#include <string.h>



// couleur des chiffres
#define COLOR_POS_0   0x100000   // cyan
#define COLOR_POS_1   0x081000   // amber
#define COLOR_POS_2   0x100800   // vert clair
#define COLOR_POS_3   0x000A08   // rose


#define _  0   /* pixel éteint */
#define X  1   /* pixel allumé */

static const uint8_t SHAPES[10][7][4] = {
    /* 0 */
    { {_,X,X,_}, {X,_,_,X}, {X,_,_,X}, {X,_,_,X}, {X,_,_,X}, {X,_,_,X}, {_,X,X,_} },
    /* 1 */
    { {_,_,X,_}, {_,X,X,_}, {_,_,X,_}, {_,_,X,_}, {_,_,X,_}, {_,_,X,_}, {_,X,X,X} },
    /* 2 */
    { {_,X,X,_}, {X,_,_,X}, {_,_,_,X}, {_,_,X,_}, {_,X,_,_}, {X,_,_,_}, {X,X,X,X} },
    /* 3 */
    { {X,X,X,_}, {_,_,_,X}, {_,_,_,X}, {_,X,X,_}, {_,_,_,X}, {_,_,_,X}, {X,X,X,_} },
    /* 4 */
    { {_,_,X,X}, {_,X,_,X}, {X,_,_,X}, {X,X,X,X}, {_,_,_,X}, {_,_,_,X}, {_,_,_,X} },
    /* 5 */
    { {X,X,X,X}, {X,_,_,_}, {X,_,_,_}, {X,X,X,_}, {_,_,_,X}, {_,_,_,X}, {X,X,X,_} },
    /* 6 */
    { {_,X,X,_}, {X,_,_,_}, {X,_,_,_}, {X,X,X,_}, {X,_,_,X}, {X,_,_,X}, {_,X,X,_} },
    /* 7 */
    { {X,X,X,X}, {_,_,_,X}, {_,_,X,_}, {_,_,X,_}, {_,X,_,_}, {_,X,_,_}, {_,X,_,_} },
    /* 8 */
    { {_,X,X,_}, {X,_,_,X}, {X,_,_,X}, {_,X,X,_}, {X,_,_,X}, {X,_,_,X}, {_,X,X,_} },
    /* 9 */
    { {_,X,X,_}, {X,_,_,X}, {X,_,_,X}, {_,X,X,X}, {_,_,_,X}, {_,_,_,X}, {_,X,X,_} },
};

#undef _
#undef X

/* Couleurs associées à chaque position */
static const uint32_t POS_COLORS[4] = {
    COLOR_POS_0,
    COLOR_POS_1,
    COLOR_POS_2,
    COLOR_POS_3,
};

// cette variable va servir de passerelle entre le remplissage et la conversion.
static uint32_t matrice_full[2][8][8];


// Fonctions privées


/*
 * insère le chiffre généré par place_chiffre dans la matrice de passage, avec les bonnes couleurs
 *
 * mat : représente sur quelle matrice de led afficher le chiffre
 * pos : représente à quelle colonne de la matrice démarrer le chiffre (son emplacement ds la matrice en gros mais x4)
 */
static void ecrire_chiffre_matrice(int mat, int pos, const Glyph_t *g)
{
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 4; j++) {
            matrice_full[mat][i][pos + j] = g->pixels[i][j];
        }
    }
    // on fout la dernière ligne en noir
    for (int j = 0; j < 4; j++) {
        matrice_full[mat][7][pos + j] = 0x000000;
    }
}

/*
 * crée le chiffre à envoyer à la fonction ecrire_chiffre_matrice()
 *
 * c : le chiffre d'entrée
 * pos : l'emplacement du chiffre dans le code
 */
static void place_chiffre(char c, int pos)
{
    if (c < '0' || c > '9') return;

    int digit  = c - '0'; // check table ascii pour la conversion de str à int
    int mat    = pos / 2;
    int col    = (pos % 2) * 4;
    uint32_t color = POS_COLORS[pos];

    Glyph_t g;
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 4; j++) {
            g.pixels[i][j] = SHAPES[digit][i][j] ? color : 0x000000;
        }
    }

    ecrire_chiffre_matrice(mat, col, &g);
}

/*
 * fait la conversion du tableau 2D à un tableau à envoyer à la matrice led selon l'algo suivant
 * ajoute la ligne si elle est paire, sinon l'ajoute mais en inversée, et ce pour chaque ligne, et chaque matrice
 *
 * pixels : un pointeur vers le tableau à remplir
 */
static void conversion(uint32_t *pixels)
{
    for (int mat = 0; mat < 2; mat++) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
            	pixels[mat * 64 + i * 8 + j] = matrice_full[mat][i][j];
            }
        }
    }
}

// Fonctions privées

/*
 * affiche une matrice vide
 */
void matrice_init(void)
{
    memset(matrice_full, 0, sizeof(matrice_full));
    uint32_t pixels[128];
    conversion(pixels);
    BSP_WS2812_display(pixels, 128);
}

/*
 * affiche une matrice pour un code donné
 *
 * code[] : le code à afficher
 * size : jusqu'à où afficher le code
 */
void matrice_display(char code[], int size)
{
    /* Repart d'un frame vierge à chaque appel */
    memset(matrice_full, 0, sizeof(matrice_full));

    for (int pos = 0; pos < size; pos++) {
        place_chiffre(code[pos], pos);
    }

    uint32_t pixels[128];
    conversion(pixels);
    BSP_WS2812_display(pixels, 128);
}

/*
 * affiche une matrice remplie d'une couleur
 *
 * color : la couleur à afficher
 */
void matrice_color(uint32_t color)
{
    for (int mat = 0; mat < 2; mat++) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                matrice_full[mat][i][j] = color;
            }
        }
    }
    uint32_t pixels[128];
    conversion(pixels);
    BSP_WS2812_display(pixels, 128);
}
