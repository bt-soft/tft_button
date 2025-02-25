#ifndef __POPUPDIALOG_H
#define __POPUPDIALOG_H

#include "PopupBase.h"
#include "TftButton.h"

class PopUpDialog : public PopupBase {
private:
    String message;
    TftButton *okButton;
    TftButton *cancelButton = nullptr;
    ButtonCallback callback;

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
    PopUpDialog(TFT_eSPI *pTft, uint16_t w, uint16_t h, String message, ButtonCallback callback, const char *okText = "OK", const char *cancelText = nullptr)
        : PopupBase(pTft, w, h), message(message), callback(callback) {

        // Képernyő közepét kiszámítjuk
        x = (pTft->width() - w) / 2;
        y = (pTft->height() - h) / 2;

        uint16_t buttonY = y + h - DIALOG_BUTTON_HEIGHT - 10;

        // Kiszedjük a legnagyobb gomb felirat szélességét (10-10 pixel a szélén)
        uint8_t okButtonWidth = pTft->textWidth(okText) + DIALOG_BUTTON_TEXT_PADDING_X;                          // OK gomb szöveg szélessége + padding a gomb széleihez
        uint8_t cancelButtonWidth = cancelText ? pTft->textWidth(cancelText) + DIALOG_BUTTON_TEXT_PADDING_X : 0; // Cancel gomb szöveg szélessége, ha van

        // Ha van Cancel gomb, akkor a két gomb közötti gap-et is figyelembe vesszük
        uint16_t totalButtonWidth = cancelButtonWidth > 0 ? okButtonWidth + cancelButtonWidth + DIALOG_BUTTONS_GAP : okButtonWidth;
        uint16_t okX = x + (w - totalButtonWidth) / 2; // Az OK gomb X pozíciója -> a gombok kezdő X pozíciója

        // OK button
        okButton = new TftButton(pTft, okX, buttonY, okButtonWidth, DIALOG_BUTTON_HEIGHT, okText, ButtonType::PUSHABLE, callback);

        // Cancel button, ha van
        if (cancelText) {
            uint16_t cancelX = okX + okButtonWidth + DIALOG_BUTTONS_GAP; // A Cancel gomb X pozíciója
            cancelButton = new TftButton(pTft, cancelX, buttonY, cancelButtonWidth, DIALOG_BUTTON_HEIGHT, cancelText, ButtonType::PUSHABLE);
        }
    }

    ~PopUpDialog() {
        delete okButton;
        if (cancelButton)
            delete cancelButton;
    }

    void show() override {
        PopupBase::show();
        drawDialog();
    }

    void handleTouch(bool touched, uint16_t tx, uint16_t ty) override {
        okButton->handleTouch(touched, tx, ty);
        if (cancelButton)
            cancelButton->handleTouch(touched, tx, ty);
    }
};

#endif
