# Bonton_de_Emergecia_para_Kayakistas (B.E.K)

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
1. El ESP32 envía un paquete LoRa inalámbrico, este contiene un mensaje con las coordenadas tomadas por el GPS del ESP32.
2. Una estación receptora captura la señal con el otro dispositivo, este conectado a la computadora.
3. Una aplicación Python procesa el mensaje que fue enviado anteriormente por monitor serie.
4. Se genera una alerta en la computadora, marcando en un mapa la ubicación exacta del civil.

Esto permite la señalización de emergencia incluso en zonas sin cobertura móvil.
