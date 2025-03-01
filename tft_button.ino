#include <Streaming.h>

#include <TFT_eSPI.h> // TFT_eSPI könyvtár
TFT_eSPI tft = TFT_eSPI();

#include "MultiButtonDialog.h"
#include "PopUpDialog.h"

PopupBase *dialog = nullptr;
const char *buttonLabel = nullptr;
volatile ButtonState_t buttonState;
void buttonCallback(const char *label, ButtonState_t state) {
    buttonLabel = label;
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
    TftButton(&tft, BUTTON_X(4), 100, 60, _BUTTON_HEIGHT, "Dis", ButtonType::TOGGLE, buttonCallback)};

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
    tft.fillScreen(TFT_BLACK);
    tft.setTextFont(2);

    // tft.setTextColor(TFT_WHITE);
    // tft.drawString("Nyisd meg a soros portot!", 0, 0);
    // while (!Serial) {
    // }
    // tft.fillScreen(TFT_BLACK);
    // Serial << "button1: " << button1.getState() << endl;

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
void loop() {

    // Touch esemény lekérdezése
    static uint16_t tx, ty;
    bool touched = tft.getTouch(&tx, &ty, 40); // A treshold értékét megnöveljük a default 20msec-ről 40-re

    // Ha van diakógus, akkor annak a gombjainak a touch esemányeit hívjuk
    if (dialog) {
        dialog->handleTouch(touched, tx, ty);

    } else if (!dialog and screenButtons) {
        // Ha nincs dialóg, de vannak képernyő gombok, akkor azok touch eseményeit hívjuk meg
        for (TftButton &screenButton : screenButtons) {
            screenButton.handleTouch(touched, tx, ty);
        }
    }

    // Nyomtak gombot?
    if (buttonLabel) {

        if (!dialog) {

            if (strcmp("Popup", buttonLabel) == 0) {
                Serial << "PopUpDialog::createDialog() start" << endl;
                PopUpDialog::createDialog(&dialog, &tft, 300, 150, F("Dialog title"), F("Folytassuk?"), buttonCallback, "Igen", "Lehet megse kellene");
                Serial << "PopUpDialog::createDialog() end" << endl;

            } else if (strcmp("Multi", buttonLabel) == 0) {
                Serial << "MultiButtonDialog::createDialog() start" << endl;
                constexpr uint16_t MULTI_BUTTON_W = 80;
                constexpr uint16_t MULTI_BUTTON_H = 30;
                constexpr uint16_t MULTI_BUTTON_SIZE = 17;
                TftButton *multiButtons[MULTI_BUTTON_SIZE] = {
                    new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "OK", ButtonType::PUSHABLE, buttonCallback),
                    new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Cancel", ButtonType::PUSHABLE, buttonCallback),
                    new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry-1", ButtonType::PUSHABLE, buttonCallback),
                    new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry-2", ButtonType::PUSHABLE, buttonCallback),
                    new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry-3", ButtonType::PUSHABLE, buttonCallback),
                    new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry-4", ButtonType::PUSHABLE, buttonCallback),
                    new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry-5", ButtonType::PUSHABLE, buttonCallback),
                    new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry-6", ButtonType::PUSHABLE, buttonCallback),
                    new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry-7", ButtonType::PUSHABLE, buttonCallback),
                    new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry-8", ButtonType::PUSHABLE, buttonCallback),
                    new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry-9", ButtonType::PUSHABLE, buttonCallback),
                    new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry-10", ButtonType::PUSHABLE, buttonCallback),
                    new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry-11", ButtonType::PUSHABLE, buttonCallback),
                    new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry-12", ButtonType::PUSHABLE, buttonCallback),
                    new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry-13", ButtonType::PUSHABLE, buttonCallback),
                    new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry-14", ButtonType::PUSHABLE, buttonCallback),
                    new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry-15", ButtonType::PUSHABLE, buttonCallback),
                };

                MultiButtonDialog::createDialog(&dialog, &tft, 400, 260, F("Valasszon opciot!"), nullptr, multiButtons, MULTI_BUTTON_SIZE);
                Serial << "MultiButtonDialog::createDialog() end" << endl;

            } else {
                Serial << "Nem ismerem a(z) '" << buttonLabel << "' eseményt" << endl;
            }

        } else {

            Serial << F("Dialóg button Label: '") << buttonLabel << F("' állapot változás: ") << TftButton::decodeState(buttonState) << endl;

            // Van dialog és megnyomtak rajta egy gombot -> Töröljük a dialógot
            delete dialog;
            dialog = nullptr;
        }

        // Töröljük a gombnyomás eseményét
        buttonLabel = nullptr;
    }
}
