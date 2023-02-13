/// @author : Guyader Ludovic ,Sentchev Vassili
/// @file : fichier client v2
/// @version : 2
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
    // ---------------------- VARIABLES ----------------------

    // ------------
    // --- SERVEUR
    // ------------
    int socket_serveur, port;
    struct sockaddr_in serv_addr; // Structures pour enregistrer l'adresse du serveur et de client


    // ------------
    // --- CLIENT 1
    // ------------
    int socket_client1;
    socklen_t cli1_len;           // Longueur de l'adresse client 1
    struct sockaddr_in cli1_addr; // Structures pour enregistrer l'adresse de client 1

    char mot_en_cours_cli1[25];
    char lettre_deja_dites_cli1[25] = "";
    int nb_erreurs_cli1 = 0;


    // ------------
    // --- CLIENT 2
    // ------------
    int socket_client2;
    socklen_t cli2_len;
    struct sockaddr_in cli2_addr;

    char mot_en_cours_cli2[25];
    char lettre_deja_dites_cli2[25] = "";
    int nb_erreurs_cli2 = 0;


    // ------------------
    // --- CLIENT ACTUEL
    // ------------------
    int *socket_clientActuel;

    char (*mot_en_cours_cliActuel)[25];
    char (*lettre_deja_dites_cliActuel)[25];
    int *nb_erreurs_cliActuel;


    // ------------
    // --- AUTRES
    // ------------
    char message_recu[256];                 // buffer pour stocker les messages envoyés par le client
    int n;                                  // Nombre d'octets lus ou écrits
    char message[64] = "";
    int compteur = 0;
    int fin_de_partie = -1;


    // ---------------------- CONNECTIONS ----------------------


    // Vérifie si le port a été fourni en argument
    if (argc < 2)
    {
        fprintf(stderr, "erreur, mettre un port svp \n");
        exit(1);
    }

    // Crée le socket
    socket_serveur = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_serveur < 0) {error("erreur ouverture socket");}

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
    if (bind(socket_serveur, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {error("bind");}

    // Écouter les demandes de connexion
    listen(socket_serveur, 5);

    // Récupère la longueur des adresses clients
    cli1_len = sizeof(cli1_addr);
    cli2_len = sizeof(cli2_addr);

    // Boucle infinie pour accepter les connexions des clients
    while (1)
    {
        // Accepter une connexion 1
        socket_client1 = accept(socket_serveur, (struct sockaddr *)&cli1_addr, &cli1_len);
        if (socket_serveur < 0) {error("Connection joueur 1 non acceptée");}

        // Accepter une connexion 2
        socket_client2 = accept(socket_serveur, (struct sockaddr *)&cli2_addr, &cli2_len);
        if (socket_serveur < 0) {error("Connection joueur 2 non acceptée");}
        


        // ---------------------- DEBUT JEU ----------------------


        char *mot_final = choisir_mot();
        // char *mot_final = "AO";
        initialiser_mot_en_cours(mot_en_cours_cli1, mot_final);
        initialiser_mot_en_cours(mot_en_cours_cli2, mot_final);
        printf("Le mot à deviner est : %s", mot_final);


        // Boucle pour lire les messages du client
        // Boucle interne pour permettre la réception de plusieurs messages du client
        while (fin_de_partie == -1)
        {
            // Alternance des clients : 
            if ((compteur % 2) == 0) //client 1 : Les variables du client actuel pointent vers celles du client 1
            {
                socket_clientActuel = &socket_client1;

                mot_en_cours_cliActuel = &mot_en_cours_cli1;
                lettre_deja_dites_cliActuel = &lettre_deja_dites_cli1;
                nb_erreurs_cliActuel = &nb_erreurs_cli1;
            }
            else //client 2 : Les variables du client actuel pointent vers celles du client 1
            {
                socket_clientActuel = &socket_client2;

                mot_en_cours_cliActuel = &mot_en_cours_cli2;
                lettre_deja_dites_cliActuel = &lettre_deja_dites_cli2;
                nb_erreurs_cliActuel = &nb_erreurs_cli2;
            }
            compteur = compteur + 1;

            lettre_deja_dite_ou_invalide: // Point de retour pour le goto lorsque qu'un joueur a d"jà dit une lettre ou qu'elle est invalide

            // Coeur du jeu
            printf("Le mot à deviner est : %s", mot_final);
            printf("Le client en est à : %s", *mot_en_cours_cliActuel);


            bzero(message, 64); // Mettre à zéro le tampon pour stocker le message
            sprintf(message, "Erreurs :  %d/6\nMot : %s", *nb_erreurs_cliActuel, *mot_en_cours_cliActuel); 

            n = write(*socket_clientActuel, message, strlen(message));
            // Si l'envoi échoue, afficher une erreur
            if (n < 0) {error("erreur d'écriture sur le socket");}

            bzero(message_recu, 256);

            // Lire le message envoyé par le client
            n = read(*socket_clientActuel, message_recu, 255);

            // Si la lecture échoue, afficher une erreur
            if (n < 0) {error("erreur de lecture sur le socket");}

            // Si aucun message n'est lu, la connexion est fermée
            if (n == 0) {break;}

            // Afficher le message reçu
            printf("\nLettre recu du client: %s\n", message_recu);
            upper(&message_recu[0]);
            if (verif_est_une_lettre(message_recu[0]) == 0)
            {
                // lettre valide

                if (verif_lettre_deja_dites(message_recu[0], *lettre_deja_dites_cliActuel) == 0)
                {
                    // nouvelle lettre
                    if (est_une_lettre_du_mot(message_recu[0], mot_final) == 0)
                    {
                        bonne_reponse(message_recu[0], *mot_en_cours_cliActuel, mot_final);
                    }
                    else
                    {
                        // mauvaise_reponse(&nb_erreurs_cliActuel);
                        *nb_erreurs_cliActuel = *nb_erreurs_cliActuel + 1;
                    }
                    fin_de_partie = est_termine(*mot_en_cours_cliActuel, mot_final, *nb_erreurs_cliActuel);
                }
                else
                {
                    // printf("Lettre déjà dites\n");
                    n = write(*socket_clientActuel, "Lettre déjà dites", strlen(message));
                    if (n < 0) {error("erreur d'écriture sur le socket");}

                    goto lettre_deja_dite_ou_invalide; // Permet de redemander une lettre
                }
            }
            else
            {
                // printf("Lettre invalide\n");
                n = write(*socket_clientActuel, "Lettre invalide", strlen(message));
                if (n < 0) {error("erreur d'écriture sur le socket");}
                goto lettre_deja_dite_ou_invalide; // Permet de redemander une lettre
            }
            if (fin_de_partie == 0){ 
                // Cas si le joueur actuel à perdu
                //      Donc avertir joueur actuel qu'il a perdu | L'autre qu'il a gagné

                bzero(message, 64);
                sprintf(message, "Dommage tu as perdu. Le mot était : %s\n", mot_final);

                n = write(*socket_clientActuel, message, strlen(message));
                if (n < 0) {error("Erreur d'écriture sur le socket");}

                //Si le joueur 1 a gagné
                if (compteur%2==0) 
                {
                    sprintf(message, "Le joueur 2 a perdu, le mot était : %s\n", mot_final);
                    n = write(socket_client1, message, strlen(message));
                    if (n < 0) {error("Erreur d'écriture sur le socket");}
                }
                //Si le joueur 2 a gagné
                else
                {
                    sprintf(message, "Le joueur 1 a perdu, le mot était : %s\n", mot_final); 
                    n = write(socket_client2, message, strlen(message));
                    if (n < 0) {error("Erreur d'écriture sur le socket");}
                }
                break;
            }else if (fin_de_partie == 1){
                bzero(message, 64);

                // Avertir le joueur actuel qu'il a gagné
                sprintf(message, "Bravo tu as gagné. Le mot était bien : %s\n", mot_final); 

                n = write(*socket_clientActuel, message, strlen(message));
                if (n < 0) {error("Erreur d'écriture sur le socket");}

                bzero(message, 64);

                // Avertir l'autre jour qu'il a perdu 
                //Si le joueur 1 a gagné
                if (compteur%2==0) 
                {
                    sprintf(message, "Le joueur 2 a gagné , le mot était : %s\n", mot_final);
                    n = write(socket_client1, message, strlen(message));
                    if (n < 0) {error("Erreur d'écriture sur le socket");}
                }
                //Si le joueur 2 a gagné
                else
                {
                    sprintf(message, "Le joueur 1 a gagné , le mot était : %s\n", mot_final); 
                    n = write(socket_client2, message, strlen(message));
                    if (n < 0) {error("Erreur d'écriture sur le socket");}
                }
                break;
            }
        }
    }
    // Fermer le socket du serveur
    close(socket_serveur);
    return 0;
}
