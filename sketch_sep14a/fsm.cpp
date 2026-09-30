#include "fsm.h"

void task_fsm(void *pvParameters){
  
  ParametrosSensores* params = (ParametrosSensores*) pvParameters;

  QueueHandle_t cola_sensor = params->cola_sensor;
  QueueHandle_t cola_web = params->cola_web;
  float msj = 0;
  for(;;){
    if(xQueueReceive(cola_sensor, &msj, portMAX_DELAY) == pdTRUE){
        switch (*(params->estado)) {
          case ACS712_30:
                if ((msj<30) && (msj>=5))
                {
                  *estado = ACS712_20;  
                }
                else if ((msj<5) && (msj>=3.2))
                {
                  *estado= ACS712_5;
                }
                else if ((msj<5) && (msj>=3.2))
                {
                  *estado= INA219;
                }
              break;
          case ACS712_20:
                if ((msj>=20))
                {
                  *estado= ACS712_30;
                }
                else if ((msj<5) && (msj>=3.2))
                {
                  *estado= ACS712_5;
                }
                else if ((msj<5) && (msj>=3.2))
                {
                  *estado= INA219;
                }
              break;
          case ACS712_5:
                if ((msj>=20))
                {
                  *estado= ACS712_20;
                }
                else if ((msj<5) && (msj>=3.2))
                {
                  *estado= ACS712_5;
                }
                else if ((msj<5) && (msj>=3.2))
                {
                  *estado= INA219;
                }
              break;
          case INA219:
                if ((msj>=20))
                {
                  *estado= ACS712_30;
                }
                else if ((msj<5) && (msj>=3.2))
                {
                  *estado= ACS712_5;
                }
                else if ((msj<5) && (msj>=3.2))
                {
                  *estado= INA219;
                }
              break;
        default:break;
        }
      cambiarReles(*estado);
    }
    vTaskDelay(pdMS_TO_TICKS(50));
    xQueueSend(cola_web,&lectura,0);
  }

}

void cambiarReles(EstadoSistema est){
 switch (*(params->estado)) {
      case ACS712_30:
          digitalWrite(RELE30, LOW);
          digitalWrite(RELE20, LOW);
          digitalWrite(RELE5, LOW);
          digitalWrite(RELE3, LOW);
          digitalWrite(RELE30, HIGH);
          break;
      case ACS712_20:
          digitalWrite(RELE30, LOW);
          digitalWrite(RELE20, LOW);
          digitalWrite(RELE5, LOW);
          digitalWrite(RELE3, LOW);
          digitalWrite(RELE20, HIGH);
        break;
      case ACS712_5:
          digitalWrite(RELE30, LOW);
          digitalWrite(RELE20, LOW);
          digitalWrite(RELE5, LOW);
          digitalWrite(RELE3, LOW);
          digitalWrite(RELE5, HIGH);
        break;
      case INA219:
          digitalWrite(RELE30, LOW);
          digitalWrite(RELE20, LOW);
          digitalWrite(RELE5, LOW);
          digitalWrite(RELE3, LOW);
          digitalWrite(RELE3, HIGH);
        break;
     default:
     break;
    }
}
}