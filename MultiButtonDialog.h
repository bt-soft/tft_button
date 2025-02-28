#ifndef __MULTIBUTTONDIALOG_H
#define __MULTIBUTTONDIALOG_H

#include "PopupBase.h"
#include "TftButton.h"

class MultiButtonDialog : public PopupBase {

private:
    String message;
    TftButton **buttons;
    uint8_t buttonCount;

    void drawDialog() {
        pTft->setTextColor(TFT_WHITE);
        pTft->setTextDatum(MC_DATUM);
        pTft->drawString(message, x + w / 2, contentY);

        // Gombok kirajzolása
        for (uint8_t i = 0; i < buttonCount; i++) {
            buttons[i]->draw();
        }
    }

public:
    MultiButtonDialog(TFT_eSPI *pTft, uint16_t w, uint16_t h, String message, TftButton *buttonArray[], uint8_t count)
        : PopupBase(pTft, w, h), message(message), buttons(buttonArray), buttonCount(count) {

        uint16_t maxRowWidth = w - 20; // Max szélesség, kis margóval
        uint16_t buttonHeight = DIALOG_BUTTON_HEIGHT;
        uint8_t buttonsPerRow = 1;
        uint16_t totalWidth = 0;

        // Kiszámoljuk, hogy hány gomb fér el egy sorban
        for (uint8_t i = 0; i < buttonCount; i++) {
            totalWidth += buttons[i]->getWidth() + DIALOG_BUTTONS_GAP;
            if (totalWidth - DIALOG_BUTTONS_GAP > maxRowWidth) {
                break; // Ha túlcsordul, az előző szám volt a max
            }
            buttonsPerRow++;
        }
        buttonsPerRow--; // Az utolsó túlcsordulás miatt csökkentjük

        // Kiszámoljuk a sorok számát
        uint8_t rowCount = (buttonCount + buttonsPerRow - 1) / buttonsPerRow; // Felkerekítés
        uint16_t totalHeight = rowCount * buttonHeight + (rowCount - 1) * DIALOG_BUTTONS_GAP;

// Szöveg alatti térköz kiszámítása
#define SPACING_AFTER_MESSAGE 20 // Kis térköz a szöveg után
        uint16_t startY = contentY + SPACING_AFTER_MESSAGE;

        // Gombok pozicionálása több sorban
        uint8_t row = 0, col = 0;
        uint16_t startX = 0;
        for (uint8_t i = 0; i < buttonCount; i++) {
            // Sor elején újraszámoljuk a kezdő X-et (középre igazítás)
            if (col == 0) {
                uint16_t rowWidth = 0;
                uint8_t itemsInRow = min(buttonsPerRow, buttonCount - i);
                for (uint8_t j = 0; j < itemsInRow; j++) {
                    rowWidth += buttons[i + j]->getWidth();
                }
                rowWidth += (itemsInRow - 1) * DIALOG_BUTTONS_GAP;
                startX = x + (w - rowWidth) / 2;
            }

            // Gomb elhelyezése
            buttons[i]->setPosition(startX, startY);
            startX += buttons[i]->getWidth() + DIALOG_BUTTONS_GAP;
            col++;

            // Ha betelt egy sor, új sorba lépünk
            if (col >= buttonsPerRow) {
                col = 0;
                row++;
                startY += buttonHeight + DIALOG_BUTTONS_GAP;
            }
        }
    }

    ~MultiButtonDialog() {
        for (uint8_t i = 0; i < buttonCount; i++) {
            delete buttons[i];
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
