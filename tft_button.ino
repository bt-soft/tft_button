#include <Streaming.h>

#include <TFT_eSPI.h> // TFT_eSPI könyvtár
TFT_eSPI tft = TFT_eSPI();

#include "TftButton.h"

String buttonLabel = "";
ButtonState_t buttonState;
void buttonsCallback(String label, ButtonState_t state) {
    buttonLabel = label;
    buttonState = state;
}

#define BUTTONS_X_START 10 // Gombok kezdő X koordinátája
#define BUTTON_W 60        // gomb szélesség
#define BUTTON_H 30        // gomb magasság
#define BUTTONS_Y 100      // Gombok fix Y pozíciója
#define BUTTONS_GAP 3      // A gombok közötti térköz pixelekben
#define BUTTON_X(n) (BUTTONS_X_START + (BUTTON_W + BUTTONS_GAP) * n)
TftButton buttons[] = {
#define BUTTON_X(n) (BUTTONS_X_START + (BUTTON_W + BUTTONS_GAP) * n)
    TftButton(&tft, BUTTON_X(0), BUTTONS_Y, BUTTON_W, BUTTON_H, "Push", ButtonType::PUSHABLE, buttonsCallback),
    TftButton(&tft, BUTTON_X(1), BUTTONS_Y, BUTTON_W, BUTTON_H, "Sw-1", ButtonType::TOGGLE, buttonsCallback),
    TftButton(&tft, BUTTON_X(2), BUTTONS_Y, BUTTON_W, BUTTON_H, "Sw-2", ButtonType::TOGGLE, buttonsCallback, ButtonState::ON),
    TftButton(&tft, BUTTON_X(3), BUTTONS_Y, BUTTON_W, BUTTON_H, "Dis", ButtonType::TOGGLE, buttonsCallback) // Disabled állapotú gomb
};

#include "PopUpDialog.h"

PopUpDialog *popUp = nullptr;
void popupCallback(String label, ButtonState_t state) {

    Serial << label << F(" dialóg gomb megnyomva, state: ") << TftButton::decodeState(state) << endl;

    // if (popUp != nullptr) {
    //     popUp->hide();   // Eltünteti a dialógust és visszaállítja a háttért
    //     delete popUp;    // Töröljük a dialógust
    //     popUp = nullptr; // Nullázzuk a mutatót, hogy elkerüljük a hibás hivatkozásokat
    // }
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
    buttons[3].setState(ButtonState::DISABLED); // A gomb alapértelmezés szerint le van tiltva
}

/**
 *
 */
void loop() {

    static uint16_t tx, ty;
    bool touched = tft.getTouch(&tx, &ty, 40); // A treshold értékét megnöveljük a default 20msec-ről 40-re
    // A gombok service metódusainak hívása
    for (TftButton &btn : buttons) {
        btn.handleTouch(touched, tx, ty);
    }

    if (buttonLabel.length()) {

        Serial << buttonLabel << F(" state changed to: ") << TftButton::decodeState(buttonState) << endl;

        if (buttonLabel.equals("Push")) {
            if (popUp == nullptr) {
                // Dialógus ablak létrehozása (tft, szélesség, magasság, üzenet, callback, okText, cancelText)
                popUp = new PopUpDialog(&tft, 300, 150, "Folytassuk?", popupCallback, "Igen", "Nem");
            }

            // Dialógus megjelenítése/elrejtése
            if (popUp->isVisible()) {
                popUp->hide();
            } else {
                popUp->show();
            }
        }

        buttonLabel = "";
    }

    if (popUp) {
        popUp->handleTouch(touched, tx, ty);
    }
}
