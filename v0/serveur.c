/// @author : Leclercq Tom, Brunel Bastien
/// @file : fichier serveur v0
/// @version : 4
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
    struct sockaddr_in serv_addr; // Structures pour enregistrer l'adresse du serveur

    
    // ------------
    // --- CLIENT
    // ------------
    int socket_client;
    socklen_t clilen;              // Longueur de l'adresse client
    struct sockaddr_in cli_addr;

    
    // ------------
    // --- AUTRES
    // ------------
    char message_recu[256];         // buffer pour stocker les messages envoyés par le client
    int n;                          // Nombre d'octets lus ou écrits


    // ---------------------- CONNECTIONS ----------------------


    // Vérifie si le port a été fourni en argument
    if (argc < 2)
    {
        fprintf(stderr, "erreur, mettre un port svp \n");
        exit(1);
    }

    // Variables de la partie du pendu
    //////////////
    char mot_en_cours[25];
    char *mot_final = choisir_mot();
    initialiser_mot_en_cours(mot_en_cours, mot_final);
    char lettre_deja_dites[25] = "";
    int nb_erreurs = 0;
    int fin_de_partie = -1;
    char message[64] = "";
    //////////////

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
    clilen = sizeof(cli_addr);

    // Boucle infinie pour accepter les connexions des clients
    while (1)
    {
        // Accepter une connexion
        socket_client = accept(socket_serveur, (struct sockaddr *)&cli_addr, &clilen);
        if (socket_serveur < 0)
            error("connection non accepté");
        else
        {
            printf("nouvelle connexion\n");

        }
        

        // ---------------------- DEBUT JEU ----------------------
        
        //reset les parametres ici
        printf("parametres reinitialise\n");
        mot_final = choisir_mot();
        initialiser_mot_en_cours(mot_en_cours, mot_final);
        bzero(lettre_deja_dites, 25);
        nb_erreurs = 0;
        fin_de_partie = -1;

        while (fin_de_partie == -1)
        {
            printf("-----Nouveau-Tour-----\n");
            // Répondre au client que le message a été reçu
            printf("%s", mot_en_cours);
            bzero(message, 64);
            sprintf(message, "nb errors :  %d\nword : %s", nb_erreurs, mot_en_cours); 

            n = write(socket_client, message, strlen(message));
            // Si l'envoi échoue, afficher une erreur
            if (n < 0)
                error("erreur d'écriture sur le socket");

            // Mettre à zéro le tampon pour stocker le message
            bzero(message_recu, 256);

            // Lire le message envoyé par le client
            n = read(socket_client, message_recu, 255);

            // Si la lecture échoue, afficher une erreur
            if (n < 0)
                error("erreur de lecture sur le socket");

            // Si aucun message n'est lu, la connexion est fermée
            if (n == 0)
                break;

            // Afficher le message reçu
            printf("\nLettre recu du client: %s\n", message_recu);

            //on convertie la lettre minuscule en majuscule 
            upper(&message_recu[0]);
            if (verif_est_une_lettre(message_recu[0]) == 0)
            {
                // lettre valide, qui n'a jamais été utilisé
                if (verif_lettre_deja_dites(message_recu[0], lettre_deja_dites) == 0)
                {
                    // la lettre est dans le mot
                    if (est_une_lettre_du_mot(message_recu[0], mot_final) == 0)
                    {
                        // on remplace les _ par la lettre correcte
                        bonne_reponse(message_recu[0], mot_en_cours, mot_final);
                        printf("bonne réponse !\n");
                    }
                    //la lettre n'est pas dans le mot
                    else
                    {
                        //on incrémente le nombre d'érreur
                        mauvaise_reponse(&nb_erreurs);
                        printf("la lettre n'est pas dans le mot\n");
                        printf("%s\n", mot_final);
                       
                    }
                    fin_de_partie = est_termine(mot_en_cours, mot_final, nb_erreurs);
                }
                else
                {
                    printf("lettre deja dites\n");
                }
            }
            else
            {
                printf("lettre invalide\n");
            }
        // la partie est perdu 
        if (fin_de_partie == 0){

            bzero(message, 64);
            sprintf(message, "Perdu le mot était : %s", mot_final);
            n = write(socket_client, "perdu", 5);

            // Si l'envoi échoue, afficher une erreur
            if (n < 0)
                error("erreur d'écriture sur le socket");
            
            close(socket_client);
            break;
        }else if (fin_de_partie == 1){

            bzero(message, 64);
            sprintf(message, "Gagne le mot était bien: %s", mot_final); 
            n = write(socket_client, message, strlen(message));

            // Si l'envoi échoue, afficher une erreur
            if (n < 0)
                error("erreur d'écriture sur le socket");
            
            // Fermer la connexion avec ce client
            close(socket_client);
            break;
        }
        }
        

    }
    // Fermer le socket du serveur
    close(socket_serveur);
    return 0;
}
