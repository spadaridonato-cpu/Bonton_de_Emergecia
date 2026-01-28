import serial
import requests
import re
import threading
import tkinter as tk
from tkinter import messagebox
from flask import Flask, request, jsonify
from flask_cors import CORS

# ===== CONFIG FLASK =====
app = Flask(__name__)
CORS(app)

sistema_activo = False

# ABRIR PUERTO SERIAL
try:
    puerto = serial.Serial("COM8", 115200, timeout=1)
except serial.SerialException:
    root = tk.Tk()
    root.withdraw()  # ocultar ventana principal de Tk
    messagebox.showerror("Error", "No se detecta el receptor en COM8.\nConéctelo y vuelva a abrir el programa.")
    raise SystemExit  # salir del programa limpio

# RECEPCIÓN (de los emisores)
def procesar_mensaje(mensaje):
    if "No hay FIX" in mensaje or "GPS sin FIX" in mensaje:
        return  

    match = re.search(r"ID:(\w+)\s*\|\s*Lat:\s*(-?\d+\.\d+)\s*\|\s*Lng:\s*(-?\d+\.\d+)\s*\|\s*SOS:(\d+)", mensaje)
    if match:
        codigo = match.group(1)
        lat = float(match.group(2))
        lng = float(match.group(3))
        sos = "SOS" if match.group(4) == "1" else "PING"
        print(f"Recibido de {codigo}: {lat}, {lng} Tipo: {sos}")
        try:
            r = requests.post("http://localhost/Kayakbutton/api/recibir_alerta.php", json={
                "codigo": codigo, "lat": lat, "lng": lng, "tipo": sos
            })
            print("Respuesta del servidor PHP:", r.json())
        except Exception as e:
            print("Error al enviar a PHP:", e)
    else:
        if mensaje.strip():
            print("Mensaje desconocido:", mensaje)

# ENVÍO (hacia los emisores)
@app.route("/api/enviar_mensaje", methods=["POST"])
def enviar_mensaje():
    data = request.json
    print("recibido en Flask:", data)

    destino = data.get("to")
    msg = data.get("msg")

    if not destino or not msg:
        return jsonify({"ok": False, "error": "Faltan datos"}), 400

    paquete = f"TO:{destino} | MSG:{msg}"
    puerto.write((paquete + "\n").encode("utf-8"))
    print("🚀 Mensaje enviado por LoRa:", paquete)

    return jsonify({"ok": True, "enviado": paquete})

# LOOP ESCUCHA
def iniciar_lector():
    def loop():
        global sistema_activo
        while True:
            if sistema_activo and puerto.in_waiting:
                mensaje = puerto.readline().decode("utf-8", errors="ignore").strip()
                if mensaje:
                    procesar_mensaje(mensaje)
    threading.Thread(target=loop, daemon=True).start()

# INTERFAZ TKINTER
def toggle_sistema():
    global sistema_activo
    sistema_activo = not sistema_activo
    estado.set("PRENDIDO" if sistema_activo else "APAGADO")
    boton.config(text="Apagar" if sistema_activo else "Prender")

# Crear ventana
ventana = tk.Tk()
ventana.title("Control KayakButton")
ventana.geometry("250x150")

estado = tk.StringVar(value="APAGADO")

tk.Label(ventana, text="Estado del sistema:", font=("Arial", 12)).pack(pady=10)
tk.Label(ventana, textvariable=estado, font=("Arial", 14, "bold")).pack(pady=5)

boton = tk.Button(ventana, text="Prender", font=("Arial", 12), command=toggle_sistema)
boton.pack(pady=10)

# MAIN
if __name__ == "__main__":
    iniciar_lector()
    threading.Thread(target=lambda: app.run(port=5000, debug=False, use_reloader=False), daemon=True).start()
    ventana.mainloop()

