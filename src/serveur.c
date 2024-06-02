#include "common.h"
#define MAX_JOUEUR 5
#define max_client 6

//Fonction initialisant le plateau de jeu
struct display_info init_plateau()
{
    struct display_info plt;
    plt.winner = 0;
    for (size_t i = 0; i < YMAX; i++)
    {
        for (size_t j = 0; j < XMAX; j++)
        {
            if (i == 0 || i == YMAX - 1 || j == 0 || j == XMAX - 1)
                plt.board[i][j] = 111;
            else
                plt.board[i][j] = 100;
        }
    }
    return plt;
}
//Mets a jour le jeu en fonction du joueur
void update_plateau(struct display_info *jeu, struct joueur *j, int numero)
{
    if (j->trail == 1)
        jeu->board[j->y][j->x] = numero + 50;
    else
        jeu->board[j->y][j->x] = 100;
    switch (j->direction)
    {
    case 0:
        j->y -= 1;
        break;
    case 1:
        j->y += 1;
        break;
    case 2:
        j->x -= 1;
        break;
    case 3:
        j->x += 1;
        break;
    }
    if (jeu->board[j->y][j->x] == 100)
        jeu->board[j->y][j->x] = numero;
    else
        j->alive = 0;
}

//Fonction empechant de faire demi tour
int dir_checker(int old_pos, int new_pos)
{
    if (old_pos == new_pos)
        return 0;
    if ((old_pos == 0 && new_pos == 1) || (old_pos == 1 && new_pos == 0) || (old_pos == 2 && new_pos == 3) || (old_pos == 3 && new_pos == 2))
        return 0;
    return 1;
}

