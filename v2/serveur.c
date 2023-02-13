/// @author : Leclercq Tom, Brunel Bastien
/// @file : fichier client v0
/// @version : 5
/// @date : 10/02/2023


//  ./serveur 5050
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "./pendu.c"

// Fonction pour afficher les erreurs
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int socket_serveur, port, socket_client, socket_client_1, socket_client_2;
    socklen_t clilen1, clilen2; // Longueur de l'adresse client
    char message_recu[128];     // buffer pour stocker les messages envoyés par le client
    char message_recu2[128];
    struct sockaddr_in serv_addr, cli_addr1, cli_addr2; // Structures pour enregistrer l'adresse du serveur et de client
    int n;
    int compteur = 0; // Nombre d'octets lus ou écrits

    // Vérifie si le port a été fourni en argument
    if (argc < 2)
    {
        fprintf(stderr, "erreur, mettre un port svp \n");
        exit(1);
    }

    /////////////////////////////////////////////////////////
    char *mot_en_cours;
    int nb_erreurs = 0;
    // char *mot_final;
    int fin_de_partie = -1;
    char message[256] = "";
    /////////////////////////////////////////////////////////

    // Crée le socket
    socket_serveur = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_serveur < 0)
    {
        error("erreur ouverture socket");
    }

    // Remplit la structure de l'adresse du serveur avec des 0
    bzero((char *)&serv_addr, sizeof(serv_addr));

    // Récupère le numéro de port à partir des arguments
    port = atoi(argv[1]);

    // Remplit la structure de l'adresse du serveur
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    // Convertit le numéro de port en format réseau
    serv_addr.sin_port = htons(port);

    // Associe le socket à l'adresse du serveur
    if (bind(socket_serveur, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("bind");
    }

    // Écouter les demandes de connexion
    listen(socket_serveur, 5);

    // Récupère la longueur de l'adresse client
    clilen1 = sizeof(cli_addr1);
    clilen2 = sizeof(cli_addr2);

    // Boucle infinie pour accepter les connexions des clients
    while (1)
    {

        // Accepter une connexion du client 1
        socket_client_1 = accept(socket_serveur, (struct sockaddr *)&cli_addr1, &clilen1);
        if (socket_serveur < 0)
            error("connection non accepté");

        // Accepter une connexion du client 2
        socket_client_2 = accept(socket_serveur, (struct sockaddr *)&cli_addr2, &clilen2);
        if (socket_serveur < 0)
            error("connection non accepté");

        printf("connexion reussie avec les 2 clients\n");

        if (compteur % 2 == 0)
        {
            socket_client = socket_client_1;
        }
        else
        {
            socket_client = socket_client_2;
        }
        compteur++;

    // demander le mot au client1
        bzero(message, 256);
        bzero(message_recu, 128);

        sprintf(message, "J1 : Choisir un mot a faire deviner ");
        //upper(message);
        n = write(socket_client_1, message, strlen(message));
        if (n < 0)
            error("erreur d'écriture sur le socket");

    // lecture du mot du client1
        n = read(socket_client_1, message_recu, 128);
        if (n < 0)
            error("erreur de lecture sur le socket");
        printf("mot du joueur 1 a faire deviner : %s\n", message_recu);

        char *mot_final = message_recu;

        char mot_en_cours[128];
        initialiser_mot_en_cours(mot_en_cours, mot_final);
        printf("Mot final : %s \nMot en cours %s\n", mot_final, mot_en_cours);

        // Boucle pour lire les messages du client
        // Boucle interne pour permettre la réception de plusieurs messages du client
        while (fin_de_partie == -1)
        {
        // envoyer le mot au client2
            bzero(message, 256);
            //upper(message);
            sprintf(message, "Voici le mot a deviner : %s\nProposer une lettre", mot_en_cours);
            n = write(socket_client_2, message, strlen(message));
            if (n < 0)
                error("erreur d'écriture sur le socket");
        
            
            bzero(message,256);

        if ((strstr(message_recu, "victoire")||(strstr(message_recu, "defaite"))))
            {
                exit(-1);
            }
        // attendre la lettre de client2 (1 caractère en maj)
        printf("j'attend la lettre de J2\n");
            bzero(message_recu2, 128);
            printf("read J2\n");
            n = read(socket_client_2, message_recu2, 128);

            if (n < 0)
                error("erreur de lecture sur le socket");
            printf("Lettre de Joueur 2 : %s\n", message_recu2);

        
        // transmettre la lettre au client1
            bzero(message, 256);
            sprintf(message, "%s\n", message_recu2);
            //upper(message);
            n = write(socket_client_1, message, strlen(message));
            if (n < 0)
                error("erreur d'écriture sur le socket");
            printf("j'ai envoye la lettre a J1\n");

        // attendre la réponse de client1 (mot actualisé)
            bzero(message_recu, 128);
            printf("read J1\n");
            n = read(socket_client_1, message_recu, 128);

            if (n < 0)
                error("erreur de lecture sur le socket");

            printf("Mot actualise : %s\n", message_recu);

            strcpy(mot_en_cours, message_recu);

            
            
            
        }

        // Fermer la connexion avec ce client
        // close(socket_serveur);
    }
    // Fermer le socket du serveur
    close(socket_serveur);
    return 0;
}
