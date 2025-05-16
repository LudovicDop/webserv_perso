#!/bin/bash
##########################################################################
# Script de test multi-clients pour un serveur web HTTP/1.1
#
# Ce script permet de tester simultanément plusieurs clients.
# Pour chaque client, vous pouvez choisir de tester indépendamment les
# méthodes GET, POST et DELETE.
#
# Pour chaque requête, le script enregistre dans le fichier de log :
#   - La méthode testée (GET, POST, DELETE)
#   - L'identifiant du client
#   - Le code HTTP retourné
#   - La version HTTP
#
# À la fin, un résumé affiche pour chaque méthode le nombre total
# de tests, les succès (code commençant par 2) et les échecs.
##########################################################################

# ----- Paramètres par défaut
DEFAULT_URL="http://localhost:8080"        # URL par défaut du serveur à tester
DEFAULT_NUM_CLIENTS=100                    # Nombre de clients simultanés par itération
DEFAULT_DURATION=30                        # Durée totale du test en secondes

# ----- Saisie des paramètres par l'utilisateur
read -p "Entrez l'URL de votre serveur (défaut: $DEFAULT_URL): " URL
URL=${URL:-$DEFAULT_URL}

read -p "Entrez le nombre de clients simultanés (défaut: $DEFAULT_NUM_CLIENTS): " num_clients
num_clients=${num_clients:-$DEFAULT_NUM_CLIENTS}

read -p "Entrez la durée du test en secondes (défaut: $DEFAULT_DURATION): " DURATION
DURATION=${DURATION:-$DEFAULT_DURATION}

# Choix des méthodes à tester
read -p "Tester la méthode GET ? (O/n): " choix_get
choix_get=${choix_get:-O}
read -p "Tester la méthode POST ? (O/n): " choix_post
choix_post=${choix_post:-O}
read -p "Tester la méthode DELETE ? (O/n): " choix_delete
choix_delete=${choix_delete:-O}

echo "Test sur $URL avec $num_clients clients simultanés pendant $DURATION secondes."
echo "Méthodes sélectionnées :"
[ "$choix_get"  = "O" ] && echo " - GET"
[ "$choix_post" = "O" ] && echo " - POST"
[ "$choix_delete" = "O" ] && echo " - DELETE"
echo ""

# ----- Fichier de log pour enregistrer les résultats
LOGFILE="results.log"
> "$LOGFILE"  # On vide le fichier log

# Récupère le timestamp de départ
start_time=$(date +%s)

##########################################################################
# Fonction test_client
#
# Pour un client donné, effectue les requêtes selon les méthodes choisies.
# Pour chaque requête, enregistre dans le log :
#   "METHOD client_id code version"
##########################################################################
test_client() {
    client_id=$1

    if [ "$choix_get" = "O" ]; then
        result_get=$(curl -s -w " %{http_code} %{http_version}" -o /dev/null "$URL")
        code_get=$(echo $result_get | awk '{print $(NF-1)}')
        version_get=$(echo $result_get | awk '{print $NF}')
        echo "GET $client_id $code_get $version_get" >> "$LOGFILE"
    fi

    if [ "$choix_post" = "O" ]; then
        # Envoi d'un payload simple pour le POST
        result_post=$(curl -s -X POST -d "data=test" -w " %{http_code} %{http_version}" -o /dev/null "$URL")
        code_post=$(echo $result_post | awk '{print $(NF-1)}')
        version_post=$(echo $result_post | awk '{print $NF}')
        echo "POST $client_id $code_post $version_post" >> "$LOGFILE"
    fi

    if [ "$choix_delete" = "O" ]; then
        result_delete=$(curl -s -X DELETE -w " %{http_code} %{http_version}" -o /dev/null "$URL")
        code_delete=$(echo $result_delete | awk '{print $(NF-1)}')
        version_delete=$(echo $result_delete | awk '{print $NF}')
        echo "DELETE $client_id $code_delete $version_delete" >> "$LOGFILE"
    fi
}

##########################################################################
# Boucle principale
#
# Tant que la durée de test n'est pas écoulée, lance en parallèle le nombre
# spécifié de clients à chaque itération.
##########################################################################
while [ $(($(date +%s) - start_time)) -lt $DURATION ]; do
    for (( i=1; i<=num_clients; i++ )); do
        test_client "$i" &
    done
    wait
done

echo "Test terminé. Génération du résumé ..."

##########################################################################
# Résumé des résultats depuis le fichier log
##########################################################################
function summary_method() {
    method=$1
    total=$(grep -c "^$method" "$LOGFILE")
    success=$(grep "^$method" "$LOGFILE" | awk '{if(substr($3,1,1)=="2") count++} END {print count+0}')
    failure=$(grep "^$method" "$LOGFILE" | awk '{if(substr($3,1,1)!="2") count++} END {print count+0}')
    echo "Méthode $method:"
    echo "  Total   : $total requête(s)"
    echo "  Succès  : $success (code commençant par 2)"
    echo "  Échecs  : $failure"
    echo ""
}

# Afficher le résumé pour chaque méthode sélectionnée
echo "--------------------------------------------------"
echo "Résumé des tests sur $URL"
echo "--------------------------------------------------"
[ "$choix_get" = "O" ] && summary_method "GET"
[ "$choix_post" = "O" ] && summary_method "POST"
[ "$choix_delete" = "O" ] && summary_method "DELETE"
echo "--------------------------------------------------"
echo "Fin du test."
