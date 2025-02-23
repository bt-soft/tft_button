#ifndef __TFT_BUTTON_H
#define __TFT_BUTTON_H

#include "ESP_free_fonts.h"

#define TFT_COLOR(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))
#define COMPLEMENT_COLOR(color) \
    (TFT_COLOR((255 - ((color >> 16) & 0xFF)), (255 - ((color >> 8) & 0xFF)), (255 - (color & 0xFF))))
#define PUSHED_COLOR(color) ((((color & 0xF800) >> 1) & 0xF800) | (((color & 0x07E0) >> 1) & 0x07E0) | (((color & 0x001F) >> 1) & 0x001F))

// Gomb állapotai
typedef enum ButtonState_t {
    OFF,
    ON,
    DISABLED,
    //---- technikai állapot
    HOLD,
    PUSHED // csak az esemény jelzésére a calbback függvénynek, nincs színhez kötve az állapota
} ButtonState;

typedef enum ButtonType_t {
    TOGGLE,
    PUSHABLE
} ButtonType;

// Callback típusa
typedef void (*ButtonCallback)(ButtonState_t);

/**
 *
 */
class TftButton {

private:
    TFT_eSPI *pTft;
    int x, y, w, h;
    String label;
    ButtonState state;
    ButtonState oldState;
    ButtonType type;
    ButtonCallback callback;
    uint16_t colors[3] = {TFT_COLOR(65, 65, 114), TFT_COLOR(65, 65, 114) /*TFT_DARKGREEN*/, TFT_COLOR(65, 65, 65)};
    bool buttonPressed; // Flag a gomb nyomva tartásának követésére

    /**
     *
     */
    void pressed() {
        buttonPressed = true;
        oldState = state;
        state = HOLD;
        draw();
    }

    /**
     *
     */
    void released() {
        buttonPressed = false;
        if (type == TOGGLE) {
            state = (oldState == OFF) ? ON : OFF;
        } else {
            state = OFF;
        }
        oldState = state;

        draw();
        if (callback) {
            callback(type == PUSHABLE ? PUSHED : state);
        }
    }

    /**
     * A gombot nyomták meg?
     */
    bool contains(int tx, int ty) {
        return (tx >= x && tx <= x + w && ty >= y && ty <= y + h);
    }

public:
    /**
     *
     */
    TftButton(TFT_eSPI *pTft, int x, int y, int w, int h, String label, ButtonType type, ButtonCallback callback, ButtonState state = OFF)
        : pTft(pTft), x(x), y(y), w(w), h(h), label(label), type(type), callback(callback), buttonPressed(false) {

        this->state = this->oldState = state;
    }

    /**
     *
     */
    uint16_t darkenColor(uint16_t color, uint8_t amount) {
        uint8_t r = (color & 0xF800) >> 11;
        uint8_t g = (color & 0x07E0) >> 5;
        uint8_t b = (color & 0x001F);

        r = max(0, r - (amount >> 3)); // 5 bites piros csökkentés
        g = max(0, g - (amount >> 2)); // 6 bites zöld csökkentés
        b = max(0, b - (amount >> 3)); // 5 bites kék csökkentés

        return (r << 11) | (g << 5) | b;
    }

    /**
     * Kirajzolás
     */
    void draw() {

        if (buttonPressed) {
            // A gomb teljes szélességét és magasságát kihasználó sötétedés
            uint8_t steps = 6; // Több lépés, erősebb hatás
            uint8_t stepWidth = w / steps;
            uint8_t stepHeight = h / steps;
            for (uint8_t i = 0; i < steps; i++) {
                uint16_t fadedColor = darkenColor(colors[oldState], i * 30); // Erősebb sötétítés
                pTft->fillRoundRect(x + i * stepWidth / 2, y + i * stepHeight / 2, w - i * stepWidth, h - i * stepHeight, 5, fadedColor);
            }
        } else {
            pTft->fillRoundRect(x, y, w, h, 5, colors[state]);
        }

        pTft->drawRoundRect(x, y, w, h, 5, state == ON ? TFT_GREEN : buttonPressed ? TFT_ORANGE
                                                                                   : TFT_WHITE); // zöld a keret, ha aktív, narancs ha nyomják
        pTft->setTextColor(state == ON ? TFT_GREEN : buttonPressed ? TFT_ORANGE
                                                                   : TFT_WHITE); // zöld a szöveg, ha aktív, narancs ha nyomják
        pTft->setTextDatum(MC_DATUM);                                            // Az (x, y) koordináta a szöveg középpontja

        pTft->setFreeFont(&FreeSansBold9pt7b);
        pTft->setTextSize(1);
        pTft->setTextPadding(0);
        pTft->drawString(label, x + w / 2, y + h / 2);

        // LED csík kirajzolása ha a gomb aktív vagy push, és nyomják
        if (state == ON or (type == PUSHABLE and buttonPressed)) {
            int ledHeight = 5;
            pTft->fillRect(x + 5, y + h - ledHeight - 3, w - 10, ledHeight, TFT_GREEN);
        }
    }

    /**
     * Touch adat van
     */
    void service(bool touched, uint16_t tx, uint16_t ty) {

        // Ha tiltott, akkor nem megyünk tovább
        if (state == DISABLED) {
            return;
        }

        // Ha van touch, de még nincs lenyomva a gomb, és erre a gombra jött a touch
        if (touched and !buttonPressed and contains(tx, ty)) {
            pressed();
        } else if (!touched and buttonPressed) {
            // Ha nincs ugyan touch, de ezt a gombot nyomva tartották eddig
            released();
        }
    }

    /**
     *
     */
    void setState(ButtonState_t state) {
        this->state = state;
        draw();
    }

    /**
     *
     */
    ButtonState_t getState() {
        return state;
    }

    static const __FlashStringHelper *decodeState(ButtonState_t _state) {
        switch (_state) {
        case OFF:
            return F("Off");
        case ON:
            return F("On");
        case HOLD:
            return F("Hold");
        case DISABLED:
            return F("Disabled");
        //---- technikai állapot
        case PUSHED:
            return F("Pushed");
        }

        return F("Unknown!!");
    }
};

#endif