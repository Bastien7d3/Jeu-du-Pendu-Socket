# Résumé

Ceci est un projet réalisé dans le cadre d'une SAE en groupe de 4 sur la réalisation d'un jeu du pendu entre un serveur et plusieurs client en utilisant les sockets. Le programme est réalisé en C.

auteurs : 
- Tom LECLERCQ
- Bastien BRUNEL
- Ludovic GUYADER
- Vasili SENTCHEV

Le projet se compose en 3 version :
## Version 0
V0 est la version la plus simple du pendu, elle se compose d'un client qui se connecte à un seveur. Le serveur choisit un mot aléatoire parmi sa liste de mot connue et le fait deviner au client, qui peut alors sélectionner une lettre.

![This is an image](https://cdn.discordapp.com/attachments/1072197816543359016/1073390345347420190/Organigramme_SAE_v0.svg)

## Version 1 
V1 est très similaire à V0 sauf qu'ici la gestion de 2 parties en même temps est possible, 2 clients, 2 parties indépendantes, 2 fois plus de fun !
![This is an image](https://cdn.discordapp.com/attachments/1072197816543359016/1073549306025492531/v1.svg)

## Version 2
V2 est un peu différentes des 2 versions précédentes puisque le serveur ne fait plus que office de tube entre les 2 clients. En effet, lorsque le premier client se connecte au serveur, il se voit attribuer le rôle de "meneur". Il doit alors saisir un mot pour le faire deviner au deuxieme client. Le calcul des règles du jeu se font désormais chez le client "meneur".
![This is an image](https://cdn.discordapp.com/attachments/1072197816543359016/1073548956958732309/v2.svg)

## Version 3
V3 est la version la plus simple. Le serveur peut lancer plusieurs parties en même temps. Il suffit de fork le serveur et de lancer la version 2
![This is an image](https://cdn.discordapp.com/attachments/1072197816543359016/1073577029473144985/Organigramme_SAE_1.svg)

# Utilisation
Il faut toujours lancer le serveur avant les clients
Pour lancer le serveur on utilise la commande :
```c
./serveur port
```
avec un port supérieur à 5000

Pour lancer les clients on utilise la commande :
```c
./client 127.0.0.1 port
```
avec le même port que le serveur

lien pour voir les organigrammes :
https://lucid.app/lucidchart/8d13bcb7-2f32-4b10-b4fa-b88e63b0598f/edit?invitationId=inv_61d65879-128c-4de0-935f-0f6a4b340fef&page=0_0#
