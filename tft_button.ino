#include <Streaming.h>

#include <TFT_eSPI.h> // TFT_eSPI könyvtár
TFT_eSPI tft = TFT_eSPI();

#include "MultiButtonDialog.h"
#include "PopUpDialog.h"

PopupBase *dialog = nullptr;
void popupCallback(String label, ButtonState_t state) {

    Serial << label << F(" dialóg gomb megnyomva, state: ") << TftButton::decodeState(state) << endl;

    if (dialog != nullptr) {
        Serial << "delete dialog; start" << endl;
        delete dialog;    // Töröljük a dialógust: eltünteti a dialógust és visszaállítja a háttért
        dialog = nullptr; // Nullázzuk a mutatót, hogy elkerüljük a hibás hivatkozásokat
        Serial << "delete dialog; end" << endl;
    }
}

String buttonLabel = "";
ButtonState_t buttonState;
void buttonsCallback(String label, ButtonState_t state) {
    // Ha van dialóg, ÉS látszik akkor nem megyünk itt tovább
    if (dialog and dialog->isVisible()) {
        return;
    }
    buttonLabel = label;
    buttonState = state;
}

#define BUTTONS_X_START 10 // Gombok kezdő X koordinátája
#define _BUTTON_HEIGHT 30
#define _BUTTONS_GAP 10 // Define the gap between buttons
#define BUTTON_X(n) (BUTTONS_X_START + (60 + _BUTTONS_GAP) * n)
TftButton buttons[] = {
    TftButton(&tft, BUTTON_X(0), 100, 60, _BUTTON_HEIGHT, "Popup", ButtonType::PUSHABLE, buttonsCallback),
    TftButton(&tft, BUTTON_X(1), 100, 60, _BUTTON_HEIGHT, "Multi", ButtonType::PUSHABLE, buttonsCallback),
    TftButton(&tft, BUTTON_X(2), 100, 60, _BUTTON_HEIGHT, "Sw-1", ButtonType::TOGGLE, buttonsCallback),
    TftButton(&tft, BUTTON_X(3), 100, 60, _BUTTON_HEIGHT, "Sw-2", ButtonType::TOGGLE, buttonsCallback, ButtonState::ON),
    TftButton(&tft, BUTTON_X(4), 100, 60, _BUTTON_HEIGHT, "Dis", ButtonType::TOGGLE, buttonsCallback)};

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

    for (TftButton &btn : buttons) {
        btn.draw();
    }
    buttons[4].setState(ButtonState::DISABLED); // A gomb alapértelmezés szerint le van tiltva
}

/**
 *
 */
void loop() {

    static uint16_t tx, ty;
    bool touched = tft.getTouch(&tx, &ty, 40); // A treshold értékét megnöveljük a default 20msec-ről 40-re
    // A gombok service metódusainak hívása, de csak ha nincs dialog
    if (!dialog or !dialog->isVisible()) {
        for (TftButton &btn : buttons) {
            btn.handleTouch(touched, tx, ty);
        }
    }

    if (buttonLabel.length()) {

        Serial << F("'") << buttonLabel << F("' state changed to: ") << TftButton::decodeState(buttonState) << endl;

        if (buttonLabel.equals("Popup") or buttonLabel.equals("Multi")) {

            Serial << "buttonLabel: " << buttonLabel << endl;

            if (dialog == nullptr) {
                if (buttonLabel.equals("Popup")) {
                    // Dialógus ablak létrehozása (tft, szélesség, magasság, üzenet, callback, okText, cancelText)
                    Serial << "PopUpDialog::createDialog()" << endl;
                    PopUpDialog::createDialog(&dialog, &tft, 300, 150, F("Dialog title"), F("Folytassuk?"), popupCallback, "Igen", "Lehet megse kellene");
                } else if (buttonLabel.equals("Multi")) {

                    constexpr uint16_t MULTI_BUTTON_W = 80;
                    constexpr uint16_t MULTI_BUTTON_H = 30;
                    constexpr uint16_t MULTI_BUTTON_SIZE = 17;
                    TftButton **buttons = new TftButton *[MULTI_BUTTON_SIZE] {
                        new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "OK", ButtonType::PUSHABLE, popupCallback),
                            new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Cancel", ButtonType::PUSHABLE, popupCallback),
                            new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry", ButtonType::PUSHABLE, popupCallback),
                            new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry", ButtonType::PUSHABLE, popupCallback),
                            new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry", ButtonType::PUSHABLE, popupCallback),
                            new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry", ButtonType::PUSHABLE, popupCallback),
                            new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry", ButtonType::PUSHABLE, popupCallback),
                            new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry", ButtonType::PUSHABLE, popupCallback),
                            new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry", ButtonType::PUSHABLE, popupCallback),
                            new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry", ButtonType::PUSHABLE, popupCallback),
                            new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry", ButtonType::PUSHABLE, popupCallback),
                            new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry", ButtonType::PUSHABLE, popupCallback),
                            new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry", ButtonType::PUSHABLE, popupCallback),
                            new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry", ButtonType::PUSHABLE, popupCallback),
                            new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry", ButtonType::PUSHABLE, popupCallback),
                            new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry", ButtonType::PUSHABLE, popupCallback),
                            new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, "Retry", ButtonType::PUSHABLE, popupCallback),
                    };

                    Serial << "MultiButtonDialog::createDialog() start" << endl;
                    MultiButtonDialog::createDialog(&dialog, &tft, 400, 250, F("Title"), F("Valasszon opciot!"), buttons, MULTI_BUTTON_SIZE);
                    Serial << "MultiButtonDialog::createDialog() end" << endl;
                } else {
                    Serial << "Nem ismerem a(z) '" << buttonLabel << "' eseményt" << endl;
                }
            }

            // Dialógus megjelenítése/elrejtése
            if (dialog) {
                if (dialog->isVisible()) {
                    dialog->hide();
                } else {
                    dialog->show();
                }
            }
        }

        buttonLabel = "";
    }

    if (dialog) {
        dialog->handleTouch(touched, tx, ty);
    }
}
