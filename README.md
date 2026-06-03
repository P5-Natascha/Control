# Control
Diese Repo ist teil eines P5-Projektes am Beruflichen Gymnasium Technik

## Dokumentation:
Dieser Code gehört zu einer ESP 32-Fernbedienung um ein [Auto](https://github.com/P5-Natascha/Captain) zu steuern.
Außerdem ist die Fernbedienung mit einer eigenen Debug Shell ausgestattet.

* Aufbau:
    * main.cpp: Globale Funktionen und setup/loop
    * config.cpp: Config laden über Preferences.h
    * shell.cpp: Shell für config, debug, alles eigentlich
    * communication.cpp: Alle Comms Zusammengeführt, Differenzierung zwischen WIFI und BLUETOOTH
        * networkMgr.cpp: WiFi Comms
        * bluetooth.cpp: Bluetooth Comms *WIP*
    * DisplayManager.cpp: Menüs, alles was auf dem Display angezeigt wird
    * menuHandler.cpp: Steuerung; Joystick eingabe, Aktionsverarbeitung
    * global.h: Globale Variablen/Funktionen
    * setupwizard.cpp: Setup Menü (WIFI/Bluetooth auswählen, Netzwerkprofil auswählen)

* Verkabelung:
![Control Verkabelung](/Control_Verkabelung.png)
