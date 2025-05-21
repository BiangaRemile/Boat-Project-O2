/* ----------------------------- */
/* 1. GLOBAL VARIABLES           */
/* ----------------------------- */

let speed = 0; // Tracks the current speed of the system.
var prevSpeed = 0; // Tracks the previous speed value for comparison.
var currentScale = 1; // Tracks the current active scale on the speedometer.

/* ----------------------------- */
/* 2. DEVICE ORIENTATION CHECK   */
/* ----------------------------- */

/**
 * Checks the device orientation and shows/hides the landscape warning accordingly.
 */
function checkOrientation() {
    const warning = document.getElementById("landscape-warning"); // Landscape warning message element
    const content = document.getElementById("content"); // Main content container

    if (window.innerWidth < 500) { // If the screen width is less than 500px (likely portrait mode)
        content.style.visibility = "hidden"; // Hide the main content
        warning.style.display = "flex"; // Show the warning message
        warning.style.visibility = "visible";
    } else { // Otherwise (landscape mode or wider screen)
        content.style.visibility = "visible"; // Show the main content
        warning.style.display = "none"; // Hide the warning message
        warning.style.visibility = "hidden";
    }
}

// Prevent zooming on iOS devices by disabling pinch gestures.
document.addEventListener('gesturestart', function (e) {
    e.preventDefault(); // Prevent default gesture behavior (pinch-to-zoom).
});

// Listen for window resize events to handle orientation changes dynamically.
window.addEventListener('resize', checkOrientation);
window.addEventListener('load', checkOrientation); // Check orientation when the page loads.

/* ----------------------------- */
/* 3. JOYSTICK CONTROLLER        */
/* ----------------------------- */

/**
 * Class representing a joystick controller with drag-and-drop functionality.
 */
class JoystickController {
    /**
     * @param stickID The ID of the HTML element representing the joystick.
     * @param maxDistance The maximum distance the joystick can move in any direction.
     * @param deadzone The minimum distance the joystick must move from its origin to register a value change.
     */
    constructor(stickID, maxDistance, deadzone) {
        this.id = stickID; // Store the joystick's ID
        let stick = document.getElementById(stickID); // Get the joystick element

        this.dragStart = null; // Tracks the starting position of the drag
        this.touchId = null; // Tracks the touch identifier for multi-touch support
        this.active = false; // Indicates whether the joystick is being dragged
        this.value = { x: 0, y: 0 }; // Stores the current joystick position values (x, y)

        let self = this; // Reference to the current instance for use in event handlers

        /**
         * Handles the start of a drag event (touch or mouse down).
         */
        function handleDown(event) {
            self.active = true; // Mark the joystick as active
            stick.style.transition = '0s'; // Disable transitions for immediate movement
            event.preventDefault(); // Prevent default browser behavior

            // Determine the starting position of the drag
            if (event.changedTouches) // For touch events
                self.dragStart = { x: event.changedTouches[0].clientX, y: event.changedTouches[0].clientY };
            else // For mouse events
                self.dragStart = { x: event.clientX, y: event.clientY };

            // Track the touch identifier for multi-touch support
            if (event.changedTouches)
                self.touchId = event.changedTouches[0].identifier;
        }

        /**
         * Handles the movement of the joystick during a drag event.
         */
        function handleMove(event) {
            if (!self.active) return; // Ignore if the joystick is not active

            let touchmoveId = null; // Track the correct touch event in multi-touch scenarios
            if (event.changedTouches) {
                for (let i = 0; i < event.changedTouches.length; i++) {
                    if (self.touchId == event.changedTouches[i].identifier) {
                        touchmoveId = i;
                        event.clientX = event.changedTouches[i].clientX;
                        event.clientY = event.changedTouches[i].clientY;
                    }
                }
                if (touchmoveId == null) return; // Ignore unrelated touch events
            }

            // Calculate the joystick's new position based on drag movement
            const xDiff = event.clientX - self.dragStart.x;
            const yDiff = event.clientY - self.dragStart.y;
            const angle = Math.atan2(yDiff, xDiff); // Angle of movement
            const distance = Math.min(maxDistance, Math.hypot(xDiff, yDiff)); // Distance from origin
            const xPosition = distance * Math.cos(angle); // X-coordinate of the joystick
            const yPosition = distance * Math.sin(angle); // Y-coordinate of the joystick

            // Move the joystick visually
            stick.style.transform = `translate3d(${xPosition}px, ${yPosition}px, 0px)`;

            // Adjust for deadzone (ignore small movements near the center)
            const distance2 = (distance < deadzone) ? 0 : maxDistance / (maxDistance - deadzone) * (distance - deadzone);
            const xPosition2 = distance2 * Math.cos(angle);
            const yPosition2 = distance2 * Math.sin(angle);
            const xPercent = parseFloat((xPosition2 / maxDistance).toFixed(4));
            const yPercent = parseFloat((yPosition2 / maxDistance).toFixed(4));

            self.value = { x: xPercent, y: yPercent }; // Update the joystick's value
        }

        /**
         * Handles the end of a drag event (touch or mouse up).
         */
        function handleUp(event) {
            if (!self.active) return; // Ignore if the joystick is not active

            // Ensure the correct touch event is being handled
            if (event.changedTouches && self.touchId != event.changedTouches[0].identifier) return;

            // Reset the joystick's position and state
            stick.style.transition = '.2s'; // Enable smooth transition back to center
            stick.style.transform = `translate3d(0px, 0px, 0px)`; // Move joystick back to center
            self.value = { x: 0, y: 0 }; // Reset joystick values
            self.touchId = null; // Clear touch identifier
            self.active = false; // Mark joystick as inactive
        }

        // Add event listeners for touch and mouse interactions
        stick.addEventListener('touchstart', handleDown, { passive: false });
        stick.addEventListener('mousedown', handleDown);

        document.addEventListener('touchmove', handleMove, { passive: false });
        document.addEventListener('mousemove', handleMove);

        document.addEventListener('touchend', handleUp);
        document.addEventListener('mouseup', handleUp);
    }
}

