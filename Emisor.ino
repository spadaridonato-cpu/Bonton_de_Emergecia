#include <SPI.h>
#include <LoRa.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ===== CONFIGURACIÓN =====
#define ID_DISPOSITIVO "TTGO001"   // ID del TTGO

// Pines LoRa TTGO T-Beam
#define LORA_SS 18
#define LORA_RST 14
#define LORA_DIO0 26

// Pines GPS
#define RXD2 34
#define TXD2 12

// Botón físico IO38 (para SOS)
#define BOTON_IO38 38

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// GPS y LoRa
TinyGPSPlus gps;
HardwareSerial GPS_Serial(1);

void setup() {
  Serial.begin(115200);
  GPS_Serial.begin(9600, SERIAL_8N1, RXD2, TXD2);
  pinMode(BOTON_IO38, INPUT_PULLUP);
  delay(5000);

  // Inicializar OLED
  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("Error: OLED no detectada");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println("Inicializando LoRa...");
  display.display();

  // Inicializar LoRa
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Error al iniciar LoRa");
    display.setCursor(0, 30);
    display.println("LoRa FALLÓ");
    display.display();
    while (true);
  }

  Serial.println("LoRa iniciado correctamente.");
  display.setCursor(0, 30);
  display.println("LoRa iniciado OK");
  display.setCursor(0, 45);
  display.println("Esperando GPS...");
  display.display();
}

void loop() {
  // GPS continuamente leyendo
  while (GPS_Serial.available()) {
    gps.encode(GPS_Serial.read());
  }

  // Buscando comunicacion con satelites (FIX)
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  if (gps.location.isValid() && gps.location.age() < 2000) {
    display.setCursor(0, 10);
    display.println("GPS FIX OK");
    display.setCursor(0, 30);
    display.print("Lat: ");
    display.println(gps.location.lat(), 6);
    display.setCursor(0, 45);
    display.print("Lng: ");
    display.println(gps.location.lng(), 6);
  } else {
    display.setCursor(0, 20);
    display.println("Buscando satelites...");
  }
  display.display();

  // Si se presiona el botón, enviar coordenadas + SOS
  if (digitalRead(BOTON_IO38) == LOW) {
    if (gps.location.isValid() && gps.location.age() < 2000) {
      float lat = gps.location.lat();
      float lng = gps.location.lng();
      int sos = 1; // botón presionado => SOS

      String mensaje = "ID:" + String(ID_DISPOSITIVO) + 
                       " | Lat:" + String(lat, 6) + 
                       " | Lng:" + String(lng, 6) +
                       " | SOS:" + String(sos);

      LoRa.beginPacket();
      LoRa.print(mensaje);
      LoRa.endPacket();

      Serial.println("Mensaje enviado:");
      Serial.println(mensaje);

      // Mostrar mensaje de envío en pantalla
      display.clearDisplay();
      display.setCursor(0, 20);
      display.println("Coordenadas enviadas:");
      display.setCursor(0, 40);
      display.println(mensaje);
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
    delay(1000); // Anti-rebote
  }
}
