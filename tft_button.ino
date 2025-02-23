#include <Streaming.h>

#include <TFT_eSPI.h> // TFT_eSPI könyvtár
TFT_eSPI tft = TFT_eSPI();

#include "TftButton.h"

void button1Callback(ButtonState_t state) {
    Serial << F("Button1 state changed to: ") << TftButton::decodeState(state) << endl;
}

void button2Callback(ButtonState_t state) {
    Serial << F("Button2 state changed to: ") << TftButton::decodeState(state) << endl;
}

void button3Callback(ButtonState_t state) {
    Serial << F("Button3 state changed to: ") << TftButton::decodeState(state) << endl;
}

void button4Callback(ButtonState_t state) {
    Serial << F("Button4 state changed to: ") << TftButton::decodeState(state) << endl;
}

#define BUTTONS_X_START 10 // Gombok kezdő X koordinátája
#define BUTTON_W 60        // gomb szélesség
#define BUTTON_H 30        // gomb magasság
#define BUTTONS_Y 100      // Gombok fix Y pozíciója
#define BUTTONS_GAP 3      // A gombok közötti térköz pixelekben
#define BUTTON_X(n) (BUTTONS_X_START + (BUTTON_W + BUTTONS_GAP) * n)
TftButton buttons[] = {
#define BUTTON_X(n) (BUTTONS_X_START + (BUTTON_W + BUTTONS_GAP) * n)
    TftButton(&tft, BUTTON_X(0), BUTTONS_Y, BUTTON_W, BUTTON_H, "Push", ButtonType::PUSHABLE, button1Callback),
    TftButton(&tft, BUTTON_X(1), BUTTONS_Y, BUTTON_W, BUTTON_H, "Sw-1", ButtonType::TOGGLE, button2Callback),
    TftButton(&tft, BUTTON_X(2), BUTTONS_Y, BUTTON_W, BUTTON_H, "Sw-2", ButtonType::TOGGLE, button3Callback, ButtonState::ON),
    TftButton(&tft, BUTTON_X(3), BUTTONS_Y, BUTTON_W, BUTTON_H, "Dis", ButtonType::TOGGLE, button4Callback) // Disabled állapotú gomb
};

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
    bool touched = tft.getTouch(&tx, &ty, 40); // A treshold értékét megnöveljük a default 20msec-ről
    // A gombok service metódusainak hívása
    for (TftButton &btn : buttons) {
        btn.service(touched, tx, ty);
    }
}
