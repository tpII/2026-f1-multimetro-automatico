void task_sensores(void *pvParameters){

  float lectura = 0;
  //Para no generar un quilombo de extern recibo este estruct por parametro desde fsm
  ParametrosSensores* params = (ParametrosSensores*) pvParameters;// Desesmpaqueto los punteros que necesito para utilizar la funcion del INA y acceder a los estados definidos en fsm
  QueueHandle_t cola_lectura = params->cola;
  for(;;){
    switch (*(params->estado)) {
      case ACS712_30:
          lectura = conversion_dato(*(params->estado),(float)analogRead(34));
          xQueueSend(cola_lectura,&lectura,0);
          break;
      case ACS712_20:
          lectura = conversion_dato(*(params->estado),(float)analogRead(33));
          xQueueSend(cola_lectura,&lectura,0);
        break;
      case ACS712_5:
          lectura = conversion_dato(*(params->estado) ,(float)analogRead(32));
          xQueueSend(cola_lectura,&lectura,0);
        break;
      case INA219:
        lectura = params->ina219->getCurrent_mA() / 1000.0; // Ya devuelve amper
          xQueueSend(cola_lectura,&lectura,0);
        break;
     default:break;
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }


}

float conversion_dato( EstadoSistema id , float dato){

      dato = (dato * T_REF) / RES_ADC; //Transformo la lectura en volt
      //Resto el OFFSET y divido por la sensibilidad para obtener amper
      switch(id){
        case ACS712_30:
          dato = ((dato-OFFSET_ACS30) /SENS_ACS30); 
        break;
        case ACS712_20:
          dato = ((dato-OFFSET_ACS20) /SENS_ACS20);
        break;
        case ACS712_5:
          dato = ((dato-OFFSET_ACS5) /SENS_ACS5);
        break;
        default:break;
      }
  return dato;

}
