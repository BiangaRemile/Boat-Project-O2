// WebSocket URL for communication with the server.
// Uncomment the static IP address line if you want to connect to a specific server.
// let url = "ws://192.168.17.29/ws";
let url = "ws://" + window.location.host + "/ws"; // Dynamically use the current host's address.
let socket; // Variable to hold the WebSocket connection.

/**
 * Establishes a WebSocket connection to the server at the specified URL.
 * @param url The WebSocket server URL to connect to.
 */
function wsConnection(url) {
    socket = new WebSocket(url); // Create a new WebSocket connection.

    // Event listener for when the connection is successfully opened.
    socket.addEventListener('open', function (event) {
        console.log("Connected to server"); // Log a message indicating successful connection.
    });

    // Event listener for receiving messages from the server.
    socket.addEventListener('message', function (event) {
        const data = JSON.parse(event.data); // Parse the incoming JSON message.
        const state = data.state; // Extract the "state" field from the message.
        const component = data.component; // Extract the "component" field from the message.

        // Update the LAUNCH/STOP button based on the received state and component.
        if (state && component === "led") {
            stopBtn.classList.remove("start"); // Remove the "start" class.
            stopBtn.classList.add("stop"); // Add the "stop" class.
        } else {
            stopBtn.classList.remove("stop"); // Remove the "stop" class.
            stopBtn.classList.add("start"); // Add the "start" class.
        }
        stopBtn.innerText = state ? "STOP" : "LAUNCH"; // Update the button text accordingly.
    });

    // Event listener for when the connection is closed.
    socket.addEventListener('close', function (event) {
        console.log("Disconnected from server"); // Log a message indicating disconnection.

        // Attempt to reconnect after 2 seconds.
        setTimeout(() => { wsConnection(url); }, 2000);
    });

    // Event listener for WebSocket errors.
    socket.addEventListener("error", (event) => {
        console.log("WebSocket error: ", event.data); // Log any WebSocket errors.
    });
}

/**
 * Sends a message over the WebSocket connection.
 * @param message The message to send (expected to be a JSON string).
 */
function sendMessage(message) {
    if (socket.readyState === WebSocket.OPEN) { // Check if the WebSocket connection is open.
        socket.send(message); // Send the message to the server.
    } else {
        console.error("WebSocket is not open. Ready state: ", socket.readyState); // Log an error if the connection is not open.
    }
}

// Event listener for when the page finishes loading.
window.addEventListener("load", () => {
    wsConnection(url); // Establish the WebSocket connection when the page loads.
});