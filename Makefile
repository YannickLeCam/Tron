CC = gcc
FLAGS= -Wall
FILE = src/client_template.c

#ARGUMENTS POUR LANCER LE(S) SERVEUR/CLIENT(S)
ADDRESS = 0.0.0.1
PORT = 5555
FRAMERATE = 500
UNJOUEUR = 1
DEUXJOUEURS = 2

srv:src/serveur.c
	@echo "Compilation du serveur en cours..."
	${CC} ${FLAGS} -o serveur src/serveur.c
	@echo "Le serveur a ete compilé"
	./serveur ${ADDRESS} ${PORT} ${FRAMERATE}

cli:src/client.c 
	@echo "Compilation du client en cours ..."
	${CC} ${FLAGS} -o client src/client.c -lncurses
	@echo "Le client a été compilé"
	./client ${ADDRESS} ${PORT} ${UNJOUEUR}

cli2:src/client.c 
	@echo "Compilation du client en cours ..."
	${CC} ${FLAGS} -o client src/client.c -lncurses
	@echo "Le client a été compilé"
	./client ${ADDRESS} ${PORT} ${DEUXJOUEURS}


launch: 
	gnome-terminal
	gnome-terminal
clean:
	@echo "je nez toi"
	rm serveur
	rm client
