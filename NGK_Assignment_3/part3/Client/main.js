// WebSocket connection setup
const socket = new WebSocket('ws://localhost:8080/chat');

socket.onopen = event => console.log('WebSocket connected:', event);
socket.onmessage = event => console.log('Message from server:', event.data);

// Axios configuration for HTTP requests
const axiosConfig = {
    headers: { 'Content-Type': 'application/json' }
};

// Function to send HTTP requests
function sendRequest(method, url, data = null) {
    axios({ method, url, data, ...axiosConfig })
        .then(response => {
            console.log(`${method.toUpperCase()} response:`, response.data);
            if (method !== 'get') getWeatherData();
            else displayWeatherData(response.data);
        })
        .catch(error => console.error('Error:', error));
}

// Function to handle GET request
function getWeatherData() {
    sendRequest('get', 'http://localhost:8080/');
}

// Function to handle POST request
function createWeatherData() {
    const data = getInputData();
    sendRequest('post', 'http://localhost:8080/', data);
}

// Function to handle PUT request
function updateWeatherData() {
    var dataId = document.getElementById('id').valueAsNumber;
    const data = getInputData();
    console.log(`http://localhost:8080/`+ dataId);
    sendRequest('put', `http://localhost:8080/`+ dataId, data);
}

// Function to get input data from form
function getInputData() {
    return {
        id: parseInt(document.getElementById('id').value),
        date: document.getElementById('date').value,
        time: document.getElementById('time').value,
        place: {
            placeName: document.getElementById('placeName').value,
            latitude: parseFloat(document.getElementById('lat').value),
            longitude: parseFloat(document.getElementById('long').value)
        },
        temperature: parseFloat(document.getElementById('temperature').value),
        humidity: parseInt(document.getElementById('humidity').value)
    };
}

function getLatestWeatherData() {
    sendRequest('get', 'http://localhost:8080/limit/3');
}

function getWeatherByDate() {
    const date = document.getElementById('dateInput').value;
    sendRequest('get', `http://localhost:8080/date/${date}`);

}

function clearTable() {
    const tableBody = document.getElementById('tableBody');
    tableBody.innerHTML = '';
}



// Function to display weather data
function displayWeatherData(data) {
    const tableBody = document.getElementById('tableBody');
    tableBody.innerHTML = ''; // Clear the table

    // Iterate through each weather data object
    data.forEach(item => {
        const row = tableBody.insertRow();
        row.insertCell().textContent = item.id;
        row.insertCell().textContent = item.date;
        row.insertCell().textContent = item.time;
        row.insertCell().textContent = item.place.placeName;
        row.insertCell().textContent = item.place.latitude;
        row.insertCell().textContent = item.place.longitude;
        row.insertCell().textContent = item.temperature;
        row.insertCell().textContent = item.humidity;
    });
}

// Attach event listeners to buttons
document.getElementById('getButton').addEventListener('click', getWeatherData);
document.getElementById('postButton').addEventListener('click', createWeatherData);
document.getElementById('putButton').addEventListener('click', updateWeatherData);
document.getElementById('getLatestButton').addEventListener('click', getLatestWeatherData);
document.getElementById('getDateButton').addEventListener('click', getWeatherByDate);
document.getElementById('clearTableButton').addEventListener('click', clearTable);


