#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DFPlayerMini_Fast.h>

// Configuración de los pines GPIO para los sensores
const int pin_MQ135 = 34;
const int pin_MQ2 = 35;
const int pin_MQ7 = 36;

// Configuración de los pines para el LED RGB
const int pin_RED = 25;
const int pin_GREEN = 26;
const int pin_BLUE = 27;

// Configuración de los pines para el DFPlayer Mini
#define DFPLAYER_RX 16  // Conectado al pin TX del DFPlayer Mini
#define DFPLAYER_TX 17  // Conectado al pin RX del DFPlayer Mini
HardwareSerial mySerial(2); // UART 2 (TX2 y RX2)
DFPlayerMini_Fast myDFPlayer;

// Configuración de la pantalla OLED SSD1306
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1                 // Pin de reset (no se usa si está a -1)
#define SCREEN_ADDRESS 0x3C              // Dirección I2C común para SSD1306

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Configuración de la red WiFi
const char* ssid = "HP-VICTUS";
const char* password = "Qwerty090903";

// Configuración del broker MQTT
const char* mqtt_server = "10.14.55.68";
WiFiClient espClient;
PubSubClient client(espClient);

// Funciones de configuración
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

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conectar al broker MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("conectado");
    } else {
      Serial.print("falló, rc=");
      Serial.print(client.state());
      Serial.println(" intentando nuevamente en 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Inicializar la comunicación I2C
  Wire.begin(21, 22);  // Pines SDA y SCL

  // Inicializar la pantalla OLED
  if (!oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { // Cambia SSD1306_I2C_ADDRESS por SCREEN_ADDRESS
    Serial.println(F("No se encontró la pantalla OLED"));
    for (;;);
  }
  
  // Configurar los pines del LED RGB
  pinMode(pin_RED, OUTPUT);
  pinMode(pin_GREEN, OUTPUT);
  pinMode(pin_BLUE, OUTPUT);

  // Configurar DFPlayer Mini
  mySerial.begin(9600, SERIAL_8N1, DFPLAYER_RX, DFPLAYER_TX);
  if (!myDFPlayer.begin(mySerial)) {
    Serial.println(F("No se encontró el DFPlayer Mini"));
    for (;;);
  }
  myDFPlayer.volume(30);  // Volumen entre 0 y 30

  // Limpiar la pantalla
  oled.clearDisplay();

  // Configurar WiFi
  setup_wifi();

  // Configurar el cliente MQTT
  client.setServer(mqtt_server, 1883);
}

void loop() {
  // Conectar al broker MQTT si no está conectado
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Leer valores de los sensores
  int value_MQ135 = analogRead(pin_MQ135);
  int value_MQ2 = analogRead(pin_MQ2);
  int value_MQ7 = analogRead(pin_MQ7);
  
  // Publicar valores en el broker MQTT
  client.publish("sensor/mq135", String(value_MQ135).c_str());
  client.publish("sensor/mq2", String(value_MQ2).c_str());
  client.publish("sensor/mq7", String(value_MQ7).c_str());
  
  // Control del LED RGB según el valor del MQ-135
  if (value_MQ135 < 300) {
    // Aire limpio - LED verde
    digitalWrite(pin_RED, LOW);
    digitalWrite(pin_GREEN, HIGH);
    digitalWrite(pin_BLUE, LOW);
  } else if (value_MQ135 >= 300 && value_MQ135 < 600) {
    // Nivel moderado de contaminación - LED amarillo
    digitalWrite(pin_RED, HIGH);
    digitalWrite(pin_GREEN, HIGH);
    digitalWrite(pin_BLUE, LOW);
  } else {
    // Alta contaminación - LED rojo
    digitalWrite(pin_RED, HIGH);
    digitalWrite(pin_GREEN, LOW);
    digitalWrite(pin_BLUE, LOW);
  }

  // Reproducir sonido de alarma si el valor del MQ-2 es muy alto
  if (value_MQ2 > 6600) {  // Ajusta el umbral según sea necesario 300 a 10000 ppm
    myDFPlayer.play(1);   // Reproduce el archivo de sonido 1 en la tarjeta SD
  }
  
  // Limpiar la pantalla
  oled.clearDisplay();
  
  // Mostrar los valores en la pantalla OLED
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0, 0);
  oled.print("MQ-135: ");
  oled.println(value_MQ135);
  oled.setCursor(0, 20);
  oled.print("MQ-2: ");
  oled.println(value_MQ2);
  oled.setCursor(0, 40);
  oled.print("MQ-7: ");
  oled.println(value_MQ7);
  
  // Actualizar la pantalla para mostrar el texto
  oled.display();
  
  // Espera 1 minuto antes de la próxima lectura
  delay(60000);
}