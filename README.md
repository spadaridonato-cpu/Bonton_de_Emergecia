# Bonton_de_Emergecia

# Descripción
Un sistema de alerta de emergencia inalámbrico de largo alcance diseñado para deportes acuáticos como el kayak.
Al pulsar un botón, un dispositivo ESP32 LoRa transmite una señal de socorro a una estación base, donde una aplicación Python recibe la alerta y activa una notificación.

## Tecnologías y Componentes Utilizados
- TTGO LoRa T-Beam 1.2 (ESP32 + LoRa)
- LILYGO ESP32
- Comunicación LoRa
- Python (software del receptor)
- Comunicación inalámbrica de largo alcance
- C++ Integrado (ESP32)

## Cómo Funciona
El dispositivo portátil contiene un botón conectado a un módulo ESP32 LoRa.

Al pulsarlo:
1. El ESP32 envía un paquete LoRa inalámbrico.
2. Una estación receptora captura la señal.
3. Una aplicación Python procesa el mensaje.
4. Se genera una alerta en el ordenador.

Esto permite la señalización de emergencia incluso en zonas sin cobertura móvil.

## 📡 Arquitectura del sistema
