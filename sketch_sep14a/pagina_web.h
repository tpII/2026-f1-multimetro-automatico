#pragma once //es lo mismo que ifndef y define
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <Arduino.h>

struct MensajeWeb {
    float lectura;
    EstadoSistema estado;
}
void task_web(void *pvParameters);