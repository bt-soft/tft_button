#include <Streaming.h>

#include <TFT_eSPI.h> // TFT_eSPI könyvtár
TFT_eSPI tft;

#include "MultiButtonDialog.h"
#include "PopUpDialog.h"

/// @brief dialógus pointer
PopupBase *dialog = nullptr;
/// @brief Megnyomott gomb label
const char *buttonLabel = nullptr;
/// @brief Megnyomott gomb állapota
ButtonState_t buttonState;

/// @brief Gombok callback
/// A megnyomott gomb visszaadja a label-jét és az állapotát
/// @param label megnyomott gomb label
/// @param state megnyomott gomb állapota
void buttonCallback(const char *label, ButtonState_t state) {
    buttonLabel = label;
    buttonState = state;
}

#define SCREEN_BUTTONS_X_START 10 // Gombok kezdő X koordinátája
#define SCREEN_BUTTON_HEIGHT 30
#define SCREEN_BUTTONS_GAP 10 // Define the gap between buttons
#define BUTTON_X(n) (SCREEN_BUTTONS_X_START + (60 + SCREEN_BUTTONS_GAP) * n)
TftButton screenButtons[] = {
    TftButton(&tft, BUTTON_X(0), 100, 60, SCREEN_BUTTON_HEIGHT, F("Popup"), ButtonType::PUSHABLE, buttonCallback),
    TftButton(&tft, BUTTON_X(1), 100, 60, SCREEN_BUTTON_HEIGHT, F("Multi"), ButtonType::PUSHABLE, buttonCallback),
    TftButton(&tft, BUTTON_X(2), 100, 60, SCREEN_BUTTON_HEIGHT, F("Sw-1"), ButtonType::TOGGLE, buttonCallback),
    TftButton(&tft, BUTTON_X(3), 100, 60, SCREEN_BUTTON_HEIGHT, F("Sw-2"), ButtonType::TOGGLE, buttonCallback, ButtonState::ON),
    TftButton(&tft, BUTTON_X(4), 100, 60, SCREEN_BUTTON_HEIGHT, F("Dis"), ButtonType::TOGGLE, buttonCallback) //
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
void createPopupDialog() {
    dialog = PopUpDialog::createDialog(&tft, 300, 150, F("Dialog title"), F("Folytassuk?"), buttonCallback, "Igen", "Lehet megse kellene");
}

/**
 *
 */
void createMultiButtonDialog(const char *buttonLabels[], int buttonsCount) {
#define MULTI_BUTTON_W 80
#define MULTI_BUTTON_H 30

    TftButton **multiButtons = new TftButton *[buttonsCount];
    for (uint8_t i = 0; i < buttonsCount; i++) {
        multiButtons[i] = new TftButton(&tft, MULTI_BUTTON_W, MULTI_BUTTON_H, buttonLabels[i], ButtonType::PUSHABLE, buttonCallback);
    }
    dialog = MultiButtonDialog::createDialog(&tft, 400, 260, F("Valasszon opciot!"), multiButtons, buttonsCount);
}

/**
 *
 */
void handleButtonPress() {
    if (strcmp("Popup", buttonLabel) == 0) {
        createPopupDialog();

    } else if (strcmp("Multi", buttonLabel) == 0) {

        const __FlashStringHelper *buttonLabels[] = {
            F("OK"), F("Cancel"), F("Retry-1"), F("Retry-2"), F("Retry-3"), F("Retry-4"), F("Retry-5"), F("Retry-6"),
            F("Retry-7"), F("Retry-8"), F("Retry-9"), F("Retry-10"), F("Retry-11"), F("Retry-12"), F("Retry-13"), F("Retry-14"), F("Retry-15")};

        createMultiButtonDialog(reinterpret_cast<const char **>(buttonLabels), ARRAY_ITEM_COUNT(buttonLabels));

    } else {
        Serial << F("Screen button Label: '") << buttonLabel << F("' állapot változás: ") << TftButton::decodeState(buttonState) << endl;
    }
}

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
                handleButtonPress();
            } else {
                // Van dialog és megnyomtak rajta egy gombot -> Töröljük a dialógot
                Serial << F("Dialóg button Label: '") << buttonLabel << F("' állapot változás: ") << TftButton::decodeState(buttonState) << endl;
                delete dialog;
                dialog = nullptr;
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
