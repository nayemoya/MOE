# MOE
### 1\. Nombre del Proyecto
**MOE Air Quality**
### 2\. Enunciado de Visión
El proyecto MOE Air Quality tiene como objetivo proporcionar un sistema integral para la gestión y monitoreo de la calidad del aire en tiempo real, especialmente diseñado para hogares. Este sistema permitirá a los usuarios mantener un ambiente seguro para sus familias mediante la visualización de datos actuales e históricos, así como alertas sobre niveles de contaminantes y gases nocivos.
### 3\. Software Empleado
- **Nombre:** Arduino IDE
  - **Licencia:** Open Source
- **Nombre:** Visual Studio Code
  - **Licencia:** Open Source
### 4\. Hardware Empleado
- **Nombre:** Sensor de Calidad del Aire (MQ-135)
  - **Imagen:**
  -![mq135](https://github.com/nayemoya/MOE/assets/107650526/16957d61-02b9-4ee7-9df5-83b05b292a0e)
  - **Descripción:** Sensor utilizado para medir la calidad del aire, detectando varios gases nocivos.
  - **Cantidad:** 2
- **Nombre:** Microcontrolador ESP32
  - **Imagen:**
- ![esp32](https://github.com/nayemoya/MOE/assets/107650526/9769a898-499e-4028-b18a-68f05fd11648)
  - **Descripción:** Microcontrolador con conectividad Wi-Fi y Bluetooth, usado para procesar los datos de los sensores.
  - **Cantidad:** 1
### 5\. Historias de Usuario Épicas
- **Cristian Emmanuel Gutiérrez Díaz:** Gestión de dióxido de carbono en el aire.

  - **ID:** H-1
  - **Historia:** Como una madre de casa quiero mantener mi casa con un ambiente seguro para mi familia. Quiero poder monitorear la calidad del aire de mi casa en mi teléfono o algún dispositivo especializado. Debe incluir gráficas, alertas, información relevante niveles de aire. En el Sprint número: 3 sprint.
- **Nayeli Moya Díaz:** Desarrollo de web para datos reales.
  - **ID:** H-3
  - **Historia:** Como una persona que usa mucho el teléfono quiero poder monitorear el aire de mi casa en mi dispositivo. Quiero poder monitorear la calidad del aire de mi casa en mi teléfono o algún dispositivo especializado. Debe incluir un dominio seguro, información del sistema, alertas, experiencia de usuario. En el Sprint número: 2 sprint.
- **Oscar Iván Pérez Mejía:** Monitoreo de la calidad del aire en tiempo real.
  - **ID:** H-4
  - **Historia:** Como una abuela quiero saber si en mi casa hay algún gas o sustancia gaseosa nociva para mi salud. Quiero poder escuchar en caso de mala calidad de aire en mi ambiente o en caso de gases nocivos para la salud. Debe detectar gases nocivos, diferenciar tipos de gas (posible). En el Sprint número: 2 sprint.
### 6\. Fotografía del Prototipo
(Nota: Puedes usar software de diseño como Figma, Sketch o incluso PowerPoint para crear el diseño profesional del prototipo.)

- **Descripción del Prototipo:** Prototipo de dispositivo wearable con pantalla LED para mostrar la calidad del aire, equipado con sensores de gas y conectividad Wi-Fi para sincronización con aplicaciones móviles.
### 7\. Arquitectura del Proyecto
- **Sensores:** MQ-135 (Calidad del aire)
- **Actuadores:** Alarma sonora para alertas de gases nocivos
- **Controladores:** ESP32
- **Plataforma de Base de Datos:** ThingSpeak para almacenamiento y análisis de datos
- **Protocolo de Comunicación:** MQTT para transmisión de datos entre sensores y servidor
- **Gestión de Energía:** Baterías recargables de Li-ion
- **Dispositivos Receptores y/o Transmisores:** Teléfonos móviles, tabletas, laptops
- **Elementos Adicionales:**
  - **Nube:** ThingSpeak para análisis y almacenamiento de datos en la nube
  - **Redes:** Wi-Fi para conectividad del ESP32
  - **Seguridad:** SSL/TLS para la comunicación segura de datos
### 8\. Captura de Pantalla del Tablero Kanban
![Captura de pantalla de computadora

Descripción generada automáticamente](Aspose.Words.6671419a-ba0f-4ee7-b452-24cd66e1236a.001.png)

- **Sprint 1:** 
- **Sprint 2:** 
- **Sprint 3:** 
### 9\. Circuito Diseñado para el Proyecto Completo
(Nota: Usa software como Fritzing para diseñar el circuito.)

- **Descripción del Circuito:** Circuito que incluye el ESP32, sensores MQ-135, y conexiones a una alarma sonora y una pantalla LED para visualización de datos.
### 10\. Resultados
- **Sprint 1 Resultado:** 
  - **Descripción:** Integración inicial del sensor MQ-135 con el ESP32, datos enviados a ThingSpeak.
- **Sprint 2 Resultado:** 
  - **Descripción:** Desarrollo de la web con visualización en tiempo real y alertas implementadas.
- **Sprint 3 Resultado:** 
  - **Descripción:** Visualización histórica de datos y pruebas comparativas de calidad del aire realizadas exitosamente.

