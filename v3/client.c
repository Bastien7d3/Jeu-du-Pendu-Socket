//  ./client 127.0.0.1 5050
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "./pendu.c"

// Fonction pour afficher un message d'erreur
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void EnvoyerMsgA(int socket_client)
{
    int n;
    char message[256];
    // Envoie le message au serveur//

    printf(">>> ");             // Afficher un message invitant l'utilisateur à entrer un message
    bzero(message, 256);        // Réinitialiser le tampon de mémoire
    fgets(message, 255, stdin); // Lit un message à partir de l'entrée standard

    n = write(socket_client, message, strlen(message)); // Écrire le message dans le socket
    if (n < 0)
        error("Erreur d'écriture sur le socket"); // Gérer les erreurs d'écriture dans le socket
    // bzero(message, 256);
}

void RecevoirDe(int socket_client)
{
    int n;
    char message[256];
    // Receptionne les messages du serveur//

    bzero(message, 256);

    n = read(socket_client, message, 256); // Lire la réponse du serveur depuis le socket
    if (n < 0)
        error("Erreur de lecture sur le socket"); // Gérer les erreurs de lecture dans le socket
    upper(message);
    printf("%s\n", message); // Afficher la réponse du serveur

    char ret = strstr(message, "victoire") || strstr(message, "defaite");
    if (ret)
    {
        printf("fin de la partie\n");
        close(socket_client);
        exit(-1);
    }
}

void corriger_lettre(int socket_client, char mot_final[256], char mot_en_cours[25])
{
    char message[256];
    int n;
    bzero(message, 256); // Réinitialiser le tampon de mémoire

    strcpy(message, mot_en_cours);
    printf("envoie de la reponse corrigee au serveur\n");
    upper(message);
    n = write(socket_client, message, strlen(message)); // Écrire le message dans le socket
    if (n < 0)
        error("Erreur d'écriture sur le socket"); // Gérer les erreurs d'écriture dans le socket
}

int main(int argc, char *argv[])
{
    //////////////////////////////Ouverture Socket///////////////////////////////////////
    int socket_client, port, J;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    // Vérifie si le nombre d'arguments passé au programme est suffisant
    if (argc < 3)
    {
        fprintf(stderr, "mettre %s localhost et un port\n", argv[0]);
        exit(0);
    }
    // Convertit le numéro de port en entier
    port = atoi(argv[2]);

    // Crée un nouveau socket
    socket_client = socket(AF_INET, SOCK_STREAM, 0);

    // Vérif l'ouverture du socket
    if (socket_client < 0)
        error("erreur ouverture du socket");

    // Récupère les informations de l'hôte à partir de son nom d'hôte
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "Erreur host \n");
        exit(0);
    }

    // Initialise la structure sockaddr_in avec des zéros
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    // Copie les informations de l'hôte dans la structure sockaddr_in
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    // Établit une connexion au serveur
    if (connect(socket_client, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("Erreur de connection");

    //////////////////////////////Fin Ouverture Socket////////////////////////////////////

    int n;
    char message[256], lettre;
    char mot_en_cours[25];
    char lettre_deja_dites[25] = "";
    int nb_erreurs = 0;
    int fin_de_partie = -1;
    char mot_final[256];
    // Receptionne le message du serveur pour connaitre le role de chaque joueur//

    bzero(message, 256);
    printf("read\n");
    n = read(socket_client, message, 256); // Lire la réponse du serveur depuis le socket
    if (n < 0)
        error("Erreur de lecture sur le socket"); // Gérer les erreurs de lecture dans le socket
    upper(message);
    printf("%s\n", message); // Afficher la réponse du serveur

    // si le message comprend J1, il s'agit du joueur1
    if (strstr(message, "J1"))
    {
        printf("je suis le J1\n");
        J = 1;

        // On envoie alors au serveur le mot a faire deviner//

        printf(">>> ");             // Afficher un message invitant l'utilisateur à entrer un message
        bzero(message, 256);        // Réinitialiser le tampon de mémoire
        bzero(mot_final, 256);      // Réinitialiser le tampon de mémoire
        fgets(message, 255, stdin); // Lit un message à partir de l'entrée standard
        for (int i = 0; i < strlen(message); i++)
        {
            upper(&message[i]);
        }

        strcpy(mot_final, message); // Garde le message envoye comme mot a deviner
        initialiser_mot_en_cours(mot_en_cours, mot_final);

        printf("%s", message);
        n = write(socket_client, message, strlen(message)); // Écrire le message dans le socket
        if (n < 0)
            error("Erreur d'écriture sur le socket"); // Gérer les erreurs d'écriture dans le socket
        printf("-----\nmot en cours : %smot final : %s\n", mot_en_cours, mot_final);
    }
    else
    {
        printf("je suis le J2\n");
        J = 2;
    }

    // Boucle de jeu
    while (fin_de_partie == -1)
    {
        if (J == 1)
        {
            // affiche le recap des valeurs

            // Lecture de la lettre choisie par J2
            bzero(message, 256);
            n = read(socket_client, message, 1); // Lire la réponse du serveur depuis le socket
            if (n < 0)
                error("Erreur de lecture sur le socket"); // Gérer les erreurs de lecture dans le socket
            upper(&message[0]);
            
            corriger_lettre(socket_client, mot_final, mot_en_cours);
            printf("-----\nmot en cours : %smot final : %s\n", mot_en_cours, mot_final);

            // Verification de la validite de la lettre
            if (verif_est_une_lettre(message[0]) == 0)
            {
                printf("message recu = %s", message);
                printf("lettre valide\n");

                if (verif_lettre_deja_dites(message[0], lettre_deja_dites) == 0)
                {
                    printf("nouvelle lettre\n");
                    if (est_une_lettre_du_mot(message[0], mot_final) == 0)
                    {
                        bonne_reponse(message[0], mot_en_cours, mot_final);
                        printf("mot en cours : %s", mot_en_cours);
                        printf("bonne réponse !\n");

                    }
                    else
                    {
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

        }
        else if (J == 2)
        {
            printf("envoyer la lettre\n");
            EnvoyerMsgA(socket_client);
            printf("read\n");
            RecevoirDe(socket_client);
        }
        if (fin_de_partie == 0)
        {
            bzero(message, 256); // Réinitialiser le tampon de mémoire
            strcpy(message, "defaite");
            printf("envoie du message de defaite au J1\n");
            // upper(message);
            n = write(socket_client, message, strlen(message)); // Écrire le message dans le socket
            if (n < 0)
                error("Erreur d'écriture sur le socket"); // Gérer les erreurs d'écriture dans le socket
        }
        else if (fin_de_partie == 1)
        {
            bzero(message, 256); // Réinitialiser le tampon de mémoire
            strcpy(message, "victoire");
            printf("envoie du message de victoire au J1\n");
            n = write(socket_client, message, strlen(message)); // Écrire le message dans le socket
            if (n < 0)
                error("Erreur d'écriture sur le socket"); // Gérer les erreurs d'écriture dans le socket
        }
    }

    // Fermer le socket
    close(socket_client);
    return 0;
}
