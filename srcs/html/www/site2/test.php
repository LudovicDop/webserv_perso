<?php
// Activer les erreurs
ini_set('display_errors', 1);
error_reporting(E_ALL);

$success = false;
$fileName = "";

if (isset($_FILES["file"])) {
    $targetDir = "uploads/";
    $fileName = basename($_FILES["file"]["name"]);
    $targetFile = $targetDir . $fileName;

    if (!file_exists($targetDir)) {
        mkdir($targetDir, 0777, true);
    }

    if (move_uploaded_file($_FILES["file"]["tmp_name"], $targetFile)) {
        $success = true;
    }
}

// Debug
file_put_contents("/tmp/cgi_debug_files.txt", print_r($_FILES, true));
?>
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Upload <?= $success ? 'Réussi' : 'Échoué' ?></title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            margin: 0;
            font-family: 'Segoe UI', sans-serif;
            background-color: #0d0d1a;
            color: white;
            overflow: hidden;
        }

        canvas {
            position: fixed;
            top: 0;
            left: 0;
            z-index: 0;
        }

        .card {
            position: relative;
            z-index: 1;
            text-align: center;
            background-color: rgba(255, 255, 255, 0.05);
            border-radius: 15px;
            padding: 40px;
            max-width: 500px;
            margin: 100px auto;
            box-shadow: 0 0 30px rgba(0, 212, 255, 0.2);
            backdrop-filter: blur(10px);
        }

        .card h1 {
            font-size: 2rem;
            margin-bottom: 16px;
        }

        .card p {
            font-size: 1.1rem;
            margin-bottom: 24px;
        }

        .button-link {
            display: inline-block;
            padding: 12px 25px;
            background: linear-gradient(135deg, #00d4ff, #0088ff);
            color: white;
            text-decoration: none;
            font-weight: bold;
            border-radius: 30px;
            transition: all 0.3s ease;
        }

        .button-link:hover {
            background: linear-gradient(135deg, #00bcd4, #0066cc);
        }
    </style>
</head>
<body>

<canvas id="particleCanvas"></canvas>

<div class="card">
    <?php if ($success): ?>
        <h1>✅ Fichier Téléversé</h1>
        <p>Le fichier <strong><?= htmlspecialchars($fileName) ?></strong> a été envoyé avec succès.</p>
    <?php else: ?>
        <h1>❌ Échec du téléversement</h1>
        <p>Une erreur est survenue. Veuillez réessayer.</p>
    <?php endif; ?>
    <a href="index.html" class="button-link">← Retour à l'accueil</a>
</div>

<script>
    const canvas = document.getElementById("particleCanvas");
    const ctx = canvas.getContext("2d");
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;

    let particles = [];

    class Particle {
        constructor() {
            this.radius = Math.random() * 3 + 2;
            this.x = Math.random() * canvas.width;
            this.y = Math.random() * canvas.height;
            this.dx = (Math.random() - 0.5) * 1.2;
            this.dy = (Math.random() - 0.5) * 1.2;
            this.color = `rgba(0, 212, 255, ${Math.random() * 0.6 + 0.3})`;
        }

        draw() {
            ctx.beginPath();
            ctx.arc(this.x, this.y, this.radius, 0, Math.PI * 2);
            ctx.fillStyle = this.color;
            ctx.fill();
        }

        update() {
            this.x += this.dx;
            this.y += this.dy;

            if (this.x + this.radius > canvas.width || this.x - this.radius < 0) this.dx *= -1;
            if (this.y + this.radius > canvas.height || this.y - this.radius < 0) this.dy *= -1;

            this.draw();
        }
    }

    function initParticles() {
        particles = [];
        for (let i = 0; i < 60; i++) {
            particles.push(new Particle());
        }
    }

    function animateParticles() {
        ctx.clearRect(0, 0, canvas.width, canvas.height);
        particles.forEach(p => p.update());
        requestAnimationFrame(animateParticles);
    }

    window.addEventListener('resize', () => {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
        initParticles();
    });

    initParticles();
    animateParticles();
</script>

</body>
</html>
