#pragma once //es lo mismo que ifndef y define

#include <Wire.h> //Mmaneja el bus i2c
#include <Adafruit_INA219.h> //libreria para el ina
#include <ACS712.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "fsm.h"
#define T_REF 3.3
#define RES_ADC 4095.0

//Esto hay calcularlo, porahora usamos esto
#define OFFSET_ACS30 1.65
#define OFFSET_ACS20 1.65
#define OFFSET_ACS5 1.65
#define SENS_ACS30 0.066
#define SENS_ACS20 0.100
#define SENS_ACS5 0.185

struct ParametrosSensores {
    Adafruit_INA219* ina;      // Puntero al objeto INA
    QueueHandle_t cola;        // Handle de la cola (ya es un puntero por definición)
    EstadoSistema* estado;     // Puntero a la variable que guarda el estado actual
};

void task_sensores(void *pvParameters);