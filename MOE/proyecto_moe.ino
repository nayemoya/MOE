#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "DFRobotDFPlayerMini.h"

// Configuración de los pines GPIO para los sensores
const int pin_MQ135 = 34;
const int pin_MQ2 = 35;
const int pin_MQ7 = 36;

// Configuración de los pines para el LED RGB (Cátodo común)
const int pin_RED = 32;
const int pin_GREEN = 33;
const int pin_BLUE = 25;

// Configuración de los pines para el DFPlayer Mini
static const uint8_t PIN_MP3_TX = 26; // Conectar al RX del módulo
static const uint8_t PIN_MP3_RX = 27; // Conectar al TX del módulo

// Crear el objeto Player
DFRobotDFPlayerMini player;

// Configuración de la pantalla OLED SSD1306
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Configuración de la red WiFi
const char* ssid = "TP-Link_921C";
const char* password = "85014213";

// Configuración del broker MQTT
const char* mqtt_server = "192.168.0.172";
const char* command_topic = "sensor/command";
WiFiClient espClient;
PubSubClient client(espClient);

// Variables para las gráficas
int historial_valores_MQ135[120] = {0};
int historial_valores_MQ2[120] = {0};
int historial_valores_MQ7[120] = {0};
const int ancho_grafico = 120;
const int alto_grafico = 44;
const int max_valor = 10000;
int posicion_x = 0;
unsigned long tiempo_inicio = 0;
const unsigned long duracion_muestra = 20000; // 20 segundos
int sensor_actual = 0; // Indice para alternar entre sensores

// Función para configurar WiFi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

// Función para reconectar al broker MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conectar al broker MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("conectado");
      client.subscribe(command_topic);
    } else {
      Serial.print("falló, rc=");
      Serial.print(client.state());
      Serial.println(" intentando nuevamente en 5 segundos");
      delay(5000);
    }
  }
}

// Función de callback para recibir mensajes MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  String command = "";
  for (int i = 0; i < length; i++) {
    command += (char)payload[i];
  }
  
  if (command == "MQ135") {
    sensor_actual = 0;
  } else if (command == "MQ2") {
    sensor_actual = 1;
  } else if (command == "MQ7") {
    sensor_actual = 2;
  }
}

// Función para actualizar la pantalla OLED
void update_display(int sensor_value, int* historial, const char* label) {
  oled.clearDisplay();
  
  // Dibujar ejes X e Y
  oled.drawLine(10, 54, ancho_grafico + 10, 54, WHITE); // Eje X
  oled.drawLine(10, 10, 10, alto_grafico + 10, WHITE);  // Eje Y
  
  // Etiquetas para los ejes
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 54);
  oled.print("0");
  oled.setCursor(100, 54);
  oled.print(max_valor);
  oled.setCursor(0, 0);
  oled.print(label);

  // Mapear el valor del sensor al rango de la gráfica
  int y_pos = map(sensor_value, 0, max_valor, 54, 10);

  // Almacenar el valor en la posición actual de X
  historial[posicion_x] = y_pos;

  // Dibujar la gráfica continua desde la izquierda
  for (int x = 0; x < posicion_x; x++) {
    oled.drawLine(10 + x, historial[x], 11 + x, historial[x + 1], WHITE);
  }

  // Avanzar la posición en X para la próxima iteración
  posicion_x++;

  // Si se llega al final de la pantalla, resetear posición X
  if (posicion_x >= ancho_grafico - 1) {
    posicion_x = 0;
  }

  // Mostrar el valor del sensor
  oled.setCursor(0, y_pos - 10);
  oled.print(sensor_value);

  oled.display(); // Mostrar en la pantalla
}

void setup() {
  Serial.begin(9600);

  // Inicializar la comunicación I2C
  Wire.begin(21, 22);  // Pines SDA y SCL

  // Retraso para asegurar que todo esté inicializado
  delay(1000);

  // Configurar DFPlayer Mini
  Serial2.begin(9600, SERIAL_8N1, PIN_MP3_RX, PIN_MP3_TX);
  if (player.begin(Serial2)) {
    Serial.println("DFPlayer Mini inicializado");
    player.volume(20);
  } else {
    Serial.println("¡Conexión con DFPlayer Mini fallida!");
    for (;;);
  }

  // Inicializar la pantalla OLED
  if (!oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { 
    Serial.println(F("No se encontró la pantalla OLED"));
    for (;;);
  }
  
  // Configurar los pines del LED RGB
  pinMode(pin_RED, OUTPUT);
  pinMode(pin_GREEN, OUTPUT);
  pinMode(pin_BLUE, OUTPUT);

  // Limpiar la pantalla
  oled.clearDisplay();

  // Configurar WiFi
  setup_wifi();

  // Configurar el cliente MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  // Inicializar el tiempo de inicio
  tiempo_inicio = millis();
}

void loop() {
  // Conectar al broker MQTT si no está conectado
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Cambiar el sensor actual después de `duracion_muestra` milisegundos
  if (millis() - tiempo_inicio >= duracion_muestra) {
    tiempo_inicio = millis(); // Reiniciar el tiempo
  }

  // Leer valores de los sensores
  int value_MQ135 = analogRead(pin_MQ135);
  int value_MQ2 = analogRead(pin_MQ2);
  int value_MQ7 = analogRead(pin_MQ7);
  
  // Publicar valores en el broker MQTT
  client.publish("sensor/mq135", String(value_MQ135).c_str());
  client.publish("sensor/mq2", String(value_MQ2).c_str());
  client.publish("sensor/mq7", String(value_MQ7).c_str());
  
  // Control del LED RGB según el valor del MQ-135
  if (value_MQ135 < 873) {
    // Aire limpio - LED verde
    digitalWrite(pin_RED, LOW);
    digitalWrite(pin_GREEN, HIGH);
    digitalWrite(pin_BLUE, LOW);
  } else if (value_MQ135 >= 873 && value_MQ135 < 1500) {
    // Nivel moderado de contaminación - LED azul
    digitalWrite(pin_RED, LOW);
    digitalWrite(pin_GREEN, LOW);
    digitalWrite(pin_BLUE, HIGH);
  } else {
    // Alta contaminación - LED rojo
    digitalWrite(pin_RED, HIGH);
    digitalWrite(pin_GREEN, LOW);
    digitalWrite(pin_BLUE, LOW);
  }

  // Reproducir sonido de alarma si el valor del MQ-2 es muy alto
  if (value_MQ2 > 1500) {  // Ajusta el umbral según sea necesario
    player.volume(20);
    player.play(1);   // Reproduce el archivo de sonido 1 en la tarjeta SD
    delay(1000);  // Asegúrate de que se reproduce solo una vez por cada activación
  }
  
  // Graficar el valor del sensor actual en la pantalla OLED
  switch (sensor_actual) {
    case 0:
      update_display(value_MQ135, historial_valores_MQ135, "calidad del aire");
      break;
    case 1:
      update_display(value_MQ2, historial_valores_MQ2, "CO2 en el aire");
      break;
    case 2:
      update_display(value_MQ7, historial_valores_MQ7, "gas butano y LP");
      break;
  }
  
  // Esperar un momento para la próxima lectura
  delay(1000);  // Ajusta el tiempo según tus necesidades
}
