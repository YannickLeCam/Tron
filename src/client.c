#include "client.h"

void tune_terminal()
{
    struct termios term;
    tcgetattr(0, &term);
    term.c_iflag &= ~ICANON;
    term.c_lflag &= ~ICANON;
    term.c_cc[VMIN] = 0;
    term.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &term);
}

void init_graphics()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(100);
    start_color();
    init_pair(BLUE_ON_BLACK, COLOR_BLUE, COLOR_BLACK);
    init_pair(RED_ON_BLACK, COLOR_RED, COLOR_BLACK);
    init_pair(YELLOW_ON_BLACK, COLOR_YELLOW, COLOR_BLACK);
    init_pair(MAGENTA_ON_BLACK, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(CYAN_ON_BLACK, COLOR_CYAN, COLOR_BLACK);
    //===============================================
    init_pair(BLUE_ON_BLUE, COLOR_BLUE, COLOR_BLUE);
    init_pair(RED_ON_RED, COLOR_RED, COLOR_RED);
    init_pair(YELLOW_ON_YELLOW, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(MAGENTA_ON_MAGENTA, COLOR_MAGENTA, COLOR_MAGENTA);
    init_pair(CYAN_ON_CYAN, COLOR_CYAN, COLOR_CYAN);

    init_pair(WALL, COLOR_WHITE, COLOR_WHITE);
}

void display_character(int color, int y, int x, char character)
{
    attron(COLOR_PAIR(color));
    mvaddch(y, x, character);

    attroff(COLOR_PAIR(color));
}

// Retourne VRAI si il y a une entrée de touche et FALSE sinon
int detect_input(void)
{
    int direction, retour;
    nodelay(stdscr, TRUE);
    noecho();
    direction = getch();
    if (direction == ERR)
        retour = FALSE;
    else
    {
        retour = TRUE;
        ungetch(direction);
    }
    echo();
    nodelay(stdscr, FALSE);
    return (retour);
}

/*
Prends en paramétre une touche et un pointeur d'une structure pour le client
Retourne 0 en cas de touche connue détectée et 1 dans le cas de touche à ignorer
*/
int key_to_int(int key, struct client_input *cli)
{
    switch (key)
    {
    case 105: // touche i
        cli->id = 2;
        cli->input = 0;
        return 0;
    case 122: // touche z
        cli->id = 1;
        cli->input = 0;
        return 0;
    case 115: // touche s
        cli->id = 1;
        cli->input = 1;
        return 0;
    case 107: // touche l
        cli->id = 2;
        cli->input = 1;
        return 0;
    case 113: // touche q
        cli->id = 1;
        cli->input = 2;
        return 0;
    case 106: // touche k
        cli->id = 2;
        cli->input = 2;
        return 0;
    case 100: // touche d
        cli->id = 1;
        cli->input = 3;
        return 0;
    case 108: // touche l
        cli->id = 2;
        cli->input = 3;
        return 0;
    case 32: // touche espace
        cli->id = 1;
        cli->input = 4;
        return 0;
    case 109: // touche m
        cli->id = 2;
        cli->input = 4;
        return 0;
    case 120: // touche x
        cli->id = 1;
        cli->input = 5;
        return 0;
    }
    return 1;
}

// Prends en paramétre notre structure contenant le jeu
void affichage(struct display_info jeu)
{
    for (size_t i = 0; i < YMAX; i++)
    {
        for (size_t j = 0; j < XMAX; j++)
        {
            if (jeu.board[i][j] == 111) // Notre mur
                display_character(WALL, i, j, ACS_VLINE);
            else if (jeu.board[i][j] == 100) // Notre espace vide
                display_character(COLOR_BLACK, i, j, ' ');
            else // les joueurs
                display_character(jeu.board[i][j], i, j, 'X');
        }
        mvaddstr(0, XMAX / 2 - strlen("C-TRON") / 2, "C-TRON");
    }
}

int main(int argc, char const *argv[])
{
    //===========================================================================================================
    // PRISE DES ARGUEMENTS EN ENTREE
    //===========================================================================================================
    if (argc < 4 || argc > 4)
    {
        printf("Rappel il y a deux arguments a ce programe \n");
        printf("1:Addresse du serveur \n");
        printf("2:Port du serveur\n");
        printf("3:le nb de joueur present sur le client \n");
        exit(1);
    }
    int adresse = atoi(argv[1]);
    int port = atoi(argv[2]);
    struct client_init_infos *cli_info = malloc(sizeof(struct client_init_infos));
    cli_info->nb_players = atoi(argv[3]);

    //===========================================================================================================
    // INITIALISATION DE LA SOCKET
    //===========================================================================================================
    int masock;
    CHECK(masock = socket(AF_INET, SOCK_STREAM, 0));
    struct sockaddr_in masock_struct;
    // Initialisation de parametres de la socket
    masock_struct.sin_family = AF_INET;
    masock_struct.sin_port = htons(port);
    masock_struct.sin_addr.s_addr = htonl(adresse);
    memset(&(masock_struct.sin_zero), '\0', 8);
    int len = sizeof(struct sockaddr_in); // taille de la sock

    //======================================================
    // CONECTION avec envoie de données
    //======================================================
    int conn;
    CHK(conn = connect(masock, (struct sockaddr *)&masock_struct, len));
    printf("CONNECTED\n");
    CHK(send(masock, cli_info, sizeof(struct client_init_infos), 0));
    //======================================================
    // 1er affichage du jeu
    //======================================================

    //======================================================
    // 1er affichage du jeu
    //======================================================
    struct display_info affi;
    CHK(recv(masock, &affi, sizeof(display_info), 0)); // 1ere reception du jeu
    tune_terminal();
    init_graphics();
    affichage(affi);
    refresh();

    //======================================================
    // commencement du jeu , recup input + affichage
    //======================================================
    struct client_input touche;
    fd_set rfds;
    FD_ZERO(&rfds);
    int descClient = masock;
    int jamais_assez = 0; // valeur disant que le client veut rester sur le jeu
    while (jamais_assez == 0)
    {
        while (affi.winner == 0) // si nous avons un gagnant notre jeu se termine
        {
            FD_SET(masock, &rfds);
            FD_SET(STDIN_FILENO, &rfds);
            masock = descClient;
            select(masock + 1, &rfds, NULL, NULL, NULL);
            // envoie des données
            if (FD_ISSET(STDIN_FILENO, &rfds))
            {
                if (detect_input())
                {
                    int test = getch();
                    if (key_to_int(test, &touche) == 0)
                        CHK(send(masock, &touche, sizeof(struct client_input), 0));
                    if (test == 5) // 5 correspond a envoyer au serveur que ce client deconnect
                        jamais_assez = 1;
                }
                flushinp(); // on vide l'input pour éviter un buffer rempli de directions
            }
            // reception de l'affichage
            if (FD_ISSET(masock, &rfds))
            {
                recv(masock, &affi, sizeof(display_info), 0); // a verifier
                affichage(affi);
                refresh();
            }
        }
        //======================================================
        // AFFICHAGE DE LA FIN DE LA PARTIE
        //======================================================
        char str[30] = "";
        if (affi.winner == -1)
            sprintf(str, "Tous des bouffs");
        if (affi.winner > 0)
            sprintf(str, "Le joueur %d a gagné", affi.winner);
        mvaddstr(YMAX / 2, (XMAX - strlen(str)) / 2, str);
        refresh();
    }
    //======================================================
    // Fin du programme
    //======================================================
    sleep(5); // pour voir le dernier affichage
    CHK(close(masock));
    free(cli_info);
    return 0;
}
