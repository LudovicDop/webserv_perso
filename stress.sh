#!/bin/bash
# filepath: /home/hclaude/Documents/webserv/nuclear_stress.sh

# Configuration - valeurs extrêmes pour un stress maximal
NUM_PROCESSES=500        # Nombre massif de processus parallèles
DURATION=30             # Test plus long
URL="http://localhost:8080/" # URL à tester
METHODS=("GET" "POST" "DELETE") # Différentes méthodes HTTP
FLOOD_INTERVAL=0.001     # Intervalle minimal entre requêtes (millisecondes)
LARGE_POST_SIZE=10485760 # Taille des données POST (10MB)

echo "⚠️ AVERTISSEMENT: Ce test est conçu pour pousser votre serveur à ses limites extrêmes ⚠️"
echo "Lancement de l'attaque dans 3 secondes..."
sleep 3

echo "🔥 DÉBUT DE L'ATTAQUE NUCLÉAIRE 🔥"
echo "Processus: $NUM_PROCESSES"
echo "URL: $URL"
echo "Durée: $DURATION secondes"
echo "Démarrage à: $(date)"
echo "-----------------------------------------"

# Génère des données aléatoires pour les requêtes POST
generate_random_data() {
    local size=$1
    head -c $size /dev/urandom | base64
}

# Crée un fichier temporaire avec des données volumineuses
LARGE_DATA_FILE=$(mktemp)
generate_random_data $LARGE_POST_SIZE > $LARGE_DATA_FILE

# Fonction pour bombarder avec différentes méthodes HTTP
attack() {
    local process_id=$1
    local start_time=$(date +%s)
    local end_time=$((start_time + DURATION))
    local requests=0
    local errors=0

    echo "🚀 Processus #$process_id lancé"

    while [ $(date +%s) -lt $end_time ]; do
        # Choisir une méthode aléatoire
        METHOD=${METHODS[$((RANDOM % ${#METHODS[@]}))]}

        # Différentes options selon la méthode
        case $METHOD in
            GET)
                # Requête GET avec paramètres aléatoires pour éviter le cache
                RANDOM_PARAM=$RANDOM
                if curl -s -X GET -o /dev/null -w "%{http_code}" "$URL?cache_buster=$RANDOM_PARAM&process=$process_id" --max-time 2 > /dev/null 2>&1; then
                    requests=$((requests + 1))
                else
                    errors=$((errors + 1))
                fi
                ;;
            POST)
                # Requête POST avec de grosses données
                if curl -s -X POST -o /dev/null -w "%{http_code}" -d @$LARGE_DATA_FILE "$URL" --max-time 2 > /dev/null 2>&1; then
                    requests=$((requests + 1))
                else
                    errors=$((errors + 1))
                fi
                ;;
            DELETE)
                # Requête DELETE
                if curl -s -X DELETE -o /dev/null -w "%{http_code}" "$URL?id=$RANDOM" --max-time 2 > /dev/null 2>&1; then
                    requests=$((requests + 1))
                else
                    errors=$((errors + 1))
                fi
                ;;
        esac

        # Interval minimal pour maximiser le débit
        sleep $FLOOD_INTERVAL
    done

    echo "Processus #$process_id terminé: $requests requêtes, $errors erreurs"
    echo "$requests $errors" > "/tmp/nuclear_result_$process_id"
}

# Fonction pour arrêter en cas d'interruption
cleanup() {
    echo "🛑 Arrêt de l'attaque..."
    killall curl 2>/dev/null
    rm -f $LARGE_DATA_FILE
    exit 1
}

# Capture de l'interruption CTRL+C
trap cleanup SIGINT

# Lancer l'attaque avec plusieurs processus en parallèle
for i in $(seq 1 $NUM_PROCESSES); do
    attack $i &

    # Lancer les processus par vagues pour maximiser l'impact
    if [ $((i % 50)) -eq 0 ]; then
        echo "Lancement de la vague $((i/50))..."
        sleep 0.5
    fi
done

echo "Toutes les attaques sont lancées. Bonne chance au serveur..."

# Attendre que tous les processus se terminent
wait

# Collecter les résultats
total_requests=0
total_errors=0
for i in $(seq 1 $NUM_PROCESSES); do
    if [ -f "/tmp/nuclear_result_$i" ]; then
        read requests errors < "/tmp/nuclear_result_$i"
        total_requests=$((total_requests + requests))
        total_errors=$((total_errors + errors))
        rm "/tmp/nuclear_result_$i"
    fi
done

# Nettoyer
rm -f $LARGE_DATA_FILE

# Afficher les statistiques
echo "-----------------------------------------"
echo "🏁 Test terminé à: $(date)"
echo "Total des requêtes: $total_requests"
echo "Total des erreurs: $total_errors"
echo "Requêtes par seconde: $((total_requests / DURATION))"
echo "Taux d'erreur: $(( (total_errors * 100) / (total_requests + total_errors) ))%"
echo "-----------------------------------------"
echo "Si votre serveur a survécu, félicitations !"