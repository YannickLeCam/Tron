#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <ncurses.h>
#include <termios.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include "common.h"

#define NB_COLORS 5
#define TRAIL_INDEX_SHIFT 50

#define BLUE_ON_BLACK 0
#define RED_ON_BLACK 2
#define YELLOW_ON_BLACK 1
#define MAGENTA_ON_BLACK 3
#define CYAN_ON_BLACK 4

#define BLUE_ON_BLUE 50
#define RED_ON_RED 52
#define YELLOW_ON_YELLOW 51
#define MAGENTA_ON_MAGENTA 53
#define CYAN_ON_CYAN 54

//Fonctions fournies dans le sujet permettant d'utiliser la bibliothéque ncurses
void tune_terminal();
void init_graphics();
void display_character(int color, int y, int x, char character);

//Fonctions qu'on a mis en place
//Cette fonction permet de faire une récuperation de touche non bloquante
int detect_input(void);                            
//Cette fonction permet de faire l'affichage du board[x][y]
void affichage(struct display_info jeu);
//Cette fonction permet de transformer la touche  récupérée en direction ou en ordre entre 0 et 5
int key_to_int(int key, struct client_input *cli); 
#endif