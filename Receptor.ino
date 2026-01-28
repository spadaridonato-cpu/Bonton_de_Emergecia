#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pines del LoRa
#define SS 18
#define RST 14
#define DIO0 26

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  delay(10000);
  Serial.begin(115200);
  
  // Inicializar OLED
  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("Error: OLED no detectada");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 30);
  display.println("Inicializando LoRa...");
  display.display();
  
  Serial.println("Inicializando LoRa...");
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("Error al iniciar LoRa.");
    while (1);
  }

  Serial.println("LoRa Receptor listo.");
}

void loop() {
    // ESCUCHAR SERIAL DESDE PYTHON
    if (Serial.available()) {
        String mensajeSerial = Serial.readStringUntil('\n'); // lee la línea completa
        mensajeSerial.trim();
        if (mensajeSerial.length() > 0) {
            // Enviar mensaje por LoRa a los emisores
            LoRa.beginPacket();
            LoRa.print(mensajeSerial);
            LoRa.endPacket();
            Serial.println("Mensaje enviado a TTGO: " + mensajeSerial);
            
            display.clearDisplay();
            display.setCursor(0, 20);
            display.println("Coordenadas Recibidas");
            display.setCursor(0, 40);
            display.println(mensajeSerial);
            display.display();
        }
    }

    // ESCUCHAR MENSAJES LORA DE EMISORES
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        String message = "";
        while (LoRa.available()) {
            message += (char)LoRa.read();
        }
        Serial.println("Mensaje recibido: " + message);

        display.clearDisplay();
        display.setCursor(0, 20);
        display.println("Coordenadas Recibidas");
        display.setCursor(0, 40);
        display.println(message);
        display.display();
    } else {
        Serial.println("No hay FIX, no se enviaron coords.");
        display.clearDisplay();
        display.setCursor(0, 20);
        display.println("GPS sin FIX");
        display.setCursor(0, 40);
        display.println("No se puede enviar");
        display.display();
    }
}

