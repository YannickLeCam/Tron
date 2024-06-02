#ifndef __COMMON_H__
#define __COMMON_H__


#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>


#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define TRAIL_UP 4

#define SERV_PORT 5555
#define XMAX 100
#define YMAX 50
#define TIE -2
#define TRAIL_INDEX_SHIFT 50

#define SA struct sockaddr
#define SAI struct sockaddr_in
#define TV struct timeval

#define WALL 111

#define max(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })


#define CHECK(x)                                            \
    do                                                      \
    {                                                       \
        if (!(x))                                           \
        {                                                   \
            fprintf(stderr, "%s:%d: ", __func__, __LINE__); \
            perror(#x);                                     \
            exit(EXIT_FAILURE);                             \
        }                                                   \
    } while (0)

#define CHK(x)                                            \
    do                                                      \
    {                                                       \
        if ((x)==-1)                                           \
        {                                                   \
            fprintf(stderr, "%s:%d: ", __func__, __LINE__); \
            perror(#x);                                     \
            exit(EXIT_FAILURE);                             \
        }                                                   \
    } while (0)

typedef struct display_info
{
    char board[YMAX][XMAX];
    int winner;
} display_info;

struct client_input {
    int id;
    char input;
};

struct client_init_infos {
    int nb_players;
};
/* 
===============================================
                    AJOUT
===============================================
*/
struct client_desc {
    int desc_client;
    int nb_joueur;
    int id_joueur1; //la place dans le tableau des joueurs du joueur.
    int id_joueur2; //si un seul joueur alors pas d'init
};

struct joueur {
    int x;//position du joueur en x
    int y;//position du joueur en y
    int direction;//input du client 
    int trail;//true par defaut
    int alive;//c'est pour les parties avec plus de 2 joueurs 1 en vie 0 mort
};


struct display_info init_plateau();

void update_plateau(struct display_info *jeu, struct joueur *j, int numero);
int demi_tour(int old_pos, int new_pos);


#endif
