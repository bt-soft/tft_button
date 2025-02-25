#ifndef __POPUPBASE_H
#define __POPUPBASE_H

#include <TFT_eSPI.h>

class PopupBase {

private:
    uint16_t *backgroundBuffer;
    bool visible;

protected:
    TFT_eSPI *pTft;
    uint16_t x, y, w, h;

public:
    PopupBase(TFT_eSPI *tft, uint16_t w, uint16_t h) : pTft(tft), w(w), h(h), visible(false) {
        x = (tft->width() - w) / 2;
        y = (tft->height() - h) / 2;
        backgroundBuffer = new uint16_t[w * h]();
    }

    virtual ~PopupBase() {
        delete[] backgroundBuffer;
    }

    virtual void show() {
        // Elmentjük a képernyő azon részét, amelyet a dialógus takar
        pTft->readRect(x, y, w, h, backgroundBuffer);
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
};

#endif
