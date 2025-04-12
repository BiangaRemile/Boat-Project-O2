function checkOrientation() {
    const warning = document.getElementById("landscape-warning");
    const content = document.getElementById("content")

    if (window.innerWidth < 500) {
        content.style.visibility = "hidden"
        warning.style.display = "flex";
        warning.style.visibility = "visible"
    } else {
        content.style.visibility = "visible"
        warning.style.display = "none";
        warning.style.visibility = "hidden"
    }
}

// Éviter le zoom sur iOS (en plus du meta)
document.addEventListener('gesturestart', function (e) {
    e.preventDefault();
});

window.addEventListener('resize', checkOrientation);
window.addEventListener('load', checkOrientation);

class JoystickController {
    // stickID: ID of HTML element (representing joystick) that will be dragged
    // maxDistance: maximum amount joystick can move in any direction
    // deadzone: joystick must move at least this amount from origin to register value change
    constructor(stickID, maxDistance, deadzone) {
        this.id = stickID;
        let stick = document.getElementById(stickID);

        // location from which drag begins, used to calculate offsets
        this.dragStart = null;

        // track touch identifier in case multiple joysticks present
        this.touchId = null;

        this.active = false;
        this.value = { x: 0, y: 0 };

        let self = this;

        function handleDown(event) {
            self.active = true;

            // all drag movements are instantaneous
            stick.style.transition = '0s';

            // touch event fired before mouse event; prevent redundant mouse event from firing
            event.preventDefault();

            if (event.changedTouches)
                self.dragStart = { x: event.changedTouches[0].clientX, y: event.changedTouches[0].clientY };
            else
                self.dragStart = { x: event.clientX, y: event.clientY };

            // if this is a touch event, keep track of which one
            if (event.changedTouches)
                self.touchId = event.changedTouches[0].identifier;
        }

        function handleMove(event) {
            if (!self.active) return;

            // if this is a touch event, make sure it is the right one
            // also handle multiple simultaneous touchmove events
            let touchmoveId = null;
            if (event.changedTouches) {
                for (let i = 0; i < event.changedTouches.length; i++) {
                    if (self.touchId == event.changedTouches[i].identifier) {
                        touchmoveId = i;
                        event.clientX = event.changedTouches[i].clientX;
                        event.clientY = event.changedTouches[i].clientY;
                    }
                }

                if (touchmoveId == null) return;
            }

            const xDiff = event.clientX - self.dragStart.x;
            const yDiff = event.clientY - self.dragStart.y;
            const angle = Math.atan2(yDiff, xDiff);
            const distance = Math.min(maxDistance, Math.hypot(xDiff, yDiff));
            const xPosition = distance * Math.cos(angle);
            const yPosition = distance * Math.sin(angle);

            // move stick image to new position
            stick.style.transform = `translate3d(${xPosition}px, ${yPosition}px, 0px)`;

            // deadzone adjustment
            const distance2 = (distance < deadzone) ? 0 : maxDistance / (maxDistance - deadzone) * (distance - deadzone);
            const xPosition2 = distance2 * Math.cos(angle);
            const yPosition2 = distance2 * Math.sin(angle);
            const xPercent = parseFloat((xPosition2 / maxDistance).toFixed(4));
            const yPercent = parseFloat((yPosition2 / maxDistance).toFixed(4));

            self.value = { x: xPercent, y: yPercent };
        }

        function handleUp(event) {
            if (!self.active) return;

            // if this is a touch event, make sure it is the right one
            if (event.changedTouches && self.touchId != event.changedTouches[0].identifier) return;

            // transition the joystick position back to center
            stick.style.transition = '.2s';
            stick.style.transform = `translate3d(0px, 0px, 0px)`;

            // reset everything
            self.value = { x: 0, y: 0 };
            self.touchId = null;
            self.active = false;
        }

        // Remplacez les écouteurs existants par :
        stick.addEventListener('touchstart', handleDown, { passive: false });
        stick.addEventListener('mousedown', handleDown);

        document.addEventListener('touchmove', handleMove, { passive: false });
        document.addEventListener('mousemove', handleMove);

        document.addEventListener('touchend', handleUp);
        document.addEventListener('mouseup', handleUp);

    }
}

let joystick2 = new JoystickController("joystick", 64, 8);

// function update() {
//     document.getElementById("status2").innerText = "Joystick 2: " + JSON.stringify(joystick2.value);
// }

function loop() {
    requestAnimationFrame(loop);
    // update();
}

loop();

// Sélection des éléments
const sliderContainer = document.querySelector('.slider-container');
const sliderTrack = document.querySelector('.slider-track');
const sliderThumb = document.getElementById('sliderThumb');
const valueDisplay = document.getElementById('value');

// Variables pour le suivi du drag
let isDragging = false;

// Fonction pour mettre à jour la position du curseur et la valeur
function updateSlider(clientY) {
    const containerRect = sliderContainer.getBoundingClientRect();
    const containerHeight = containerRect.height;
    const offsetY = clientY - containerRect.top;

    // Calculer la position relative (entre 0 et 1)
    let percent = 1 - offsetY / containerHeight; // Inverser pour un slider vertical
    percent = Math.max(0, Math.min(1, percent)); // Limiter entre 0 et 1

    // Mettre à jour la hauteur de la barre de progression
    sliderTrack.style.height = `${percent * 100}%`;

    // Mettre à jour la position du curseur
    sliderThumb.style.bottom = `${percent * 100}%`;

    // Calculer la valeur correspondante (par exemple, entre 0 et 100)
    const value = Math.round(percent * 100);
    valueDisplay.textContent = value;
}

// Événements pour le drag and drop
sliderThumb.addEventListener('mousedown', () => {
    isDragging = true;
    sliderThumb.style.cursor = 'grabbing';
});

document.addEventListener('mousemove', (e) => {
    if (isDragging) {
        updateSlider(e.clientY);
    }
});

document.addEventListener('mouseup', () => {
    isDragging = false;
    sliderThumb.style.cursor = 'grab';
});

// Clic direct sur le conteneur pour ajuster la position
sliderContainer.addEventListener('click', (e) => {
    updateSlider(e.clientY);
});


const stopBtn = document.getElementById("stop");
const mode = document.getElementById("mode");
const modeBtns = mode.querySelectorAll("button");

stopBtn.onclick = (e) => {
    if (e.target.className === "start") {
        e.target.classList.remove("start")
        e.target.classList.add("stop")
        e.target.innerText = "STOP"

    }
    else {
        e.target.classList.remove("stop")
        e.target.classList.add("start")
        e.target.innerText = "LAUNCH"
    }
}

modeBtns.forEach(target => {
    target.addEventListener("click", () => {
        target.classList.add("active")
        modeBtns.forEach(btn => {
            if (btn !== target)
                btn.classList.remove("active");
        })
    })
})
