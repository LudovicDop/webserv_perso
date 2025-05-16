<?php
$name = isset($_REQUEST['name']) ? htmlspecialchars($_REQUEST['name']) : 'Guest';
$email = isset($_REQUEST['email']) ? htmlspecialchars($_REQUEST['email']) : 'Not provided';
?>
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Hello Page</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            font-family: 'Segoe UI', Tahoma, sans-serif;
        }

        body {
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
            font-size: 2.5rem;
            margin-bottom: 20px;
        }

        .card p {
            font-size: 1.2rem;
            margin-bottom: 30px;
        }

        .home-button {
            display: inline-block;
            padding: 12px 25px;
            background: linear-gradient(135deg, #00d4ff, #0088ff);
            color: white;
            text-decoration: none;
            font-weight: bold;
            border-radius: 30px;
            transition: all 0.3s ease;
            border: none;
            font-size: 1rem;
            box-shadow: 0 0 20px rgba(0, 212, 255, 0.5);
        }

        .home-button:hover {
            transform: scale(1.05);
            box-shadow: 0 0 30px rgba(0, 212, 255, 0.7);
        }
    </style>
</head>
<body>

<canvas id="particleCanvas"></canvas>

<div class="card">
    <h1>Hello, <?= $name ?>!</h1>
    <p>Your email is: <?= $email ?></p>
    <a href="index.html" class="home-button">← Retour à l'accueil</a>
</div>

<!-- Animation des particules -->
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