// Create a new joystick controller instance
let joystick = new JoystickController("joystick", 64, 8);

/* ----------------------------- */
/* 4. SPEED AND ANGLE LOGIC      */
/* ----------------------------- */

/**
 * Converts joystick values into angle and speed adjustments.
 * @param value The current joystick position values (x, y).
 * @returns An object containing the calculated angle and speed.
 */
function toAngleSpeed(value) {
    let angleSpeed = { angle: 0, speed: 0 }; // Initialize angle and speed

    // Calculate the angle based on the x-axis value
    if (value.x > 0) {
        if (value.x < 0.2) angleSpeed.angle = 0;
        else if (value.x < 0.6) angleSpeed.angle = 5;
        else angleSpeed.angle = 10;
    } else if (value.x < 0) {
        if (value.x > -0.2) angleSpeed.angle = 0;
        else if (value.x > -0.5) angleSpeed.angle = -5;
        else angleSpeed.angle = -10;
    } else {
        angleSpeed.angle = 0;
    }

    // Calculate the speed adjustment based on the y-axis value
    if (value.y < 0) {
        if (value.y > -0.2) angleSpeed.speed = 0;
        else if (value.y > -0.6) {
            angleSpeed.speed = 0.15;
            increaseSpeed(10); // Increase speed by 10 units
        } else {
            angleSpeed.speed = 0.15;
            increaseSpeed(20); // Increase speed by 20 units
        }
    } else if (value.y > 0) {
        if (value.y < 0.2) angleSpeed.speed = 0;
        else if (value.y < 0.6) {
            angleSpeed.speed = -0.15;
            decreaseSpeed(10); // Decrease speed by 10 units
        } else {
            angleSpeed.speed = -0.3;
            decreaseSpeed(20); // Decrease speed by 20 units
        }
    } else {
        angleSpeed.speed = 0;
    }

    return angleSpeed; // Return the calculated angle and speed
}