void reattribution_place_tab_desc(struct client_desc* tab, int size_max_tab)
{
    int place_libre = -1;
    for (int i = 0; i < size_max_tab; i++)
    {
        if (tab[i].desc_client == -1 && place_libre == -1)
            place_libre = i;
        if (place_libre != -1 && tab[i].desc_client != -1)
        {
            tab[place_libre] = tab[i];
            place_libre++;
        }
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
        printf("3:framerate (en ms)\n");
        exit(1);
    }
    int adresse = atoi(argv[1]);
    int port = atoi(argv[2]);
    int framerate = atoi(argv[3]);

    //===========================================================================================================
    // INITIALISATION DE LA SOCKET
    //===========================================================================================================
    int descServ, descNvCli, max_desc;                  // desc du serveur
    struct sockaddr_in addrServ;                        // addr serveur
    struct sockaddr_in addrClient;                      // addr du client (temporair pour faire le FD_SET)
    fd_set read_fd_set, test_fd_set;                    // pour eviter les changement sur le select par effet de bord
    struct client_desc tab_desc_client[max_client + 1]; //+1 pour le serveur
    CHECK((descServ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)));
    printf("[+] S - SOCKET OK \n");
    // Param de la socket
    addrServ.sin_family = AF_INET;
    addrServ.sin_addr.s_addr = htonl(adresse); // toutes les adresses possibles du serveur !
    addrServ.sin_port = htons(port);
    CHK(bind(descServ, (struct sockaddr *)&addrServ, (sizeof(struct sockaddr_in))));
    printf("[+] S - BIND INIT \n");
    max_desc = descServ + max_client; // On peut avoir 5 clients en plus
    CHK(listen(descServ, max_desc));

    //===========================================================================================================
    // Ajout des desc aux clients
    //===========================================================================================================
    // initialisation du tableau desc des clients connecté
    for (int i = 0; i < max_client + 1; i++)
    {
        tab_desc_client[i].desc_client = -1;
    }
    tab_desc_client[0].desc_client = descServ; // PENSER A RENOMMER EN TAB DESC
    int nb_desc_actuel = descServ + 1;         // le nombre de desc qu'il y a
    int nb_client_actuel = 1;                  // 1 car le serveur est deja dans le tableau

    //===========================================================================================================
    // Gestion de la connection des clients
    //===========================================================================================================
    struct timeval tv;
    struct client_init_infos cli_inf;
    socklen_t len;
    int nb_joueur_actuel = 0;
    int placement_dans_tab; // pour sauvegarder le placement du nouveau client dans le tableau des desc des clients accepté var temporaire
    int lock = 0;
    FD_ZERO(&read_fd_set);
    FD_SET(descServ, &read_fd_set);
    int nb_game_lance = 0; // BRICOLAGE
    nb_client_actuel++;    // BRICOLAGE
    // boucle infini mais soon va pouvoir etre stoper par un message quit dans le terminal du serveur
    while (nb_client_actuel != 1)
    {
        /* BRICOLAGE POUR PEUT-ETRE TRANSFORMER LE TOUT EN DO - WHILE */
        if (nb_game_lance == 0)
        {
            nb_client_actuel--;
        }
        nb_game_lance++;
        if (nb_joueur_actuel > 2)
            lock = 1;
        while (lock != 1)
        {
            test_fd_set = read_fd_set;
            select(nb_desc_actuel + 1, &test_fd_set, NULL, NULL, NULL);
            for (int desc = 0; desc < nb_desc_actuel + 1; desc++)
            {
                if (FD_ISSET(desc, &test_fd_set))
                {
                    if (desc == descServ)
                    {
                        printf("J'ai une tentative de connection ! \n");
                        // sauvegarde du descripteur et acceptation du client si possible
                        placement_dans_tab = 0;
                        CHECK(descNvCli = accept(descServ, (struct sockaddr *)&addrClient, &len));
                        FD_SET(descNvCli, &read_fd_set);
                        nb_client_actuel++;
                        while (placement_dans_tab != max_desc)
                        {
                            if (tab_desc_client[placement_dans_tab].desc_client == -1)
                            {
                                tab_desc_client[placement_dans_tab].desc_client = descNvCli;
                                break;
                            }
                            placement_dans_tab++;
                        }
                        nb_desc_actuel++;

                        CHK(recv(descNvCli, &cli_inf, sizeof(struct client_init_infos), 0));
                        nb_joueur_actuel += cli_inf.nb_players;
                        tab_desc_client[placement_dans_tab].nb_joueur = cli_inf.nb_players;
                        if (nb_joueur_actuel >= 2)
                            lock = 1;
                        placement_dans_tab = 0;
                    }
                }
            }
        }
        //===========================================================================================================
        // Initialisation et premier envoi
        //===========================================================================================================
        struct display_info jeu = init_plateau();
        struct joueur tab_joueur[MAX_JOUEUR];
        struct client_input touche;
        // init du tableau de joueurs comme tu l'as demandé
        int placement_joueur = 0;
        for (int i = 1; i < nb_client_actuel; i++)
        {
            for (int j = 0; j < tab_desc_client[i].nb_joueur; j++)
            {
                tab_joueur[placement_joueur].alive = 1;
                tab_joueur[placement_joueur].trail = 1;
                tab_joueur[placement_joueur].y = YMAX / 2;
                tab_joueur[placement_joueur].x = XMAX / 2;
                tab_joueur[placement_joueur].direction = 3;
                switch (placement_joueur)
                {
                case 0:
                    tab_joueur[placement_joueur].x = 1;
                    break;
                case 1:
                    tab_joueur[placement_joueur].x = XMAX - 2;
                    tab_joueur[placement_joueur].direction = 2;
                    break;
                case 2:
                    tab_joueur[placement_joueur].y = 1;
                    tab_joueur[placement_joueur].direction = 1;
                    break;
                case 3:
                    tab_joueur[placement_joueur].y = YMAX - 2;
                    tab_joueur[placement_joueur].direction = 0;
                    break;
                }
                if (tab_desc_client[i].nb_joueur == 2)
                {
                    if (j == 0)
                        tab_desc_client[i].id_joueur1 = placement_joueur;
                    else
                        tab_desc_client[i].id_joueur2 = placement_joueur;
                }
                else
                    tab_desc_client[i].id_joueur1 = placement_joueur;
                placement_joueur++;
            }
        }
        for (int i = 0; i < nb_client_actuel + 1; i++)
        {
            if (tab_desc_client[i].desc_client != descServ)
                send(tab_desc_client[i].desc_client, &jeu, sizeof(display_info), 0);
        }
        //===========================================================================================================
        // Début du jeu
        //===========================================================================================================
        struct joueur *joueur_temporaire;
        int retour_recv; // sert a verifier si c'est une deconnection , erreur ou envoie
        int check_action_select = 0;
        printf("J'ai tout initialisé et je commence une partie \n");
        while (jeu.winner == 0)
        {
            while (lock != 2)
            {
                check_action_select = 0;
                tv.tv_usec = framerate * 1000;
                test_fd_set = read_fd_set; // pour eviter le reset des données via le select
                CHK(select(nb_desc_actuel, &test_fd_set, NULL, NULL, &tv));
                for (int i = 0; i < nb_client_actuel + 1; i++)
                {
                    if (tab_joueur[tab_desc_client[i].id_joueur1].alive == 1) // pour un joueur par client
                    {
                        if (FD_ISSET(tab_desc_client[i].desc_client, &test_fd_set))
                        {
                            // Le cas d'un nouveau client
                            if (tab_desc_client[i].desc_client == descServ)
                            {
                                CHECK(descNvCli = accept(descServ, (struct sockaddr *)&addrClient, &len));
                                FD_SET(descNvCli, &read_fd_set); // J'ai l'impression que ce FD_SET ne fonctionne pas ... car il y ne semble pas etre la dans le select car la connection se fait car il recoit le jeu mais il ne peut pas jouer
                                nb_client_actuel++;
                                nb_desc_actuel++;
                                placement_dans_tab = 0;
                                while (placement_dans_tab != max_client)
                                {
                                    if (tab_desc_client[placement_dans_tab].desc_client == -1)
                                    {
                                        tab_desc_client[placement_dans_tab].desc_client = descNvCli;
                                        break;
                                    }
                                    placement_dans_tab++;
                                }
                                CHK(recv(descNvCli, &cli_inf, sizeof(struct client_init_infos), 0));
                                // mise en spectateur du client plus init des futurs joueurs en attente
                                if (cli_inf.nb_players + nb_joueur_actuel <= MAX_JOUEUR)
                                {
                                    tab_joueur[nb_joueur_actuel].alive = 0;
                                    tab_desc_client[placement_dans_tab].nb_joueur = cli_inf.nb_players;
                                    tab_desc_client[placement_dans_tab].id_joueur1 = nb_joueur_actuel;
                                    nb_joueur_actuel++;
                                    if (cli_inf.nb_players == 2)
                                    {
                                        tab_joueur[nb_joueur_actuel].alive = 0;
                                        tab_desc_client[placement_dans_tab].id_joueur2 = nb_joueur_actuel;
                                        nb_joueur_actuel++;
                                    }
                                }
                            }
                            else
                            {
                                /*RECEPTION DES INPUTS*/
                                retour_recv = recv(tab_desc_client[i].desc_client, &touche, sizeof(struct client_input), 0);
                                if (retour_recv == 0)
                                {
                                    // Deconnection de joueur inopinné
                                    FD_CLR(tab_desc_client[i].desc_client, &read_fd_set);
                                    close(tab_desc_client[i].desc_client);
                                    tab_desc_client[i].desc_client = -1;                 // faire une meilleure chose car pas compatible mais fonctionne pour ne casser le jeu des autres
                                    tab_joueur[tab_desc_client[i].id_joueur1].alive = 0; // on kill le premier joueur du client
                                    if (tab_desc_client[i].nb_joueur == 2)
                                        tab_joueur[tab_desc_client[i].id_joueur2].alive = 0; // On kill le j2 si 2 j sur le mm client
                                    nb_joueur_actuel -= tab_desc_client[i].nb_joueur;
                                    nb_desc_actuel--;
                                    nb_client_actuel--;
                                    reattribution_place_tab_desc(tab_desc_client, max_client);
                                }
                                // a verifier si le retour de recv == alors deconnection
                                // selection du joueur en question qui envoie une touche
                                if (tab_desc_client[i].nb_joueur == 2) // on verifie le nombre de joueurs
                                {
                                    if (touche.id == 1)
                                        joueur_temporaire = &tab_joueur[tab_desc_client[i].id_joueur1];
                                    else
                                        joueur_temporaire = &tab_joueur[tab_desc_client[i].id_joueur2];
                                }
                                else
                                    joueur_temporaire = &tab_joueur[tab_desc_client[i].id_joueur1];
                                if ((int)touche.input == 4) // le trail
                                {
                                    joueur_temporaire->trail = (joueur_temporaire->trail + 1) % 2;
                                }

                                else if ((int)touche.input >= 0 && (int)touche.input < 4) // on va verifier la direction
                                {
                                    if (dir_checker(joueur_temporaire->direction, (int)touche.input))
                                        joueur_temporaire->direction = (int)touche.input;
                                }
                                else if ((int)touche.input == 5) // si le client veut quitter
                                {
                                    FD_CLR(tab_desc_client[i].desc_client, &read_fd_set);
                                    close(tab_desc_client[i].desc_client);
                                    tab_desc_client[i].desc_client = -1;                 // faire une meilleure chose car pas compatible mais fonctionne pour ne casser le jeu des autres
                                    tab_joueur[tab_desc_client[i].id_joueur1].alive = 0; // on kill le premier joueur du client
                                    if (tab_desc_client[i].nb_joueur == 2)
                                        tab_joueur[tab_desc_client[i].id_joueur2].alive = 0; // On kill le j2 si 2 j sur le mm client
                                    nb_joueur_actuel -= tab_desc_client[i].nb_joueur;
                                    nb_desc_actuel--;
                                    nb_client_actuel--;
                                    reattribution_place_tab_desc(tab_desc_client, max_client);
                                }
                                check_action_select = 1;
                            }
                        }
                    }
                }
                if (check_action_select == 0)
                    lock = 2;
            }
            int dernier_joueur = 0;
            int count = 0;
            for (int i = 0; i < nb_joueur_actuel; i++)
            {
                if (tab_joueur[i].alive == 1)
                {
                    update_plateau(&jeu, &tab_joueur[i], i);
                    count++;
                    dernier_joueur = i + 1;
                }
            }
            if (count == 1)
                jeu.winner = dernier_joueur;
            if (count == 0)
                jeu.winner = -1;
            /*ICI TRAITEMENT DES DONNEER DES JOUEUR*/
            for (int i = 0; i < nb_client_actuel + 1; i++)
            {
                if (tab_desc_client[i].desc_client != descServ)
                {
                    send(tab_desc_client[i].desc_client, &jeu, sizeof(display_info), 0);
                }
            }
            lock = 1;
        }
        jeu.winner = 0; // init pour que le jeu se relance ! a deplacer avec l'init tableau je pense
        sleep(5);       // pour le temps d'affichage du message de fin pour les clients
        nb_joueur_actuel = 0;
        for (int i = 0; i < nb_client_actuel; i++)
        {
            if (tab_desc_client[i].desc_client != descServ)
            {
                nb_joueur_actuel += tab_desc_client[i].nb_joueur;
            }
        }
    }
    CHK(close(descServ));
    return 0;
}
