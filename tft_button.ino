#include <Streaming.h>

#include <TFT_eSPI.h> // TFT_eSPI könyvtár
TFT_eSPI tft = TFT_eSPI();

#include "MultiButtonDialog.h"
#include "PopUpDialog.h"

PopupBase *dialog = nullptr;
const char *buttonLabel = nullptr;
ButtonState_t buttonState;
void buttonCallback(const char *label, ButtonState_t state) {
    static char savedLabel[50]; // Statikus változó a label tárolására
    strncpy(savedLabel, label, sizeof(savedLabel) - 1);
    savedLabel[sizeof(savedLabel) - 1] = '\0'; // Biztosítjuk, hogy a string null-terminált legyen
    buttonLabel = savedLabel;
    buttonState = state;
}

#define BUTTONS_X_START 10 // Gombok kezdő X koordinátája
#define _BUTTON_HEIGHT 30
#define _BUTTONS_GAP 10 // Define the gap between buttons
#define BUTTON_X(n) (BUTTONS_X_START + (60 + _BUTTONS_GAP) * n)
TftButton screenButtons[] = {
    TftButton(&tft, BUTTON_X(0), 100, 60, _BUTTON_HEIGHT, "Popup", ButtonType::PUSHABLE, buttonCallback),
    TftButton(&tft, BUTTON_X(1), 100, 60, _BUTTON_HEIGHT, "Multi", ButtonType::PUSHABLE, buttonCallback),
    TftButton(&tft, BUTTON_X(2), 100, 60, _BUTTON_HEIGHT, "Sw-1", ButtonType::TOGGLE, buttonCallback),
    TftButton(&tft, BUTTON_X(3), 100, 60, _BUTTON_HEIGHT, "Sw-2", ButtonType::TOGGLE, buttonCallback, ButtonState::ON),
    TftButton(&tft, BUTTON_X(4), 100, 60, _BUTTON_HEIGHT, "Dis", ButtonType::TOGGLE, buttonCallback) //
};

//---------------------------

/**
 *
 */
void drawScreen() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextFont(2);
    // Megjelenítjük a képernyő gombokat
    for (TftButton &screenButton : screenButtons) {
        screenButton.draw();
    }

    // Az 5.-et letiltjuk
    screenButtons[4].setState(ButtonState::DISABLED); // A gomb alapértelmezés szerint le van tiltva
}

/**
 *
 */
void setup() {
    Serial.begin(115200);

    tft.init();

    // Beállítjuk a touch screen-t
    uint16_t calData[5] = {213, 3717, 234, 3613, 7};
    tft.setTouch(calData);

    tft.setRotation(1);

    // tft.setTextColor(TFT_WHITE);
    // tft.drawString("Nyisd meg a soros portot!", 0, 0);
    // while (!Serial) {
    // }
    // tft.fillScreen(TFT_BLACK);
    // Serial << "button1: " << button1.getState() << endl;

    drawScreen();
}

/**
 *
 */
void loop() {
    try {
        // Touch esemény lekérdezése
        static uint16_t tx, ty;
        bool touched = tft.getTouch(&tx, &ty, 40); // A treshold értékét megnöveljük a default 20msec-ről 40-re

        // Ha van dialóg, akkor annak a gombjainak a touch eseményeit hívjuk
        if (dialog) {
            dialog->handleTouch(touched, tx, ty);
        } else {
            // Ha nincs dialóg, de vannak képernyő gombok, akkor azok touch eseményeit hívjuk meg
            for (TftButton &screenButton : screenButtons) {
                screenButton.handleTouch(touched, tx, ty);
            }
        }

        // Nyomtak gombot?
        if (buttonLabel) {

            if (!dialog) {

                if (strcmp("Popup", buttonLabel) == 0) {
                    dialog = PopUpDialog::createDialog(&tft, 300, 150, F("Dialog title"), F("Folytassuk?"), buttonCallback, "Igen", "Lehet megse kellene");

                } else if (strcmp("Multi", buttonLabel) == 0) {
#define MULTI_BUTTON_W 80
#define MULTI_BUTTON_H 30
#define MULTI_BUTTON_SIZE 17

                    const char *buttonLabels[MULTI_BUTTON_SIZE] = {
                        "OK", "Cancel", "Retry-1", "Retry-2", "Retry-3", "Retry-4", "Retry-5", "Retry-6",
                        "Retry-7", "Retry-8", "Retry-9", "Retry-10", "Retry-11", "Retry-12", "Retry-13", "Retry-14", "Retry-15"};
                    TftButton **multiButtons = new TftButton *[MULTI_BUTTON_SIZE];
                    for (int i = 0; i < MULTI_BUTTON_SIZE; i++) {
                        multiButtons[i] = new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, buttonLabels[i], ButtonType::PUSHABLE, buttonCallback);
                    }
                    dialog = MultiButtonDialog::createDialog(&tft, 400, 260, F("Valasszon opciot!"), multiButtons, MULTI_BUTTON_SIZE);

                } else {
                    Serial << "Nem ismerem a(z) '" << buttonLabel << "' eseményt" << endl;
                }

            } else {

                // Van dialog és megnyomtak rajta egy gombot -> Töröljük a dialógot
                Serial << F("Dialóg button Label: '") << buttonLabel << F("' állapot változás: ") << TftButton::decodeState(buttonState) << endl;
                if (dialog) {
                    delete dialog;
                    dialog = nullptr;
                }
                drawScreen();
            }

            // Töröljük a gombnyomás eseményét
            buttonLabel = nullptr;
        }
    } catch (...) {
        Serial << "Hiba történt!" << endl;
        buttonLabel = nullptr;
    }
}
