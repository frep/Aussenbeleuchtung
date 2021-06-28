# Aussenbeleuchtung
Die Aussenbeleuchtung besteht aus 7 LED - Streifen, welche eingeschaltet werden sollen, sobald eine Bewegung bei einem Bewegungssensor festgestellt wurde.

<p align="center">
<img src=docs/Overview.png width=80%/>
</p>

Die Logik über die Ansteuerung der LED-Streifen findet auf dem Raspberry Pi (RPi) in einem Python-Skript statt, welches als Service automatisch nach dem Aufstarten ausgeführt wird. Die Kommunikation zwischen dem RPi und den Streifen erfolgt über MQTT. Der dazu benötigte Broker, wird ebenfalls auf dem RPi ausgeführt, welcher ebenfalls automatisch nach dem Einschalten gestartet wird.

Pro LED-Streifen gibt es einen Mikrokontroller (Esp32 doit devkit v1), welcher die Kommunikation via MQTT sicherstellt und weiters den LED-Streifen ansteuert und die Bewegungssensoren (0-3 Sensoren pro Bord) auswerten. Die Programmierung des Mikrokontrollers erfolgte mittels der [PlatformIO-IDE](https://platformio.org/) innerhalb von Visual Studio Code (VSCode).

<p align="center">
<img src=docs/ESP32-DOIT-DEVKIT-V1.jpeg width=80%/>
</p>

## Setup - RPi
Es wird keine spezielle Linux-Distribution benötigt. Die folgenden Anweisungen beziehen sich jedoch auf ein Debian Linux, wie dies bei der Standard-Distribution beim RPi der Fall ist. Es muss basierend auf der Standard-Installation nun noch der MQTT-Broker, wie auch der LED-Controller installiert werden:

### MQTT
Der MQTT Broker wird wie folgt installiert und als Service an das Aufstarten angehängt:
>     sudo apt-get update
>     sudo apt-get install -y mosquitto mosquitto-clients
>     sudo systemctl enable mosquitto.service

### Python setup
Das Python skript ledController.py benötigt die MQTT-Bibliotheken, welche mit den folgenden Kommandos installiert werden können:
>     sudo apt-get install -y python-pip
>     sudo pip install paho-mqtt

### Python script als Service starten
Die Datei ledController.py in den Ordner /user/bin verschieben:
> 		sudo mv ledController.py /usr/bin/ledController.py

Sicherstellen, dass das Skript ausführbar ist: 
> 		sudo chmod +x /usr/bin/ledController.py

Die Datei ledController.service in den folgenden Ordner verschieben:

> 		sudo mv ledController.service /lib/systemd/system/ledController.service

Um die Änderungen von systemctl zu laden und den neuen Service dem Autostart hinzuzufügen, werden die folgenden Kommandos benötigt:

> 		sudo systemctl daemon-reload
> 		sudo systemctl enable ledController.service

Um zu kontrollieren, dass der Service wie gewünscht funktioniert, die folgenden Kommandos ausführen:

> 		sudo systemctl start ledController.service
> 		sudo systemctl status ledController.service

Der Service sollte nun als **active (running)** aufgeführt sein.


## Setup - Esp32
Nach dem ersten Aufstarten, kennt der Mikrokontroller die WiFi Zugangsdaten noch nicht, und kann sich somit nicht verbinden. Kommt keine solche Verbindung zu stande, wird vom Mikrokontroller ein Ad-Hoc-Netzwerk erstellt. Man kann sich daher mit dem offenen Netzwerk AutoConnectAP verbinden und die Konfiguration der WiFi-Zugangsdaten durchführen. Nach dem Verbinden mit AutoConnectAP erscheint der folgende Dialog:

<p align="center">
<img src=docs/AutoConnectAP.png width=80%/>
</p>

Unter **Configure WiFi** das richtige Netzwerk aussuchen und das entsprechende Passwort eingeben. Von nun an, korrekte Daten vorrausgesetzt, verbindet sich der Mikrokontroller automatisch mit dem Netzwerk, und der AP AutoConnectAP wird nicht mehr aufgebaut.

Nachdem der Kontroller mit dem WiFi verbunden ist, kann dieser nun noch fertig konfiguriert werden. Hierzu im Browser die folgende Seite aufrufen:

>     http://stripe0.local

Es erscheint ein Konfigurierungsdialog wie folgt:

<p align="center">
<img src=docs/EspKonfiguration.png width=80%/>
</p>

Nachdem die Client ID gesetzt und gespeichert wurde, kann die Seite unter der entsprechenden ID aufgerufen werden. D.h. für Client ID = 1 ist die Konfigurationseite erreichbar unter: http://stripe1.local

## OTA
Um den Kontroller aktualisieren zu können, kann auf die OTA (Over-The-Air) Programmierung zurückgegriffen werden. Der Update-Server kann (am Beispiel der Client-ID = 1) erreicht werden über:
>     http://stripe1.local/update

Nach dem Kompilieren in der PlatformIO IDE, die Datei **firmware.bin**, welche aktualiesiert werden muss, kann am folgenden Ort gefunden werden (für die MAC Umgebung fetheresp32-OSX):
>     .pio/build/featheresp32-OSX/firmware.bin
Falls das unsichtbare Verzeichniss **.pio** nicht angezeigt wird unter macOS, dann kann dies geändert werden durch drücken von **Command + Shift + . (Punkt)**. 