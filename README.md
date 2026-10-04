# ⚡ Multímetro Automático de Rango Dinámico - ESP32

![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![ESP32](https://img.shields.io/badge/ESP32-000000?style=for-the-badge&logo=espressif&logoColor=white)
![FreeRTOS](https://img.shields.io/badge/FreeRTOS-20232A?style=for-the-badge&logo=freertos&logoColor=white)
![HTML5](https://img.shields.io/badge/HTML5-E34F26?style=for-the-badge&logo=html5&logoColor=white)

Proyecto desarrollado para la cátedra **Taller de Proyecto II (2026)** — Facultad de Informática, UNLP.
**Grupo F1**

## 📖 Descripción General
El sistema consiste en un medidor de corriente multiescala capaz de adaptar su sensibilidad de forma autónoma (Auto-ranging) dependiendo de la magnitud de la carga conectada. Utiliza un microcontrolador ESP32 con el sistema operativo en tiempo real **FreeRTOS** para gestionar el muestreo, la lógica de control mediante una Máquina de Estados Finitos (FSM) y la publicación de los datos en tiempo real mediante un servidor web embebido.

## ✨ Características Principales
* **Conmutación Segura:** Inicia por defecto en la escala de mayor tolerancia (30A) y desciende dinámicamente evaluando ventanas de umbrales para proteger el hardware.
* **Procesamiento Asíncrono:** Arquitectura basada en tareas (Tasks) distribuidas en ambos núcleos del procesador para no bloquear las lecturas durante la comunicación de red.
* **HMI (Human-Machine Interface) en Tiempo Real:** Interfaz web gráfica vectorial (SVG) servida desde el ESP32. Utiliza peticiones asíncronas (AJAX) para visualizar el flujo de corriente y el sensor activo sin recargar la página.

## 🛠️ Hardware Utilizado
* Microcontrolador ESP32 (NodeMCU).
* Módulo INA219 (Sensor I2C de ultra-precisión - 3.2A).
* Módulos ACS712 Analógicos (5A, 20A y 30A).
* Módulo de Relés Optoacoplados (4 canales).

## 🧠 Arquitectura de Software (FreeRTOS)
El firmware se encuentra dividido lógicamente para optimizar el uso de los recursos del microcontrolador:

1. **`TaskSensores` (Core 1):** Encargada de leer los datos puros (ADC e I2C) del sensor habilitado y aplicar las fórmulas de acondicionamiento de señal (offsets y sensibilidad).
2. **`TaskFSM` (Core 1):** Máquina de estados que evalúa las mediciones, controla la lógica de los relés (Active-LOW) con tiempos de seguridad y despacha la información válida.
3. **`TaskWeb` (Core 0):** Maneja la pila de red TCP/IP, el servidor HTTP y formatea los datos recibidos mediante colas (`xQueue`) al estándar JSON para ser consumidos por el dashboard web.

## 👥 Equipo de Desarrollo
* Iloro, Gonzalo
* Lacourrege, Santiago
* Arpone, David

---
*UNLP - Universidad Nacional de La Plata*