/* ----------------------------- */
/* 5. SPEEDOMETER UPDATES        */
/* ----------------------------- */

/**
 * Increases the speed and updates the speedometer display.
 * @param value The amount by which to increase the speed.
 */
function increaseSpeed(value) {
    speed += value; // Increase the speed
    if (speed > 100) speed = 100; // Cap the speed at 100

    if (currentScale < 11) {
        currentScale += 1; // Move to the next scale
        changeActive(); // Update the active scale
    }

    addClass(); // Update the arrow position
    speedDisplay(speed); // Update the displayed speed
}

/**
 * Decreases the speed and updates the speedometer display.
 * @param value The amount by which to decrease the speed.
 */
function decreaseSpeed(value) {
    speed -= value; // Decrease the speed
    if (currentScale > 1) {
        currentScale -= 1; // Move to the previous scale
        changeActive(); // Update the active scale
    }
    if (speed < 0) speed = 0; // Ensure the speed doesn't go below 0

    addClass(); // Update the arrow position
    speedDisplay(speed); // Update the displayed speed
}

/**
 * Updates the arrow position on the speedometer.
 */
function addClass() {
    let newClass = "speed-" + speed; // New class based on the current speed
    let prevClass = "speed-" + prevSpeed; // Previous class based on the previous speed
    let el = document.getElementsByClassName("arrow-wrapper")[0]; // Arrow wrapper element

    if (el.classList.contains(prevClass) && !el.classList.contains(newClass)) {
        el.classList.remove(prevClass); // Remove the old class
        el.classList.add(newClass); // Add the new class
    }
    prevSpeed = speed; // Update the previous speed
}

/**
 * Changes the active scale on the speedometer.
 */
function changeActive() {
    let tempClass = "speedometer-scale-" + currentScale; // Class for the current scale
    let el = document.getElementsByClassName(tempClass)[0]; // Scale element
    el.classList.toggle("active"); // Toggle the "active" class
}

/**
 * Updates the displayed speed value.
 * @param value The current speed value.
 */
function speedDisplay(value) {
    let el = document.getElementById("speed-value"); // Speed value element
    el.innerText = (0.15 * value / 10).toFixed(2); // Calculate and display the speed in m/s
}

/* ----------------------------- */
/* 6. BUTTON HANDLERS            */
/* ----------------------------- */

// Handle the LAUNCH/STOP button functionality
const stopBtn = document.getElementById("stop");
stopBtn.onclick = (e) => {
    e.target.classList.toggle("start"); // Toggle between "start" and "stop" classes
    e.target.classList.toggle("stop");
    e.target.innerText = e.target.className === "start" ? "LAUNCH" : "STOP"; // Update button text

    // Send a WebSocket message to toggle the LED state
    let message = JSON.stringify({ component: "led", state: e.target.className === "start" ? 0 : 1 });
    sendMessage(message);
};

// Handle mode selection buttons (Forward/Backward)
const mode = document.getElementById("mode");
const modeBtns = mode.querySelectorAll("button");

modeBtns.forEach(target => {
    target.addEventListener("click", () => {
        target.classList.add("active"); // Mark the clicked button as active
        modeBtns.forEach(btn => {
            if (btn !== target) btn.classList.remove("active"); // Deactivate other buttons
        });
    });
});

/* ----------------------------- */
/* 7. MAIN LOOP                  */
/* ----------------------------- */

/**
 * Continuously updates the joystick's state and processes its values.
 */
function loop() {
    requestAnimationFrame(loop); // Continuously call the loop function
    toAngleSpeed(joystick.value); // Process the joystick's current values

    if (joystick.value.x !== 0) {
        let message = JSON.stringify({ component: "servo", state: joystick.value.x * 10 }); // Create a message with the joystick angle
        sendMessage(message); // Send the message via WebSocket
    }
}

loop(); // Start the loop