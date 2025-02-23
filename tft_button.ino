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

TftButton button1(&tft, 50, 10, 60, 30, "Push", ButtonType::PUSHABLE, button1Callback);
TftButton button2(&tft, 50, 80, 60, 30, "Sw-1", ButtonType::TOGGLE, button2Callback);
TftButton button3(&tft, 50, 150, 60, 30, "Sw-2", ButtonType::TOGGLE, button3Callback, ButtonState::ON);
TftButton button4(&tft, 50, 220, 60, 30, "Dis", ButtonType::TOGGLE, button4Callback); // Disabled állapotú gomb

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

    button1.draw();
    button2.draw();
    button3.draw();
    button4.draw();
    button4.setState(ButtonState::DISABLED); // A gomb alapértelmezés szerint le van tiltva
}

/**
 *
 */
void loop() {

    static uint16_t tx, ty;
    bool touched = tft.getTouch(&tx, &ty, 40); // A treshold értékét megnöveljük a default 20msec-ről
    // A gombok service metódusainak hívás
    button1.service(touched, tx, ty);
    button2.service(touched, tx, ty);
    button3.service(touched, tx, ty);
    button4.service(touched, tx, ty);
}
