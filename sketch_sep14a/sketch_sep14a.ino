#include <Arduino.h>
#include "fsm.h"              
#include "lectura_sensores.h" 
#include "pagina_web.h"
// Creación de los objetos y variables globales en memoria

Adafruit_INA219 mi_ina;
QueueHandle_t cola_sensor;
QueueHandle_t cola_web;
EstadoSistema estado_fsm = ACS712_30; // Arrancamos por defecto en el sensor más grande por seguridad
ParametrosSensores params_sensores;   // El paquete que le vamos a enviar a la tarea

ParametroFsm params_fsm;

void setup() {
    Serial.begin(115200); // Imprimo si surge algun error

    // DEFINimos LAS ENTRADAS ANALÓGICAS. LA ENTRADA DEL INA SE DEFINE EN EL ina.begin
    pinMode(34, INPUT);
    pinMode(33, INPUT);
    pinMode(32, INPUT);

    // DEFINIMos LOS RELE
    pinMode(17, OUTPUT); // RELE 30A
    pinMode(4, OUTPUT);  // RELE 20A
    pinMode(15, OUTPUT); // RELE 5A
    pinMode(18, OUTPUT); // RELE INA

    // Inicialiamos el sensor I2C
    if (!mi_ina.begin()) {
        Serial.println("Error: No se encontro el INA219");
    }

    // Crea la cola por la cual la tarea de lectura se comunica con la tarea fsm (espacio para 10 lecturas tipo float)
    cola_sensor = xQueueCreate(10, sizeof(float));
    cola_web = xQueueCreate(10, sizeof(MensajeWeb));
    if ((cola_sensor == NULL)|| (cola_web == NULL)) {
        Serial.println("Error: No se pudo crear la cola");
    }
    //Empaqueta los punteros en la estructura para que la reciba la tarea de lectura
    params_sensores.ina = &mi_ina;
    params_sensores.cola = cola_sensor;
    params_sensores.estado = &estado_fsm;
   
    //Lanzar la tarea de lectura de sensores
    xTaskCreatePinnedToCore(
        task_sensores,             // Nombre de la función
        "TaskSensores",            // Nombre de texto para debug
        2048,                      // Tamaño en memoria RAM reservada
        (void*)&params_sensores,   // Pasamos la estructura empaquetada casteada a void*
        1,                         // Prioridad de la tarea
        NULL,                      // Variable para guardar el handle de la tarea (no nos hace falta)
        1                          // Anclamos la tarea al Core 1 (Lógica de hardware)
    );
    params_fsm.cola_sensor = cola_sensor;
    params_fsm.cola_web = cola_web;
    params_fsm.estado_fsm = &estado_fsm;
    xTaskCreatePinnedToCore(
    task_fsm,             
    "TaskFsm",            
    2048,                    
    (void*)&params_fsm,  
    1,                        
    NULL,                      
    1                      
);
   xTaskCreatePinnedToCore(
    task_web,             
    "TaskWeb",            
    4096,                    
    (void*)&params_fsm,  
    1,                        
    NULL,                      
    0 //core 0                     
);
}

void loop() {
    // Usamos vTaskDelete(NULL) para matar esta tarea residual de Arduino 
    vTaskDelete(NULL); 
}