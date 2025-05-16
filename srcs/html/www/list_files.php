<?php
// Définir l'en-tête pour renvoyer du JSON
header('Content-Type: application/json');

// Définir le répertoire à explorer
$dir = "uploads/";
$files = [];

// Vérifier si le répertoire existe
if (is_dir($dir)) {
    // Ouvrir le répertoire
    if ($dh = opendir($dir)) {
        // Lire chaque entrée du répertoire
        while (($file = readdir($dh)) !== false) {
            // Ignorer . et .. et les fichiers cachés
            if ($file != "." && $file != ".." && substr($file, 0, 1) != ".") {
                $files[] = $file;
            }
        }
        closedir($dh);
    }
}

// Renvoyer la liste des fichiers au format JSON
echo json_encode($files);
?>
