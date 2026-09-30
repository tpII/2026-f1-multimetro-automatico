#include <Arduino.h>
#include "fsm.h"              
#include "lectura_sensores.h" 

// 1. Creación de los objetos y variables globales en memoria
Adafruit_INA219 mi_ina;
QueueHandle_t cola_sensor;
QueueHandle_t cola_web;
EstadoSistema estado_fsm = ACS712_30; // Arrancamos por defecto en el sensor más grande por seguridad
ParametrosSensores params_sensores;   // El paquete que le vamos a enviar a la tarea

ParametroFsm params_fsm;

void setup() {
    Serial.begin(115200); //Imprimo si surge algun error

    // DEFINIR LAS ENTRADAS ANALÓGICAS. LA ENTRADA DEL INA SE DEFINE EN EL ina.begin
    pinMode(34, INPUT);
    pinMode(33, INPUT);
    pinMode(32, INPUT);

    // DEFINIR LOS RELE
    pinMode(17, OUTPUT); // RELE 30A
    pinMode(4, OUTPUT);  // RELE 20A
    pinMode(15, OUTPUT); // RELE 5A
    pinMode(18, OUTPUT); // RELE INA

    // 2. Inicializar el sensor I2C
    if (!mi_ina.begin()) {
        Serial.println("Error: No se encontro el INA219");
    }

    // 3. Crear la cola por la cual la tarea de lectura se comunica con la tarea fsm (espacio para 10 lecturas tipo float)
    mi_cola = xQueueCreate(10, sizeof(float));
    if (mi_cola == NULL) {
        Serial.println("Error: No se pudo crear la cola");
    }

    // 4. Empaquetar los punteros en la estructura para que la reciba la tarea de lectura
    params_sensores.ina = &mi_ina;
    params_sensores.cola = cola_sensor;
    params_sensores.estado = &estado_fsm;
   
    // 5. Lanzar la tarea de lectura de sensores
    xTaskCreatePinnedToCore(
        task_sensores,             // Nombre de la función
        "TaskSensores",            // Nombre de texto para debug
        2048,                      // Tamaño en memoria RAM reservada
        (void*)&params_sensores,   // Pasamos la estructura empaquetada casteada a void*
        1,                         // Prioridad de la tarea (1 está perfecto)
        NULL,                      // Variable para guardar el handle de la tarea (no nos hace falta)
        1                          // Anclamos la tarea al Core 1 (Lógica de hardware)
    );
    params_fsm.cola_sensor = cola_sensor;
    params_fsm.cola_web = cola_web;
    params_fsm.estado_fsm = &estado_fsm;
    xTaskCreatePinnedToCore(
    task_fsm,             // Nombre de la función
    "TaskFsm",            // Nombre de texto para debug
    2048,                      // Tamaño en memoria RAM reservada
    (void*)&params_fsm,   // Pasamos la estructura empaquetada casteada a void*
    1,                         // Prioridad de la tarea (1 está perfecto)
    NULL,                      // Variable para guardar el handle de la tarea (no nos hace falta)
    1                          // Anclamos la tarea al Core 1 (Lógica de hardware)
);
}

void loop() {
    // En arquitecturas RTOS puras, el loop clásico queda vacío.
    // Usamos vTaskDelete(NULL) para matar esta tarea residual de Arduino 
    // y liberar recursos, ya que tus propias tasks harán todo el trabajo.
    vTaskDelete(NULL); 
}