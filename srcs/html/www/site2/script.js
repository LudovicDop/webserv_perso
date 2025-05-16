// Attendre que le DOM soit complètement chargé
document.addEventListener('DOMContentLoaded', function() {
    // Animation du cercle lumineux
    animateLuminousCircle();
    
    // Animation des éléments au scroll
    initScrollAnimation();
    
    // Effet de survol pour les cartes
    initCardHoverEffects();
    
    // Animation du bouton pulse
    initPulseButton();
});

// Animation du cercle lumineux
function animateLuminousCircle() {
    const circle = document.querySelector('.luminous-circle');
    
    // Animation de mouvement subtil
    document.addEventListener('mousemove', function(e) {
        const mouseX = e.clientX / window.innerWidth;
        const mouseY = e.clientY / window.innerHeight;
        
        // Déplacement léger du cercle en fonction de la position de la souris
        gsap.to(circle, {
            x: (mouseX - 0.5) * 1000,
            y: (mouseY - 0.5) * 1000,
            duration: 2,
            ease: "power2.out"
        });
    });
    
    // Animation de pulsation
    const pulseTimeline = gsap.timeline({repeat: -1, yoyo: true});
    pulseTimeline.to(circle, {
        opacity: 0.9,
        scale: 1.05,
        filter: 'blur(40px)',
        duration: 4,
        ease: "sine.inOut"
    });
    pulseTimeline.to(circle, {
        opacity: 0.7,
        scale: 1,
        filter: 'blur(50px)',
        duration: 4,
        ease: "sine.inOut"
    });
}

// Animation des éléments au scroll
function initScrollAnimation() {
    // Sélectionner tous les éléments à animer
    const elementsToAnimate = document.querySelectorAll('.course-card, .category-card, .testimonial-card, .hero-content, section h2');
    
    // Ajouter la classe fade-in à tous les éléments
    elementsToAnimate.forEach(element => {
        element.classList.add('fade-in');
    });
    
    // Fonction pour vérifier si un élément est visible dans la fenêtre
    function checkVisibility() {
        elementsToAnimate.forEach(element => {
            const elementTop = element.getBoundingClientRect().top;
            const elementVisible = 150;
            
            if (elementTop < window.innerHeight - elementVisible) {
                element.classList.add('visible');
            }
        });
    }
    
    // Vérifier la visibilité au chargement et au scroll
    window.addEventListener('scroll', checkVisibility);
    checkVisibility();
}

// Animation du bouton pulse
function initPulseButton() {
    const pulseButtons = document.querySelectorAll('.cta-button.pulse');
    
    pulseButtons.forEach(button => {
        // Animation de pulsation
        const pulseTimeline = gsap.timeline({repeat: -1, yoyo: true});
        pulseTimeline.to(button, {
            boxShadow: '0 0 25px rgba(139, 10, 154, 0.8)',
            scale: 1.03,
            duration: 1.5,
            ease: "sine.inOut"
        });
        pulseTimeline.to(button, {
            boxShadow: '0 0 15px rgba(139, 10, 154, 0.5)',
            scale: 1,
            duration: 1.5,
            ease: "sine.inOut"
        });
        
        // Effet au survol
        button.addEventListener('mouseenter', function() {
            gsap.to(this, {
                y: -3,
                boxShadow: '0 0 30px rgba(139, 10, 154, 0.9)',
                duration: 0.3
            });
        });
        
        button.addEventListener('mouseleave', function() {
            gsap.to(this, {
                y: 0,
                boxShadow: '0 0 20px rgba(139, 10, 154, 0.7)',
                duration: 0.3
            });
        });
    });
}

// Effet de parallaxe pour le cercle lumineux au scroll
window.addEventListener('scroll', function() {
    const scrollPosition = window.scrollY;
    const circle = document.querySelector('.luminous-circle');
    
    if (circle) {
        // Déplacement vertical du cercle en fonction du scroll
        gsap.to(circle, {
            y: scrollPosition * 0.1,
            duration: 0.5,
            ease: "power1.out"
        });
    }
});

// Initialisation de GSAP (fallback si la bibliothèque n'est pas chargée)
if (typeof gsap === 'undefined') {
    console.log("GSAP n'est pas chargé, utilisation de fallbacks pour les animations");
    
    // Fonction de fallback pour les animations
    window.gsap = {
        to: function(element, options) {
            if (typeof element === 'string') {
                element = document.querySelector(element);
            }
            
            if (!element) return;
            
            // Animation de base avec CSS transitions
            const duration = (options.duration || 1) + 's';
            
            if (Array.isArray(element)) {
                element.forEach(el => applyStyles(el));
            } else {
                applyStyles(element);
            }
            
            function applyStyles(el) {
                el.style.transition = 'all ' + duration + ' ease';
                
                if (options.y !== undefined) el.style.transform = 'translateY(' + options.y + 'px)';
                if (options.x !== undefined) el.style.transform = 'translateX(' + options.x + 'px)';
                if (options.scale !== undefined) el.style.transform = 'scale(' + options.scale + ')';
                if (options.opacity !== undefined) el.style.opacity = options.opacity;
                if (options.boxShadow !== undefined) el.style.boxShadow = options.boxShadow;
                if (options.borderColor !== undefined) el.style.borderColor = options.borderColor;
                if (options.filter !== undefined) el.style.filter = options.filter;
            }
        },
        timeline: function(options) {
            return {
                to: function() { return this; },
                from: function() { return this; }
            };
        }
    };
}

// Ajout de la bibliothèque GSAP dynamiquement
function loadGSAP() {
    const script = document.createElement('script');
    script.src = 'https://cdnjs.cloudflare.com/ajax/libs/gsap/3.11.4/gsap.min.js';
    script.async = true;
    document.head.appendChild(script);
    
    script.onload = function() {
        console.log('GSAP chargé avec succès');
        // Réinitialiser les animations
        animateLuminousCircle();
        initCardHoverEffects();
        initPulseButton();
    };
}

// Charger GSAP
loadGSAP();
