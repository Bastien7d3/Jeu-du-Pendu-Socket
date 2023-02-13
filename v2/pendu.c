/// @author : Leclercq Tom, Brunel Bastien
/// @file : fichier client v2
/// @version : 4
/// @date : 10/02/2023


#include <stdio.h> // inclusion de la bibliotheque d'entrees/sorties
#include <stdlib.h> // inclusion de la bibliotheque standard
#include <string.h>
#include <ctype.h> // inclusion de la bibliothèque pour utiliser isalpha
#include <time.h> // inclusion de la bibliothèque pour le random


//variable global
char tab_mots_a_deviner[][20] = {"ANDOLOUSIE","AMOUR","AMITIE","INSOLATION","CHAUVE", "NUCLEARISATION"};
int taille_tab_mots_a_deviner=sizeof(tab_mots_a_deviner) / sizeof(tab_mots_a_deviner[0]);


// Fonction qui génère un mot parmi ceux de la variable global tab_mot_à_deviner.
char* choisir_mot()
{
    // génére un nombre entre 1 et la taille de la variable global tab mot_à_deviner
    srand(time(NULL));
    int rand_int = rand() % taille_tab_mots_a_deviner;

    char *mot_selectionne = tab_mots_a_deviner[rand_int];
    
    return mot_selectionne;
}

void upper(char *lettre)
{
    *lettre = toupper(*lettre);
}

// Fonction qui initialise le mot qui sera affiché aux joueurs
void initialiser_mot_en_cours(char mot_en_cours[20], const char *mot_final)
{
    int len = strlen(mot_final); // Recup taille du mot

    for (int i = 0; i < len; i++) {
        if (isalpha(mot_final[i])) // Verif si le caractère est une lettre
        {
            mot_en_cours[i] = '_';
        }
        else
        {
            mot_en_cours[i] = mot_final[i];
        }
    }
    mot_en_cours[len] = '\0';
}

// Fonction qui vérifie si le caractère donné est une lettre
// Retourne :
//       0 : Si le caractère est une lettre
//      -1 : Si le caractère n'est pas une lettre
int verif_est_une_lettre(char lettre)
{
    if (isalpha(lettre))
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

// Fonction qui vérifie si une lettre a déjà été dites
// Retourne :
//       0 : Si la lettre n'a jamais été dites
//      -1 : Si la lettre a déjà été dites
int verif_lettre_deja_dites(char lettre, char lettre_deja_dites[])
{
    int len = strlen(lettre_deja_dites); // Recup taille du mot

    for (int i = 0; i < len; i++) {
        if (lettre_deja_dites[i] == lettre) // Verif si le caractère est une lettre
        {
            return -1;
        }
    }

    lettre_deja_dites[len] = lettre; // Ajout de la nouvelle lettre dans cette déjà dit
    lettre_deja_dites[len + 1] = '\0'; 

    printf("Lettres dites : %s\n", lettre_deja_dites);
    return 0;
}

// Fonction qui vérifie si une lettre est dans le mot à trouver
// Retourne :
//      0 : Si la lettre est dans le mot
//     -1 : Si la lettre n'est pas dans le mot
int est_une_lettre_du_mot(char lettre, const char *mot_final)
{
    int len = strlen(mot_final); // Recup taille du mot

    for (int i = 0; i < len; i++) {
        if (mot_final[i] == lettre) // Verif si le caractère est une lettre
        {
            return 0;
        }
    }
    return -1;
}

// Fonction qui change les '_' du mot à trouver par la ou les lettres trouvées.
void bonne_reponse(char lettre, char mot_en_cours[], const char *mot_final)
{
    int len = strlen(mot_en_cours); // Recup taille du mot

    for (int i = 0; i < len; i++) {
        if (mot_final[i] == lettre) // Verif si le caractère est une lettre
        {
            mot_en_cours[i] = lettre;
        }
    }
}

// Fonction qui incrémente le nbr d'erreur.
void mauvaise_reponse(int *nbr_erreurs)
{
    *nbr_erreurs += 1;
}


// Fonction qui vérifie si une partie est terminée et quel est le résultat.
// Retourne -1 si la partie n'est pas terminée
//          0 si partie perdu
//          1 si partie gagnée
int est_termine(char mot_en_cours[] ,const char mot_final[], int nbr_erreurs)
{
    const int vie_max=6;
    if (nbr_erreurs == vie_max)
    {
        return 0;
    }
    else if (strcmp(mot_en_cours, mot_final) == 0) // strcmp : vérifie si les 2 mots sont égaux.
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

// Fonction qui affiche le mot en cours 
void affichage_mot(char mot_en_cours[])
{
    char mot_en_cours_espace[100];
    int j = 0;
    for (int i = 0; i < strlen(mot_en_cours); i++) {
        mot_en_cours_espace[j] = toupper(mot_en_cours[i]);
        j++;
        mot_en_cours_espace[j] = ' ';
        j++;
    }
    mot_en_cours_espace[j - 1] = '\0';

    printf("+");
    for (int i = 0; i < strlen(mot_en_cours_espace); i++) {
        printf("-");
    }
    printf("+\n");

    printf("|%s|\n", mot_en_cours_espace);

    printf("+");
    for (int i = 0; i < strlen(mot_en_cours_espace); i++) {
        printf("-");
    }
    printf("+\n");
}


//fonction qui affiche le bonhomme du pendu en fonction de l'etat des erreurs de la partie
void affichage_pendu(int nbs_erreurs)
{
    switch (nbs_erreurs)
    {
    // 0 erreur
        case 0:
            printf("\n");
            printf(" __________\n");
            printf("|          \n");
            printf("|           \n");
            printf("|           \n");
            printf("|           \n");
            printf("|\n");
            printf("|\n");
            printf("\n");
            break;
            // 1 erreur
        case 1:

            printf("\n");printf(" __________\n");printf("|         |\n");printf("|         O \n");printf("|           \n");printf("|           \n");printf("|\n");printf("|\n");printf("\n");break;
            // 2 erreurs
        case 2:
            printf("\n");printf(" __________\n");printf("|         |\n");printf("|         O \n");printf("|         | \n");printf("|           \n");printf("|\n");printf("|\n");printf("\n");break;
            // 3 erreurs
        case 3:
            printf("\n");printf(" __________\n");printf("|         |\n");printf("|         O\n");printf("|         |\\\n");printf("|           \n");printf("|\n");printf("|\n");printf("\n");break;
            // 4 erreurs
        case 4:
            printf("\n");printf(" __________\n");printf("|         |\n");printf("|         O\n");printf("|        /|\\\n");printf("|           \n");printf("|\n");printf("|\n");printf("\n");break;
            // 5 erreurs
        case 5:
            printf("\n");printf(" __________\n");printf("|         |\n");printf("|         O\n");printf("|        /|\\\n");printf("|        / \n");printf("|\n");printf("|\n");printf("\n");
            break;
            // 6 erreurs
        case 6:
            printf("\n");printf(" __________\n");printf("|         |\n");printf("|         O\n");printf("|        /|\\\n");printf("|        / \\\n");printf("|\n");printf("|\n");printf("\n");
            break;
        default:
            break;
    }
}

void affichage_fin_partie(char mot_final[], int a_gagne)
{
    printf("\t-----------\t\n");
    if (a_gagne == 0)
    {
        
        printf("\tDEFAITE\t\n");
        printf("Le mot était : %s", mot_final);
    }
    else
    {
        printf("\tFELICITATION\t\n");
    }
}




