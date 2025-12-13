#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <vector>
#include <iostream>

/*
 * DEMONSTRATION DE POLL
 * 
 * Ce programme montre comment poll() fonctionne pour gérer plusieurs sockets simultanément.
 * 
 * FONCTIONNEMENT :
 * 1. Crée 2 sockets d'écoute sur les ports 8080 et 8081
 * 2. Utilise poll() pour surveiller tous les sockets (listeners + clients)
 * 3. Accepte les nouvelles connexions
 * 4. Lit les requêtes des clients
 * 5. Envoie une réponse HTTP
 */

int create_listen_socket(int port)
{
	// Créer le socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket");
		exit(1);
	}

	// Permettre la réutilisation du port
	int opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		perror("setsockopt");
		exit(1);
	}

	// Configuration de l'adresse
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	// Bind
	if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("bind");
		exit(1);
	}

	// Listen
	if (listen(sockfd, 10) < 0) {
		perror("listen");
		exit(1);
	}

	printf("✓ Socket d'écoute créé sur le port %d (fd=%d)\n", port, sockfd);
	return sockfd;
}

bool is_listen_socket(int fd, int listen1, int listen2)
{
	return (fd == listen1 || fd == listen2);
}

int main()
{
	printf("=== DEMONSTRATION DE POLL ===\n\n");

	// Créer 2 sockets d'écoute
	int listen_fd1 = create_listen_socket(8080);
	int listen_fd2 = create_listen_socket(8081);

	printf("\nServeur prêt. Testez avec :\n");
	printf("  curl http://localhost:8080\n");
	printf("  curl http://localhost:8081\n\n");

	// Préparer le tableau pour poll
	std::vector<struct pollfd> pollfds;

	// Ajouter les 2 sockets d'écoute
	struct pollfd pfd1;
	pfd1.fd = listen_fd1;
	pfd1.events = POLLIN;  // On veut être notifié quand on peut accept()
	pfd1.revents = 0;
	pollfds.push_back(pfd1);

	struct pollfd pfd2;
	pfd2.fd = listen_fd2;
	pfd2.events = POLLIN;
	pfd2.revents = 0;
	pollfds.push_back(pfd2);

	printf("État initial du tableau poll :\n");
	printf("  Index 0 : fd=%d (listen port 8080), events=POLLIN\n", listen_fd1);
	printf("  Index 1 : fd=%d (listen port 8081), events=POLLIN\n\n", listen_fd2);

	// Boucle principale
	while (true)
	{
		printf(">>> Appel de poll() sur %zu descripteurs...\n", pollfds.size());
		
		// POLL BLOQUE ICI jusqu'à ce qu'un événement arrive
		int ret = poll(pollfds.data(), pollfds.size(), -1);
		
		if (ret < 0) {
			perror("poll");
			break;
		}

		printf(">>> poll() a retourné : %d descripteur(s) prêt(s)\n\n", ret);

		// Parcourir tous les descripteurs
		for (size_t i = 0; i < pollfds.size(); i++)
		{
			// Ignorer ceux sans événement
			if (pollfds[i].revents == 0)
				continue;

			printf("[Index %zu, fd=%d] Événement détecté : revents=0x%x\n", 
			       i, pollfds[i].fd, pollfds[i].revents);

			// CAS 1 : Socket d'écoute prêt (nouvelle connexion)
			if (is_listen_socket(pollfds[i].fd, listen_fd1, listen_fd2))
			{
				if (pollfds[i].revents & POLLIN)
				{
					int port = (pollfds[i].fd == listen_fd1) ? 8080 : 8081;
					printf("  → Socket d'écoute port %d : nouvelle connexion entrante\n", port);

					// Accepter la connexion
					int client_fd = accept(pollfds[i].fd, NULL, NULL);
					if (client_fd < 0) {
						perror("accept");
						continue;
					}

					printf("  → accept() réussi : client_fd=%d\n", client_fd);

					// Ajouter le client au tableau poll
					struct pollfd client_pfd;
					client_pfd.fd = client_fd;
					client_pfd.events = POLLIN;  // Prêt à lire
					client_pfd.revents = 0;
					pollfds.push_back(client_pfd);

					printf("  → Client ajouté au tableau poll (index %zu)\n\n", pollfds.size() - 1);
				}
			}
			// CAS 2 : Client prêt à lire ou écrire
			else
			{
				// Lire la requête
				if (pollfds[i].revents & POLLIN)
				{
					printf("  → Client fd=%d : données disponibles pour lecture\n", pollfds[i].fd);

					char buffer[4096];
					int n = read(pollfds[i].fd, buffer, sizeof(buffer) - 1);
					
					if (n <= 0)
					{
						printf("  → Client déconnecté (read=%d)\n", n);
						close(pollfds[i].fd);
						pollfds.erase(pollfds.begin() + i);
						i--;
						printf("  → Client retiré du tableau poll\n\n");
					}
					else
					{
						buffer[n] = '\0';
						printf("  → Lu %d octets\n", n);
						printf("  → Première ligne : %.50s...\n", buffer);

						// Passer en mode écriture
						pollfds[i].events = POLLOUT;
						printf("  → events changé vers POLLOUT (prêt à écrire)\n\n");
					}
				}
				// Écrire la réponse
				else if (pollfds[i].revents & POLLOUT)
				{
					printf("  → Client fd=%d : prêt pour l'écriture\n", pollfds[i].fd);

					const char* html = 
						"<!DOCTYPE html>\n"
						"<html>\n"
						"<head><title>Demo Poll</title></head>\n"
						"<body>\n"
						"<h1>Poll fonctionne !</h1>\n"
						"<p>Cette réponse vient d'un serveur multi-port géré par poll()</p>\n"
						"</body>\n"
						"</html>\n";

					char response[8192];
					snprintf(response, sizeof(response),
						"HTTP/1.1 200 OK\r\n"
						"Content-Type: text/html; charset=UTF-8\r\n"
						"Content-Length: %zu\r\n"
						"Connection: close\r\n"
						"\r\n"
						"%s",
						strlen(html), html);

					write(pollfds[i].fd, response, strlen(response));
					printf("  → Réponse HTTP envoyée (%zu octets)\n", strlen(response));

					// Fermer et retirer du tableau
					close(pollfds[i].fd);
					pollfds.erase(pollfds.begin() + i);
					i--;
					printf("  → Connexion fermée et retirée du tableau poll\n\n");
				}

				// Erreur ou déconnexion
				if (pollfds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
				{
					printf("  → Erreur détectée (POLLHUP/POLLERR/POLLNVAL)\n");
					close(pollfds[i].fd);
					pollfds.erase(pollfds.begin() + i);
					i--;
					printf("  → Client fermé et retiré\n\n");
				}
			}
		}

		printf("État du tableau après traitement : %zu descripteurs\n", pollfds.size());
		for (size_t i = 0; i < pollfds.size(); i++) {
			const char* type = is_listen_socket(pollfds[i].fd, listen_fd1, listen_fd2) 
			                   ? "LISTEN" : "CLIENT";
			printf("  [%zu] fd=%d (%s), events=0x%x\n", 
			       i, pollfds[i].fd, type, pollfds[i].events);
		}
		printf("\n--- Attente du prochain événement ---\n\n");
	}

	// Nettoyage
	close(listen_fd1);
	close(listen_fd2);

	return 0;
}
