#pragma once //es lo mismo que ifndef y define

#include <stdint.h>
#include <Arduino.h>
#define RELE30 17.0
#define RELE20 4.0
#define RELE5 15.0
#define RELE3 18.0
struct ParametrosFSM {
    QueueHandle_t cola_sensor;        // Handle de la cola (ya es un puntero por definición)
    QueueHandle_t cola_web;
    EstadoSistema* estado;     // Puntero a la variable que guarda el estado actual
};

enum EstadosSistema{
  REPOSO,
  ACS712_30,
  ACS712_20,
  ACS712_5,
  INA219
};

void cambiarReles(EstadoSistema);