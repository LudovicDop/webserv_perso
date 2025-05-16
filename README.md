# webserv

## En quoi consiste ce projet?

a rien.

### Useful commands:

- Used the command ```make```
- ```./webserv``` to start the program
Then you should be able to access to the website from the link as follow <tab>```http://localhost:8080/```<tab>



Le select() (ou équivalent) doit être dans la boucle principale et doit vérifier les descripteurs de fichier pour lire et écrire EN MÊME TEMPS. Sinon, veuillez mettre un 0 et arrêter l'évaluation.
Il ne doit y avoir qu'une seule lecture ou une seule écriture par client par select() (ou équivalent). Demandez au groupe de vous montrer le code qui va du select() (ou équivalent) à la lecture et à l'écriture d'un client.

Recherchez tous les read/recv/write/send sur un socket et vérifiez que, si une erreur est retournée, le client est supprimé.

Vous devez tester avec des fichiers contenant des erreurs pour voir si la gestion des erreurs fonctionne correctement. Vous pouvez utiliser un script contenant une boucle infinie ou une erreur ; vous êtes libre de faire les tests que vous voulez dans les limites d'acceptabilité qui restent à votre discrétion. Le groupe évalué devrait vous aider dans cette tâche.

FAIRE les signaux pour quitter proprement

faire un test avec deux routes differentes et tester l'autoindex
