# <span style="color: #87CEEB;">Tron</span>

## <span style="color: #87CEEB;">Présentation</span>
Ce projet est un projet réalisé lors de mes études à l'<strong>Université de Strasbourg</strong> en 3ème année. Le principe était de reproduire le jeu Tron sur terminal en C, permettant à 2 joueurs de jouer en local ou jusqu'à 4 joueurs en ligne. Nous devions également respecter une condition qui était de faire du "**Cloud gaming**", c'est-à-dire que <span style="color: blue">le(s) client(s)</span> transmettait les inputs et <span style="color: blue">le serveur</span> transmettait l'interface entre chaque itération.

## <span style="color: #87CEEB;">Auteurs

- [@YannickLeCam](https://github.com/YannickLeCam)
- [@ViterDaniel]()

## <span style="color: #87CEEB;">Technologies utilisées</span>

**Client:** C (ncurses, termios)

**Server:** C (netinet, arpa)

## <span style="color: #87CEEB;">Déploiement</span>

Le projet de base est configuré sur l'adresse locale 0.0.0.1. Si vous souhaitez l'importer sur une adresse précise, vous pouvez la modifier dans le fichier Makefile, ainsi que le port souhaité.

Une fois l'adresse et le port paramétrés, déployez d'abord **le serveur** :

```bash
  make srv
```

Si vous êtes à deux sur la même machine/terminal, pour déployer **le client** pour **2 joueurs** :

```bash
  make cli2
```

Si vous etes seul sur une machine/terminal, pour déployer **le client** pour **1 joueur** :

```bash
  make cli
```

Une fois le serveur et le(s) client(s) déployé(s), le jeu se lancera automatiquement.

## <span style="color: #87CEEB;">Les Commandes en jeu</span>

Si vous êtes seul :

| Commande       | Description                |
|----------------|----------------------------|
| z ou i         | Se déplacer vers le **haut**   |
| q ou j         | Se déplacer vers la **gauche** |
| s ou k         | Se déplacer vers **le bas**    |
| d ou l         | Se déplacer vers **la droite** |


Si vous êtes deux joueurs :

| Joueur 1       | Joueur 2       |
|----------------|----------------|
| z = **haut**       | i = **haut**       |
| q = **gauche**     | j = **gauche**     |
| s = **bas**        | k = **bas**        |
| d = **droite**     | l = **droite**    |


Ces commandes vous permettent de contrôler les déplacements des joueurs dans le jeu <span style="color: #87CEEB;">Tron</span>. Amusez-vous bien ! 

