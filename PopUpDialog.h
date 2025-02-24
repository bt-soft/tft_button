#ifndef __POPUPDIALOG_H
#define __POPUPDIALOG_H

#include "TftButton.h"
#include <TFT_eSPI.h>

class PopUpDialog {
private:
    TFT_eSPI *pTft;
    uint16_t x, y, w, h;
    String message;
    TftButton *okButton;
    TftButton *cancelButton = nullptr;
    ButtonCallback callback;
    uint16_t *backgroundBuffer;
    bool visible;

    void drawDialog() {
        pTft->fillRect(x, y, w, h, TFT_DARKGREY);
        pTft->drawRect(x, y, w, h, TFT_WHITE);

        pTft->setTextColor(TFT_WHITE);
        pTft->setTextDatum(MC_DATUM);
        pTft->drawString(message, x + w / 2, y + h / 3);

        // Kirajzoljuk az OK gombot
        okButton->draw();

        // Ha van Cancel gomb, akkor kirajzoljuk azt is
        if (cancelButton)
            cancelButton->draw();
    }

public:
    PopUpDialog(TFT_eSPI *tft, uint16_t w, uint16_t h, String message, ButtonCallback callback, const char *okText = "OK", const char *cancelText = nullptr)
        : pTft(tft), w(w), h(h), message(message), callback(callback) {

        // Képernyő közepét kiszámítjuk
        x = (tft->width() - w) / 2;
        y = (tft->height() - h) / 2;

        uint16_t buttonWidth = w / 3;
        uint16_t buttonHeight = 30;
        uint16_t buttonY = y + h - buttonHeight - 10;

        // backgroundBuffer = new uint16_t[w * h]; // Memória foglalása a háttérhez
        backgroundBuffer = (uint16_t *)calloc(w * h, sizeof(uint16_t)); // Memória foglalása a háttérhez

        // Ha van Cancel gomb, akkor az OK gombot balra, Cancel gombot jobbra igazítjuk
        // Ha nincs Cancel gomb, akkor az OK gombot középre helyezzük
        uint16_t totalButtonWidth = buttonWidth * 2 + 5;  // A két gomb szélessége és a gap
        uint16_t gap = 5;                                 // A gombok közötti térköz
        uint16_t startX = x + (w - totalButtonWidth) / 2; // A gombok kezdő X pozíciója

        uint16_t okX = startX; // Az OK gomb X pozíciója
        okButton = new TftButton(pTft, okX, buttonY, buttonWidth, buttonHeight, okText, ButtonType::PUSHABLE, callback);

        if (cancelText) {
            uint16_t cancelX = startX + buttonWidth + gap; // A Cancel gomb X pozíciója
            cancelButton = new TftButton(pTft, cancelX, buttonY, buttonWidth, buttonHeight, cancelText, ButtonType::PUSHABLE);
        }

        visible = false;
    }

    ~PopUpDialog() {
        delete okButton;
        if (cancelButton)
            delete cancelButton;
        free(backgroundBuffer);
    }

    void show() {
        // Elmentjük a képernyő azon részét, amelyet a dialógus takar
        pTft->readRect(x, y, w, h, backgroundBuffer); // Háttér mentése
        drawDialog();
        visible = true;
    }

    void hide() {
        pTft->pushRect(x, y, w, h, backgroundBuffer); // Háttér visszaállítása
        visible = false;
    }

    bool isVisible() {
        return visible;
    }

    void handleTouch(bool touched, uint16_t tx, uint16_t ty) {
        okButton->handleTouch(touched, tx, ty);
        if (cancelButton)
            cancelButton->handleTouch(touched, tx, ty);
    }
};

#endif
