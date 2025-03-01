#ifndef __POPUPBASE_H
#define __POPUPBASE_H

#include <TFT_eSPI.h>

#define DIALOG_CLOSE_BUTTON_LABEL "X"                 // Jobb felső sarok bezáró gomb
#define DIALOG_DEFAULT_BUTTONS_GAP 10                 // A gombok közötti térköz pixelekben
#define DIALOG_DEFAULT_BUTTON_HEIGHT 30               // Gomb(ok) magassága a dialógusban
#define DIALOG_DEFAULT_BUTTON_TEXT_PADDING_X (2 * 15) // 15-15px X padding a gombok szövegépen

/**
 * @class PopupBase
 * @brief Alaposztály felugró dialógusok létrehozásához TFT kijelzőn.
 *
 * Ez az osztály biztosítja az alapvető funkcionalitást felugró dialógusok megjelenítéséhez
 * és kezeléséhez TFT kijelzőn a TFT_eSPI könyvtár használatával. Kezeli a dialógus megjelenítését,
 * beleértve a címet és a bezárás gombot, valamint az érintési eseményeket a dialógus bezárásához.
 *
 * @note Ez az osztály származtatásra szolgál specifikus típusú dialógusok létrehozásához.
 */
class PopupBase {

private:
    static constexpr uint8_t DIALOG_CLOSE_BUTTON_SIZE = 20; // "X" gomb mérete
    static constexpr uint8_t DIALOG_HEADER_HEIGHT = 30;     // Fejléc magassága

    const __FlashStringHelper *title;   // Flash memóriában tárolt title szöveg
    const __FlashStringHelper *message; // Flash memóriában tárolt dialóg szöveg
    uint16_t *backgroundBuffer;         // A kitakart terület mentésének  buffere
    uint16_t y;                         // A leszármazottak nem láthatják az y pozíciót, csak a contentY alapján pozíciónálhatnak
    uint16_t messageY;
    uint16_t closeButtonX, closeButtonY; // X gomb pozíciója

protected:
    TFT_eSPI *pTft;
    uint16_t x, w, h;
    uint16_t contentY;

    /**
     * @brief Megjeleníti a dialógust.
     *
     * Ez a metódus megjeleníti a dialógust a TFT kijelzőn, beleértve a címet és a bezárás gombot.
     * Emellett elmenti a dialógus által takart háttérterületet.
     */
    virtual void drawDialog() {
        // Elmentjük a képernyő azon részét, amelyet a dialógus takar
        pTft->readRect(x, y, w, h, backgroundBuffer);

        // Kirajzoljuk a dialógot
        pTft->fillRect(x, y, w, h, TFT_DARKGREY); // háttér

        // Title kiírása
        if (title) {
            // Fejléc háttér kitöltése
            pTft->fillRect(x, y, w, DIALOG_HEADER_HEIGHT, TFT_NAVY);

            // Title kiírása
            pTft->setTextColor(TFT_WHITE);
            pTft->setTextDatum(TL_DATUM);                                                             // Bal felső sarokhoz igazítva
            pTft->drawString(title, x + 10, y + 5 + (DIALOG_HEADER_HEIGHT - pTft->fontHeight()) / 2); // Bal oldali margó 10px

            // Fejléc vonala
            // pTft->drawLine(x, y + DIALOG_HEADER_HEIGHT, x + w, y + DIALOG_HEADER_HEIGHT, TFT_WHITE);
            pTft->drawFastHLine(x, y + DIALOG_HEADER_HEIGHT, w, TFT_WHITE);
        }

        // Dialógus kerete
        pTft->drawRect(x, y, w, h, TFT_WHITE); // keret

        // "X" gomb kirajzolása
        closeButtonX = x + w - DIALOG_CLOSE_BUTTON_SIZE - 5; // Az "X" gomb pozíciója a title jobb oldalán, kis margóval a jobb szélre
        closeButtonY = y + 5;                                // Fejléc tetejéhez igazítva
        pTft->setTextColor(TFT_WHITE);
        pTft->setTextDatum(MC_DATUM); // Középre igazítva az "X"-et
        pTft->drawString(F(DIALOG_CLOSE_BUTTON_LABEL), closeButtonX + DIALOG_CLOSE_BUTTON_SIZE / 2, closeButtonY + DIALOG_CLOSE_BUTTON_SIZE / 2);

        // Üzenet kirajzolása, ha van üzenet
        if (message) {
            pTft->setTextColor(TFT_WHITE);
            pTft->setTextDatum(MC_DATUM);
            pTft->drawString(message, x + w / 2, messageY);
        }
    }

    /**
     * @brief PopupBase konstruktora.
     *
     * Inicializál egy új PopupBase osztály példányt a megadott paraméterekkel.
     *
     * @param tft A TFT_eSPI példányra mutató pointer.
     * @param w A dialógus szélessége.
     * @param h A dialógus magassága.
     * @param title A dialógus címe (opcionális).
     */
    PopupBase(TFT_eSPI *tft, uint16_t w, uint16_t h, const __FlashStringHelper *title = nullptr, const __FlashStringHelper *message = nullptr)
        : pTft(tft), w(w), h(h), title(title), message(message) {

        // Háttérbuffer lefoglalása
        backgroundBuffer = new uint16_t[w * h]();

        // Dialóg bal felső sarkának kiszámítása a képernyő középre igzaításához
        x = (tft->width() - w) / 2;
        y = (tft->height() - h) / 2;

        messageY = y + (title ? DIALOG_HEADER_HEIGHT + 15 : 5); // Az üzenet a fejléc utánkezdődjön, ha van fejléc
        contentY = messageY + (message ? 15 : 0);               // A belső tér az üzenet után kezdődjön, ha van üzenet
    }

public:
    /**
     * @brief PopupBase destruktora.
     *
     * Elrejti a dialógot, ha látszik és felszabadítja a PopupBase példány által használt erőforrásokat.
     */

    virtual ~PopupBase() {
        // Háttér visszaállítása
        pTft->pushRect(x, y, w, h, backgroundBuffer);

        // Háttér buffer felszabadítása
        delete[] backgroundBuffer;
    }

    /// @brief A párbeszédablak gombjainak érintési eseményeinek kezelése, a leszármazott implemnetálja
    /// @param touched Jelzi, hogy történt-e érintési esemény.
    /// @param tx Az érintési esemény x-koordinátája.
    /// @param ty Az érintési esemény y-koordinátája.
    virtual void handleTouch(bool touched, uint16_t tx, uint16_t ty) {}

protected:
    /**
     * @brief Ez a metódus ellenőrzi, hogy a bezárás gombot megérintették-e?
     *
     * @param touched Jelzi, hogy történt-e érintési esemény.
     * @param tx Az érintési esemény x-koordinátája.
     * @param ty Az érintési esemény y-koordinátája.
     * @returns true, ha megnyomták az X gombot a fejlécben
     */
    bool checkCloseButtonTouch(bool touched, uint16_t tx, uint16_t ty) {

        if (touched) {
            if (tx >= closeButtonX && tx <= closeButtonX + DIALOG_CLOSE_BUTTON_SIZE &&
                ty >= closeButtonY && ty <= closeButtonY + DIALOG_CLOSE_BUTTON_SIZE) {
                return true;
            }
        }

        return false;
    }
};

#endif
