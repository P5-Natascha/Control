#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <global.h>
#include <DisplayMgr.h>

const String mainMenuTitle = "Fernsteuerung   "; // Immer bis 16 Zeichen auffüllen!
unsigned long stayUntil;
bool statusDisplaying = false;
bool prevBacklightState = true;

// LCD Config
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C Addresse, 16x2 Characters Init.

void initDisplay() {
    lcd.init();
    lcd.backlight();
    lcd.clear();
}

void InactivityHandler() {
    if(millis() - lastInteraction > inactivityTimeout) {
        displayBacklightState = false;
        lcd.noBacklight();
        prevBacklightState = false;
    } else if (prevBacklightState == false) {
        lcd.backlight();
        prevBacklightState = true;
        displayBacklightState = true;
    }
}

void displayTitle() {
    lcd.setCursor(0, 0);
    lcd.print(mainMenuTitle);
    lcd.setCursor(0, 1);
}

void displayMode(String title) {
    lcd.setCursor(0, 0);
    if(title.length() < 16) {
        int add = 16 - title.length();
        for (int i = 0; i < add; i++) {
            title += " ";
        }
    }
    lcd.print(title);
    lcd.setCursor(0, 1);
}

void changeBottomDisplay(String content) {
    lcd.setCursor(0, 1);
    if(content.length() < 16) {
        int add = 16 - content.length();
        for (int i = 0; i < add ; i++) {
            content += " ";
        }
    }
    content = content.substring(0, 16);
    lcd.print(content);
}

void allSDATAValuesMenu(int page) { // Ja, ist nicht so schön aber ich hab Zeitstress
    String bottomContent = "";
    switch(page) {
        case 1: {
            bottomContent = "Bat:" + String(battValue) + "V; " + String(ampereValue) + "A";
            break;
        } case 2: {
            bottomContent = "Lenkung: " + String(lenkungValue);
            break;
        } case 3: {
            bottomContent = "Abstand V: " + String(abstandVorne);
            break;
        } case 4: {
            bottomContent = "Abstand H: " + String(abstandHinten);
            break;
        }
    }
    changeBottomDisplay(bottomContent);
}

void updateDisplay() { // Alle Menüs
    if(stayUntil  > millis()||statusDisplaying){
        logging.debug("Update Display: Passive because of showStatus." + String(stayUntil));
        return;
    }
    String text;
    switch(currentOpMode) {
        case SETUP: {
            displayMode("Initialisiere...");
            break;
        }
        case SHELL: {
            displayMode("In Shell");
            break;
        }
        case REGULAR:
            switch(currentCtrlMode) {
                case OFF:
                    displayTitle();
                    switch(currentMenuOption) {
                        case M_MANUAL:
                            changeBottomDisplay("Manuell");
                            break;
                        case M_HAUTO:
                            changeBottomDisplay("Halb-Automatik");
                            break;
                        case M_AUTO:
                            changeBottomDisplay("Automatik");
                            break;
                        case M_INFO:
                            changeBottomDisplay("Informationen");
                            break;
                        case M_SETTINGS:
                            changeBottomDisplay("Settings");
                            break;
                    }
                    InactivityHandler();
                    break;
                case MANUAL: {
                    displayMode("Manuell");
                    String bottomContent = "Bat:" + String(battValue) + "V " + String(ampereValue) + "A";
                    changeBottomDisplay(bottomContent);
                    break;
                }
                case HAUTO: {
                    displayMode("Halb-Automatik");
                    String bottomContent = "Bat:" + String(battValue) + "V " + String(ampereValue) + "A";
                    changeBottomDisplay(bottomContent);
                    break;
                }
                case AUTO: {
                    displayMode("Automatik"); 
                    break;
                }
                case INFO: {
                    displayMode("Informationen");
                    break;
                }
            }
            break;
    }
}

void showStatus(String content) { // Temporärer Statustext
    lcd.clear();
    lcd.setCursor(0,0);
    String line1 = "";
    String line2 = "";
    if (content.length() > 16) {
        line1 = content.substring(0,16);
        line2 = content.substring(16,32);
    } else {
        line1 = content;
        line2 = "";
    }
    lcd.print(line1);
    lcd.setCursor(0,1);
    lcd.print(line2);
    statusDisplaying = true;
}

void removeStatus(int time) {
    if(time == 0){
        lcd.clear();
    } else {
        time = time * 1000;
        stayUntil = millis() + time;
    }
    statusDisplaying = false;
    updateDisplay();
}

void displaySetBacklight(bool value) {
    if(value) {
        lcd.backlight();
    } else {
        lcd.noBacklight();
    }
}
void clearDisplay() {
    lcd.clear();
}

void showCustomMenu(CustomMenu menu) {
    if(menu.title.length() > 16) {
        menu.title = menu.title.substring(0,16);
    }
    if(menu.value.length() > 16) {
        menu.value = menu.value.substring(0,16);
    }
    if(menu.title.length() < 16) {
        for(int i = 0; menu.title.length()<16; i++) {
            menu.title += " ";
        }
    }
    if (menu.value.length() < 16) {
        for (int i = 0; menu.value.length()<16; i++) {
            menu.value += " ";
        }
    }
    lcd.setCursor(0,0);
    lcd.print(menu.title);
    lcd.setCursor(0,1);
    lcd.print(menu.value);
}