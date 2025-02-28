#ifndef __POPUPBASE_H
#define __POPUPBASE_H

#include <TFT_eSPI.h>

#define DIALOG_BUTTONS_GAP 5                  // A gombok közötti térköz pixelekben
#define DIALOG_BUTTON_HEIGHT 30               // Gomb(ok) magassága a dialógusban
#define DIALOG_BUTTON_TEXT_PADDING_X (2 * 15) // 15-15px X padding

class PopupBase {

private:
    uint16_t *backgroundBuffer;
    bool visible;
    uint16_t y;

protected:
    TFT_eSPI *pTft;
    uint16_t x, w, h;
    uint16_t contentY;
    String title;
#define CLOSE_BUTTON_SIZE 20             // "X" gomb mérete
    uint16_t closeButtonX, closeButtonY; // Az 'X' gomb pozíciója

public:
    PopupBase(TFT_eSPI *tft, uint16_t w, uint16_t h, String title = "") : pTft(tft), w(w), h(h), visible(false), title(title) {
        x = (tft->width() - w) / 2;
        y = (tft->height() - h) / 2;
        backgroundBuffer = new uint16_t[w * h]();

        contentY = y + (title.length() > 0 ? 50 : 5); // Fejléc után kezdődjön a belső tér, ha van title

        // Az "X" gomb pozíciója a title jobb oldalán
        closeButtonX = x + w - CLOSE_BUTTON_SIZE - 5; // Kis margóval a jobb szélre
        closeButtonY = y + 5;                         // Fejléc tetejéhez igazítva
    }

    virtual ~PopupBase() {
        delete[] backgroundBuffer;
    }

    virtual void show() {
        // Elmentjük a képernyő azon részét, amelyet a dialógus takar
        pTft->readRect(x, y, w, h, backgroundBuffer);

        // Kirajzoljuk a dialógot
        pTft->fillRect(x, y, w, h, TFT_DARKGREY);
        pTft->drawRect(x, y, w, h, TFT_WHITE);

        // Title kiírása
        if (title.length() > 0) {
            pTft->setTextColor(TFT_WHITE);
            pTft->setTextDatum(TL_DATUM);                        // Bal felső sarokhoz igazítva
            pTft->drawString(title, x + 10, y + 10);             // Bal oldali margó 10px
            pTft->drawLine(x, y + 30, x + w, y + 30, TFT_WHITE); // Fejléc alatti vonal
        }

        // "X" gomb kirajzolása
        pTft->setTextColor(TFT_WHITE);
        pTft->setTextDatum(MC_DATUM); // Középre igazítva az "X"-et
        pTft->drawString("X", closeButtonX + CLOSE_BUTTON_SIZE / 2, closeButtonY + CLOSE_BUTTON_SIZE / 2);

        visible = true;
    }

    virtual void hide() {
        // Háttér visszaállítása
        pTft->pushRect(x, y, w, h, backgroundBuffer);
        visible = false;
    }

    bool isVisible() const {
        return visible;
    }

    virtual void handleTouch(bool touched, uint16_t tx, uint16_t ty) {
        // Ha megérintettük a "X" gombot, elrejtjük a dialógust
        if (touched) {
            Serial << "hello" << endl;
            if (tx >= closeButtonX && tx <= closeButtonX + CLOSE_BUTTON_SIZE &&
                ty >= closeButtonY && ty <= closeButtonY + CLOSE_BUTTON_SIZE) {
                hide(); // Elrejtjük a dialógust
            }
        }
    }
};

#endif
