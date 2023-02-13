/// @author : Guyader Ludovic ,Sentchev Vassili
/// @file : fichier client v1
/// @version : 2
/// @date : 10/02/2023


//  ./client 127.0.0.1 5050
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

// Fonction pour afficher un message d'erreur
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

/**
 * @brief methode qui permet d'envoyer un msg au serveur et de recevoir la réponse
 * 
 * @param socket_client 
 */
void message(int socket_client){
        int n;
        char message_envoye[256];
        char erreurs[256];
        bzero(message_envoye,256);

        // Receptionne les messages du serveur//

        n = read(socket_client,message_envoye,256);             // Lire la réponse du serveur depuis le socket
        // printf("Le message envoyé : %s\n", message_envoye);
        if (n < 0) error("Erreur de lecture sur le socket");    // Gérer les erreurs de lecture dans le socket
        printf("%s\n",message_envoye);                          // Afficher la réponse du serveur

        char ret = strstr(message_envoye, "gagn") || strstr(message_envoye, "perdu");
        if (ret){
            close(socket_client);
            exit(-1);
        } 

        // Envoie le message au serveur//

        printf("Entrer une lettre --> ");                       // Afficher un message invitant l'utilisateur à entrer un message
        
        bzero(message_envoye,256);                              // Réinitialiser le tampon de mémoire
        fgets(message_envoye,255,stdin);                        // Lit un message à partir de l'entrée standard

                    
        
        n = write(socket_client,message_envoye,1);              // Écrire le message dans le socket
        if (n < 0) error("Erreur d'écriture sur le socket");    // Gérer les erreurs d'écriture dans le socket
        bzero(message_envoye,256);                              // Réinitialiser le tampon de mémoire
        

}

int main(int argc, char *argv[])
{
    //////////////////////////////Ouverture Socket///////////////////////////////////////
    int socket_client, port;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    // Vérifie si le nombre d'arguments passé au programme est suffisant
    if (argc < 3) {
       fprintf(stderr,"mettre %s localhost et un port\n", argv[0]);
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
    if (server == NULL) {
        fprintf(stderr,"Erreur host \n");
        exit(0);
    }

    // Initialise la structure sockaddr_in avec des zéros
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    // Copie les informations de l'hôte dans la structure sockaddr_in
    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(port);

    // Établit une connexion au serveur
    if (connect(socket_client,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("Erreur de connection");

    //////////////////////////////Fin Ouverture Socket////////////////////////////////////

    // Boucle infinie pour envoyer et recevoir des messages
    while(1) message(socket_client);
    
    // Fermer le socket
    close(socket_client);
    return 0;
}
