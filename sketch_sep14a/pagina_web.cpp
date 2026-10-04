#include <WiFi.h>
#include <WebServer.h>
#include "fsm.h"

void enviarDatos();
void respuesta();
const char paginaHTML[] PROGMEM = R"====(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Multímetro Automático</title>
  <!-- Acá está el link mágico de Pico.css que hace que se vea bien -->
  <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/@picocss/pico@2/css/pico.min.css">
</head>
<body>
  <main class="container">
    <h1>Multímetro de Rango Automático</h1>
    
    <article>
      <header>Lectura en tiempo real</header>
      <!-- Estos IDs son los que el Javascript va a buscar para actualizar el texto -->
      <h2 id="valorCorriente" style="font-size: 3rem; color: #017fc0;">-- A</h2>
      <p>Sensor Activo: <strong id="nombreSensor">Cargando...</strong></p>
    </article>

  </main>

  <script>
    // Este script le pregunta al ESP32 por los datos cada 1 segundo (1000 ms)
    setInterval(function() {
      fetch('/datos')
        .then(respuesta => respuesta.json())
        .then(datos => {
          document.getElementById('valorCorriente').innerText = datos.corriente + " A";
          document.getElementById('nombreSensor').innerText = datos.sensor;
        });
    }, 1000);
  </script>
</body>
</html>
)====";

//variables globales

WebServer servidor(80);
static MensajeWeb ultimo_mensaje;
const char* ssid = "Fabian";
const char* key = "18642084";
QueueHandle_t cola_web_local;

void task_web(void *pvParameters){
 
  ParametroFsm* params = (ParametroFsm*) pvParameters;
  cola_web_local = params->cola_web;
  
  WiFi.begin(ssid,key); 
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");  
    vTaskDelay(pdMS_TO_TICKS(500));
  }

  Serial.println();
  Serial.print("Conectado!! IP LOCAL --> ");
  Serial.print(WiFi.localIP());

  servidor.on("/",respuesta);
  servidor.on("/datos", enviarDatos);
  servidor.begin();

  for(;;){

    while(xQueueReceive(cola_web_local, &ultimo_mensaje, 0) == pdTRUE) {} //consume todos los msj que haya 
                                                                          // en la cola para siempre tener el ultimo
    servidor.handleClient();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}


void respuesta(){
    servidor.send(200,"text/html", paginaHTML);
}

void enviarDatos(){

  String nombre_sensor = "";
  switch(ultimo_mensaje.estado){

    case ACS712_30: nombre_sensor = "ACS712 (30A)";
                    break;
    case ACS712_20: nombre_sensor = "ACS712 (20A)";
                    break;
    case ACS712_5:  nombre_sensor = "ACS712 (5A)";
                    break;
    case INA219:   nombre_sensor = "INA219 (3,2A)";
                    break;
    default:        nombre_sensor = "Desconocido"; 
                    break;

  }
 String json = "{\"sensor\": \"" + nombre_sensor + "\", \"corriente\": " + String(ultimo_mensaje.lectura) + "}";
  servidor.send(200, "application/json", json);

}