from machine import Pin, ADC, SoftI2C
from ssd1306 import SSD1306_I2C
from time import sleep
import network
from umqtt.simple import MQTTClient

# Constantes para conexion con el cliente MQTT
MQTT_CLIENT_ID = ""
MQTT_BROKER = "broker.hivemq.com"
MQTT_USER = ""
MQTT_PASSWORD = ""
MQTT_TOPIC_CO2 = "CEGD-gas-co2"
MQTT_TOPIC_AIR_QUALITY = "CEGD-gas-airquality"
MQTT_PORT = 1883

# Asignacion de pines para la OLED
i2c = SoftI2C(scl=Pin(22), sda=Pin(21))

# Objeto que controla la OLED
oled = SSD1306_I2C(128, 64, i2c)

# Variables para los leds
led_verde = Pin(12, Pin.OUT)
led_amarillo = Pin(14, Pin.OUT)  # Pin para el LED amarillo
led_rojo = Pin(13, Pin.OUT)

# Pin para el buzzer
buzzer = Pin(27, Pin.OUT)  # Cambia 27 por el pin que estés usando para el buzzer

# Definicion del sensor de CO2 con el ADC (convertidor ANALOGICO-DIGITAL)
sensor_co2 = ADC(Pin(32))

# Definicion del sensor de calidad del aire (MQ135) con el ADC
sensor_air_quality = ADC(Pin(34))

# Ajustes para los sensores
sensor_co2.width(ADC.WIDTH_10BIT)  # Regula precision del sensor
sensor_co2.atten(ADC.ATTN_11DB)    # Trabaja sobre 3V3

sensor_air_quality.width(ADC.WIDTH_10BIT)  # Regula precision del sensor
sensor_air_quality.atten(ADC.ATTN_11DB)    # Trabaja sobre 3V3

# Conexion a wifi
def conectar_wifi():
    sta_if = network.WLAN(network.STA_IF)
    sta_if.active(True)
    sta_if.connect('Wokwi-GUEST', '')
    while not sta_if.isconnected():
        print(".", end="")
        sleep(0.1)
    print("Conectado")

def conectar_broker():
    print("Conectando al broker", MQTT_BROKER)
    cliente = MQTTClient(MQTT_CLIENT_ID, MQTT_BROKER, user=MQTT_USER, password=MQTT_PASSWORD, port=MQTT_PORT)
    cliente.connect()
    return cliente

conectar_wifi()
cliente = conectar_broker()

# Función para convertir lectura del MQ-135 a PPM (esto es una aproximación y puede necesitar calibración)
def lectura_a_ppm(lectura):
    # La fórmula de conversión puede variar, esto es solo un ejemplo.
    return lectura * (1000 / 1023)

# Ciclo infinito
while True:
    # Leer el sensor de CO2
    lectura_co2 = sensor_co2.read()
    
    # Leer el sensor de calidad del aire (MQ135)
    lectura_air_quality = sensor_air_quality.read()
    ppm = lectura_a_ppm(lectura_air_quality)

    # Limpiar pantalla
    oled.fill(0)

    # Mostrar datos de CO2 en la OLED
    oled.text("CO2 Level:", 0, 0)
    oled.text(str(lectura_co2), 0, 10)

    # Mostrar datos de calidad del aire en la OLED
    oled.text("Air Quality:", 0, 30)
    oled.text(str(ppm) + " PPM", 0, 40)

    if lectura_co2 > 800:
        oled.text("Alerta CO Alto!", 0, 20)
        led_rojo.value(1)
        led_amarillo.value(0)
        led_verde.value(0)
        buzzer.value(1)  # Encender buzzer
    elif lectura_co2 > 400:
        oled.text("CO Moderado", 0, 20)
        led_rojo.value(0)
        led_amarillo.value(1)
        led_verde.value(0)
        buzzer.value(0)  # Apagar buzzer
    else:
        oled.text("CO Normal", 0, 20)
        led_rojo.value(0)
        led_amarillo.value(0)
        led_verde.value(1)
        buzzer.value(0)  # Apagar buzzer

    if ppm > 700:
        oled.text("Calidad Aire: Mala", 0, 50)
    elif ppm > 300:
        oled.text("Calidad Aire: Regular", 0, 50)
    else:
        oled.text("Calidad Aire: Buena", 0, 50)

    oled.show()
    cliente.publish(MQTT_TOPIC_CO2, str(lectura_co2))
    cliente.publish(MQTT_TOPIC_AIR_QUALITY, str(ppm))
    print("Publicando en {}, En el topico {}, el valor {}".format(MQTT_BROKER, MQTT_TOPIC_CO2, str(lectura_co2)))
    print("Publicando en {}, En el topico {}, el valor {}".format(MQTT_BROKER, MQTT_TOPIC_AIR_QUALITY, str(ppm)))
    sleep(10)