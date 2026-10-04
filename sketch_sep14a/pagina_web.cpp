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
    <title>Multímetro Automático HMI</title>
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background-color: #1e272e;
            color: #d2dae2;
            margin: 0;
            padding: 20px;
            display: flex;
            flex-direction: column;
            align-items: center;
            min-height: 100vh;
        }

        /* Panel Superior con el número gigante */
        .panel-medicion {
            background-color: #2c3e50;
            padding: 20px 50px;
            border-radius: 10px;
            box-shadow: 0 5px 15px rgba(0,0,0,0.5);
            text-align: center;
            margin-bottom: 30px;
            border: 1px solid #34495e;
        }
        
        .valor-corriente {
            font-size: 70px;
            font-weight: bold;
            color: #0fbcf9;
            text-shadow: 0 0 10px rgba(15, 188, 249, 0.5);
            margin: 10px 0;
        }
        .unidad { font-size: 30px; color: #808e9b; }

        /* Contenedor del Diagrama */
        .panel-diagrama {
            background-color: #2b333e;
            padding: 20px;
            border-radius: 10px;
            box-shadow: inset 0 0 20px rgba(0,0,0,0.5);
            width: 100%;
            max-width: 900px;
            overflow-x: auto;
        }

        /* Estilos base para el dibujo SVG */
        svg { width: 100%; min-width: 700px; height: auto; }
        
        .caja {
            fill: #34495e;
            stroke: #485460;
            stroke-width: 2;
            transition: all 0.3s ease;
        }
        .texto-caja {
            fill: #d2dae2;
            font-size: 14px;
            font-weight: bold;
            font-family: Arial, sans-serif;
            text-anchor: middle;
            dominant-baseline: middle;
        }

        .linea {
            fill: none;
            stroke: #485460;
            stroke-width: 4;
            stroke-linejoin: round;
            transition: all 0.3s ease;
        }

        /* ----- ESTILOS CUANDO ESTÁN ACTIVOS ----- */
        .caja.activa {
            fill: #05c46b; /* Verde industrial */
            stroke: #0be881;
            filter: drop-shadow(0 0 8px rgba(5, 196, 107, 0.6));
        }
        
        .linea.activa {
            stroke: #0fbcf9; /* Azul eléctrico brillante */
            stroke-width: 6;
            filter: drop-shadow(0 0 8px rgba(15, 188, 249, 0.8));
            /* Animación de flujo de corriente */
            stroke-dasharray: 15 10;
            animation: flujo 1s linear infinite reverse;
        }

        @keyframes flujo {
            to { stroke-dashoffset: 50; }
        }
    </style>
</head>
<body>

    <div class="panel-medicion">
        <div style="color: #808e9b; text-transform: uppercase; letter-spacing: 2px;">Lectura en tiempo real</div>
        <div class="valor-corriente">
            <span id="txt-corriente">--</span><span class="unidad"> A</span>
        </div>
        <div style="color: #05c46b; font-size: 14px;">● Sistema en línea y censando</div>
    </div>

    <div class="panel-diagrama">
        <!-- SVG del Diagrama de Bloques -->
        <svg viewBox="0 0 800 450">
            
            <!-- CAJAS ESTATICAS (Fuente, Relés, Carga) -->
            <rect x="20" y="200" width="100" height="50" rx="5" class="caja" />
            <text x="70" y="225" class="texto-caja">Alimentación</text>

            <rect x="180" y="160" width="80" height="130" rx="5" class="caja" />
            <text x="220" y="225" class="texto-caja">Relés</text>

            <rect x="650" y="175" width="120" height="100" rx="5" class="caja" />
            <text x="710" y="225" class="texto-caja">Carga Motor/Luz</text>

            <!-- LÍNEA ESTÁTICA PRINCIPAL (Entrada) -->
            <path d="M 120 225 L 180 225" class="linea activa" style="stroke:#f1c40f;" /> <!-- Siempre encendida amarilla -->

            <!-- ================= RAMA INA 219 ================= -->
            <path id="linea-ina219" d="M 260 180 L 320 180 L 320 75 L 430 75 M 550 75 L 600 75 L 600 200 L 650 200" class="linea" />
            <rect id="caja-ina219" x="430" y="50" width="120" height="50" rx="5" class="caja" />
            <text x="490" y="75" class="texto-caja">INA219 (3.2A)</text>

            <!-- ================= RAMA ACS 5A ================= -->
            <path id="linea-acs5" d="M 260 200 L 340 200 L 340 175 L 430 175 M 550 175 L 580 175 L 580 215 L 650 215" class="linea" />
            <rect id="caja-acs5" x="430" y="150" width="120" height="50" rx="5" class="caja" />
            <text x="490" y="175" class="texto-caja">ACS712 (5A)</text>

            <!-- ================= RAMA ACS 20A ================= -->
            <path id="linea-acs20" d="M 260 230 L 340 230 L 340 275 L 430 275 M 550 275 L 580 275 L 580 230 L 650 230" class="linea" />
            <rect id="caja-acs20" x="430" y="250" width="120" height="50" rx="5" class="caja" />
            <text x="490" y="275" class="texto-caja">ACS712 (20A)</text>

            <!-- ================= RAMA ACS 30A ================= -->
            <path id="linea-acs30" d="M 260 260 L 320 260 L 320 375 L 430 375 M 550 375 L 600 375 L 600 245 L 650 245" class="linea" />
            <rect id="caja-acs30" x="430" y="350" width="120" height="50" rx="5" class="caja" />
            <text x="490" y="375" class="texto-caja">ACS712 (30A)</text>

        </svg>
    </div>

    <script>
        // Función para apagar todo antes de encender el correcto
        function resetearDiagrama() {
            document.querySelectorAll('.caja').forEach(el => el.classList.remove('activa'));
            document.querySelectorAll('.linea').forEach(el => el.classList.remove('activa'));
        }

        setInterval(function() {
            fetch('/datos')
                .then(response => response.json())
                .then(data => {
                    // 1. Actualizamos el número gigante
                    document.getElementById('txt-corriente').innerText = data.corriente;

                    // 2. Apagamos todas las líneas y cajas
                    resetearDiagrama();

                    // 3. Encendemos (le agregamos la clase "activa") a la ruta correspondiente
                    if (data.sensor.includes("INA219")) {
                        document.getElementById('caja-ina219').classList.add('activa');
                        document.getElementById('linea-ina219').classList.add('activa');
                    } else if (data.sensor.includes("5A")) {
                        document.getElementById('caja-acs5').classList.add('activa');
                        document.getElementById('linea-acs5').classList.add('activa');
                    } else if (data.sensor.includes("20A")) {
                        document.getElementById('caja-acs20').classList.add('activa');
                        document.getElementById('linea-acs20').classList.add('activa');
                    } else if (data.sensor.includes("30A")) {
                        document.getElementById('caja-acs30').classList.add('activa');
                        document.getElementById('linea-acs30').classList.add('activa');
                    }
                })
                .catch(err => console.log("Error de conexión"));
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