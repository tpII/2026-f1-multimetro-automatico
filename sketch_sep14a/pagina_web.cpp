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
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Multímetro Automático ESP32</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f6f9;
            color: #333;
            margin: 0;
            padding: 20px;
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
        }
        .contenedor-principal {
            background-color: #ffffff;
            width: 100%;
            max-width: 600px;
            border-radius: 12px;
            box-shadow: 0 4px 15px rgba(0,0,0,0.1);
            overflow: hidden;
            display: flex;
            flex-direction: row;
        }
        .columna-sensores {
            background-color: #2c3e50;
            width: 40%;
            padding: 20px 0;
            display: flex;
            flex-direction: column;
        }
        .titulo-lista {
            color: #ecf0f1;
            font-size: 16px;
            text-align: center;
            margin-bottom: 20px;
            text-transform: uppercase;
            letter-spacing: 1px;
        }
        .sensor-item {
            padding: 15px 20px;
            color: #bdc3c7;
            font-weight: bold;
            font-size: 15px;
            border-left: 5px solid transparent;
            transition: all 0.3s ease;
        }
        .sensor-item.activo {
            background-color: #34495e;
            color: #2ecc71;
            border-left: 5px solid #2ecc71;
        }
        .columna-medicion {
            width: 60%;
            padding: 40px 20px;
            text-align: center;
            display: flex;
            flex-direction: column;
            justify-content: center;
        }
        .titulo-medicion {
            font-size: 18px;
            color: #7f8c8d;
            margin-bottom: 10px;
        }
        .valor {
            font-size: 64px;
            font-weight: bold;
            color: #2c3e50;
            margin: 10px 0;
        }
        .unidad {
            font-size: 28px;
            color: #7f8c8d;
        }
        .estado {
            margin-top: 20px;
            font-size: 14px;
            color: #95a5a6;
            background-color: #ecf0f1;
            padding: 8px;
            border-radius: 4px;
            display: inline-block;
        }
        @media (max-width: 500px) {
            .contenedor-principal { flex-direction: column; }
            .columna-sensores { width: 100%; padding: 10px 0; }
            .columna-medicion { width: 100%; padding: 30px 20px; }
            .sensor-item { text-align: center; border-left: none; }
            .sensor-item.activo { border-left: none; border-bottom: 2px solid #2ecc71; }
        }
    </style>
</head>
<body>
    <div class="contenedor-principal">
        <div class="columna-sensores">
            <div class="titulo-lista">Sensores</div>
            <!-- IDs únicos para que JS pueda prender o apagar la clase "activo" -->
            <div class="sensor-item" id="nav-ina219">INA219 (3.2A)</div>
            <div class="sensor-item" id="nav-acs5">ACS712 (5A)</div>
            <div class="sensor-item" id="nav-acs20">ACS712 (20A)</div>
            <div class="sensor-item" id="nav-acs30">ACS712 (30A)</div>
        </div>
        <div class="columna-medicion">
            <div class="titulo-medicion">Corriente Actual</div>
            <div class="valor">
                <span id="corriente">--</span><span class="unidad">A</span>
            </div>
            <div class="estado">
                Modo Autorrango: <strong>ON</strong>
            </div>
        </div>
    </div>

    <script>
        setInterval(function() {
            fetch('/datos')
                .then(response => response.json())
                .then(data => {
                    // Actualizamos el valor numérico de la corriente
                    document.getElementById('corriente').innerText = data.corriente;

                    // Limpiamos la clase activo de todos los sensores primero
                    document.querySelectorAll('.sensor-item').forEach(el => el.classList.remove('activo'));

                    // Dependiendo de qué sensor devuelva el ESP32, activamos su cajita correspondiente
                    if (data.sensor.includes("INA219")) {
                        document.getElementById('nav-ina219').classList.add('activo');
                    } else if (data.sensor.includes("5A")) {
                        document.getElementById('nav-acs5').classList.add('activo');
                    } else if (data.sensor.includes("20A")) {
                        document.getElementById('nav-acs20').classList.add('activo');
                    } else if (data.sensor.includes("30A")) {
                        document.getElementById('nav-acs30').classList.add('activo');
                    }
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