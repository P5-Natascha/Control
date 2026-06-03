#include <global.h>
#include <network.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <joystick.h>
#include <config.h>
#include <DisplayMgr.h>
#include <communication.h>

NetworkMgr::NetworkMgr() {}

String SDATA = "";
float battValue = 0;
float lenkungValue = 0;
float ampereValue = 0;
unsigned int abstandVorne = 0;
unsigned int abstandHinten = 0;


bool NetworkMgr::begin() { // Verbindung herstellen
    WiFi.setHostname(Device_Name.c_str());
    logging.debug("WiFi: Using Credentials: "); logging.debug("SSID:" + WiFi_SSID + "; Pass: " + WiFi_Pass + "; Current Hostname: " + String(WiFi.getHostname()));
    WiFi.begin(WiFi_SSID.c_str(),WiFi_Pass.c_str());

    unsigned long start = millis();
    logging.debug("Waiting for connection at: " + String(start));
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        if(millis() - start > 20000) {
            logging.error("WiFi Connection timed out!"); logging.error("Waited for: " + String(millis() - start));
            return false;
        }
    }

    logging.debug("WiFi connected. IP: " + WiFi.localIP().toString());
    return true;
}

void NetworkMgr::disconnect() {
    logging.debug("WiFi disconnected");
    WiFi.disconnect();
}

String NetworkMgr::getStatus() { // Detaillierter Status im STring wird zurückgegeben 
    switch (WiFi.status()) { 
        case WL_CONNECTED:  {     
        String info = "Verbunden!";
        info += "\n SSID: " + WiFi.SSID();
        info += "\n IP: " + WiFi.localIP().toString();
        info += "\n Signal: " + String(WiFi.RSSI()) + "dBm";
        info += "\n Hostname: " + String(WiFi.getHostname());
        info += "\n Mac Address: " + String(WiFi.macAddress());
        return info;
        }
        case WL_NO_SSID_AVAIL:   return "SSID nicht gefunden (WL_NO_SSID_AVAIL)";
        case WL_CONNECT_FAILED:  return "Verbindung fehlgeschlagen (WL_CONNECT_FAILED)";
        case WL_IDLE_STATUS:     return "Leerlauf (WL_IDLE_STATUS)";
        case WL_DISCONNECTED:    return "Getrennt (WL_DISCONNECTED)";
        default:                 return "Unbekannter Status (" + String(WiFi.status()) + ")";
    }
}

bool NetworkMgr::isConnected() { // Verbindungsaktivität wird in boolean zurückgegeben
    if (WiFi.status() == WL_CONNECTED) {
        return true;
    } else {
        return false;
    }
}

String NetworkMgr::getMacAddress() {
    return ("Mac Address: " + String(WiFi.macAddress()));
}

void NetworkMgr::_connectTCP() {
    logging.debug("Baue TCP Verbindung auf");
    _tcp.connect(Target_IP.c_str(), tcp_Target_Port);
    unsigned long start = millis();
    while(!_tcp.connected()) {
        delay(10);
        if(millis() - start > 5000) {
            Serial.println("Unable to connect TCP! Aborting...");
            return;
        }
    }
    if(_tcp.connected()) {
        delay(500);
    }
}

void NetworkMgr::_checkTCP() {
    if(!_tcp.connected()) {
        _connectTCP();
    }
    if(!_tcp.connected()) {
        currentCtrlMode = OFF;
        showStatus("TCP Disconnected");
        removeStatus(3);
    }
}

void NetworkMgr::sendTCP(String type, String value) {
    _checkTCP();
    String msg = type + ":" + value + "\r\n";
    _tcp.write(msg.c_str(), msg.length());
    logging.debug("Sende TCP. type: " + type + "; value: " + value);
}

void NetworkMgr::sendTCP(String type, int value) {
    _checkTCP();
    String msg = type + ":" + value + "\r\n";
    _tcp.write(msg.c_str(), msg.length());
    logging.debug("Sending TCP. type: " + type + "; value: " + value);
}

void NetworkMgr::updateTCP() { // An Empfänger den aktuellen Modus senden
    sendTCP("mode",currentCtrlMode);
}

void NetworkMgr::sendUDP(ControlPacket packet) {
    _udp.beginPacket(Target_IP.c_str(),udp_Target_Port);
    _udp.write((uint8_t*)&packet, sizeof(packet));
    _udp.endPacket();
}

void NetworkMgr::sendUDP(String value) {
    _udp.beginPacket(Target_IP.c_str(),udp_Target_Port);
    _udp.write((const uint8_t*)value.c_str(), value.length());
    _udp.endPacket();
}
void NetworkMgr::sendMovement(JoystickRaw raw) {
    ControlPacket packet = {(uint16_t)raw.x, (uint16_t)raw.y};
    sendUDP(packet);
}

void NetworkMgr::handleIncomingTCP() { // Sensordaten vom Auto
    _checkTCP();
    while(_tcp.available()) {
        String line = _tcp.readStringUntil('\n');
        line.trim();
        if(line.startsWith("SDATA:")) {
            SDATA = line.substring(6);
            if(SDATA.startsWith("BATT")) {
                battValue = SDATA.substring(5).toFloat();
                logging.debug("Received BATT: " + String(battValue));
            } else if (SDATA.startsWith("LENK")) {
                lenkungValue = SDATA.substring(5).toFloat();
                logging.debug("Received LENK: " + String(lenkungValue));
            } else if (SDATA.startsWith("AMPR")) {
                ampereValue = SDATA.substring(5).toFloat();
                logging.debug("Received AMPR: " + String(ampereValue));
            } else if (SDATA.startsWith("ABSV")) {
                abstandVorne = SDATA.substring(5).toInt();
                logging.debug("Received ABSV: " + String(abstandVorne));
            } else if (SDATA.startsWith("ABSH")) {
                abstandHinten = SDATA.substring(5).toInt();
                logging.debug("Received ABSH: " + String(abstandHinten));
            } else {
                logging.error("Received unknown SDATA: " + SDATA);
            }
        }
    }
}

String NetworkMgr::handleRawTCP() {
    String line = "No TCP available.";
    _checkTCP();
    if(_tcp.available()) {
        line = _tcp.readString();
        line.trim();
    }
    return line;
}