#ifndef __MULTIBUTTONDIALOG_H
#define __MULTIBUTTONDIALOG_H

#include "PopupBase.h"
#include "TftButton.h"

class MultiButtonDialog : public PopupBase {
private:
    TftButton **buttons;
    uint8_t buttonCount;

    void drawDialog() {
        pTft->fillRect(x, y, w, h, TFT_DARKGREY);
        pTft->drawRect(x, y, w, h, TFT_WHITE);

        pTft->setTextColor(TFT_WHITE);
        pTft->setTextDatum(MC_DATUM);
        pTft->drawString("Válassz egy opciót:", x + w / 2, y + h / 3);

        // Gombok kirajzolása
        for (uint8_t i = 0; i < buttonCount; i++) {
            buttons[i]->draw();
        }
    }

public:
    MultiButtonDialog(TFT_eSPI *pTft, uint16_t w, uint16_t h, TftButton *buttonArray[], uint8_t count)
        : PopupBase(pTft, w, h), buttons(buttonArray), buttonCount(count) {

        uint16_t totalButtonWidth = 0;

        // Gombok összélességének kiszámítása (gap-ekkel együtt)
        for (uint8_t i = 0; i < buttonCount; i++) {
            totalButtonWidth += buttons[i]->getWidth();
        }
        totalButtonWidth += (buttonCount - 1) * DIALOG_BUTTONS_GAP;

        // Gombok kezdő X pozíciója (középre igazítás)
        uint16_t startX = x + (w - totalButtonWidth) / 2;
        uint16_t buttonY = y + h - DIALOG_BUTTON_HEIGHT - 10;

        // Gombok elhelyezése
        for (uint8_t i = 0; i < buttonCount; i++) {
            buttons[i]->setPosition(startX, buttonY);
            startX += buttons[i]->getWidth() + DIALOG_BUTTONS_GAP;
        }
    }

    void show() override {
        PopupBase::show();
        drawDialog();
    }

    void handleTouch(bool touched, uint16_t tx, uint16_t ty) override {
        for (uint8_t i = 0; i < buttonCount; i++) {
            buttons[i]->handleTouch(touched, tx, ty);
        }
    }
};

#endif
