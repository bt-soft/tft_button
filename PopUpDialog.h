#ifndef __POPUPDIALOG_H
#define __POPUPDIALOG_H

#include "PopupBase.h"
#include "TftButton.h"

/**
 * @class PopUpDialog
 * @brief Egy osztály, amely egy felugró párbeszédablakot képvisel OK és opcionális Cancel gombokkal.
 *
 * Ez az osztály a PopupBase-ből származik, és egy felugró párbeszédablak létrehozására és kezelésére szolgál
 * egy TFT kijelzőn. A párbeszédablak üzenetet jeleníthet meg, és rendelkezik OK és opcionális Cancel gombokkal.
 *
 * @details
 * A PopUpDialog osztály lehetőséget biztosít egy üzenetet tartalmazó párbeszédablak megjelenítésére, a gombok érintési eseményeinek kezelésére,
 * és egy visszahívási függvény végrehajtására, amikor az OK gombot megnyomják.
 *
 * @note Az üzenet szövege flash memóriában van tárolva.
 *
 * @param pTft A TFT_eSPI példányra mutató pointer.
 * @param w A párbeszédablak szélessége.
 * @param h A párbeszédablak magassága.
 * @param title A párbeszédablak címe (opcionális).
 * @param message A párbeszédablak üzenete.
 * @param callback Az OK gomb visszahívási függvénye.
 * @param okText Az OK gomb felirata.
 * @param cancelText A Cancel gomb felirata (opcionális).
 */
class PopUpDialog : public PopupBase {
private:
    TftButton *okButton;
    TftButton *cancelButton;
    ButtonCallback callback;

    /// @brief A párbeszédablak komponenseinek megjelenítése
    void drawDialog() {
        // Kirajzoljuk az OK gombot
        okButton->draw();

        // Ha van Cancel gomb, akkor kirajzoljuk azt is
        if (cancelButton)
            cancelButton->draw();
    }

protected:
    /// @brief Párbeszédablak konstruktor
    /// @param pTft A TFT_eSPI példányra mutató pointer.
    /// @param w A párbeszédablak szélessége.
    /// @param h A párbeszédablak magassága.
    /// @param title A párbeszédablak címe (opcionális).
    /// @param message A párbeszédablak üzenete.
    /// @param callback Az OK gomb visszahívási függvénye.
    /// @param okText Az OK gomb felirata.
    /// @param cancelText A Cancel gomb felirata (opcionális).
    PopUpDialog(TFT_eSPI *pTft, uint16_t w, uint16_t h, const __FlashStringHelper *title, const __FlashStringHelper *message, ButtonCallback callback, const char *okText = "OK", const char *cancelText = nullptr)
        : PopupBase(pTft, w, h, title, message), callback(callback), cancelButton(nullptr) {

        // Kiszedjük a legnagyobb gomb felirat szélességét (10-10 pixel a szélén)
        uint8_t okButtonWidth = pTft->textWidth(okText) + DIALOG_BUTTON_TEXT_PADDING_X;                          // OK gomb szöveg szélessége + padding a gomb széleihez
        uint8_t cancelButtonWidth = cancelText ? pTft->textWidth(cancelText) + DIALOG_BUTTON_TEXT_PADDING_X : 0; // Cancel gomb szöveg szélessége, ha van

        // Ha van Cancel gomb, akkor a két gomb közötti gap-et is figyelembe vesszük
        uint16_t totalButtonWidth = cancelButtonWidth > 0 ? okButtonWidth + cancelButtonWidth + DIALOG_BUTTONS_GAP : okButtonWidth;
        uint16_t okX = x + (w - totalButtonWidth) / 2; // Az OK gomb X pozíciója -> a gombok kezdő X pozíciója

        // Gombok Y pozíció
        uint16_t buttonY = contentY + DIALOG_BUTTON_HEIGHT;

        // OK gomb
        okButton = new TftButton(pTft, okX, buttonY, okButtonWidth, DIALOG_BUTTON_HEIGHT, okText, ButtonType::PUSHABLE, callback);

        // Cancel gomb, ha van
        if (cancelText) {
            uint16_t cancelX = okX + okButtonWidth + DIALOG_BUTTONS_GAP; // A Cancel gomb X pozíciója
            cancelButton = new TftButton(pTft, cancelX, buttonY, cancelButtonWidth, DIALOG_BUTTON_HEIGHT, cancelText, ButtonType::PUSHABLE);
        }
    }

public:
    /// @brief Párbeszédablak destruktor
    ~PopUpDialog() {

        Serial << "~PopUpDialog() start" << endl;

        delete okButton;
        if (cancelButton) {
            delete cancelButton;
        }

        Serial << "~PopUpDialog() end" << endl;
    }

    /// @brief Megjeleníti a párbeszédablakot.
    void show() override {
        PopupBase::show();
        drawDialog();
    }

    /// @brief A párbeszédablak gombjainak érintési eseményeinek kezelése
    /// @param touched Jelzi, hogy történt-e érintési esemény.
    /// @param tx Az érintési esemény x-koordinátája.
    /// @param ty Az érintési esemény y-koordinátája.
    void handleTouch(bool touched, uint16_t tx, uint16_t ty) override {

        // Először meghívjuk a PopupBase érintéskezelőjét
        PopupBase::handleTouch(touched, tx, ty);

        okButton->handleTouch(touched, tx, ty);
        if (cancelButton)
            cancelButton->handleTouch(touched, tx, ty);
    }

    /**
     * @brief Létrehoz egy új PopUpDialog példányt.
     *
     * @param dialogPointer Az új dialóg pointere
     * @param pTft A TFT_eSPI objektumra mutató pointer, amelyet a kijelzőhöz használnak.
     * @param w A párbeszédablak szélessége.
     * @param h A párbeszédablak magassága.
     * @param title A cím szövegére mutató pointer, amely a flash memóriában van tárolva.
     * @param message Az üzenet szövegére mutató pointer, amely a flash memóriában van tárolva.
     * @param callback A függvény, amelyet a gomb megnyomásakor hívnak meg.
     * @param okText Az OK gomb szövege (alapértelmezett érték: "OK").
     * @param cancelText A Cancel gomb szövege (alapértelmezett érték: nullptr).
     * @return PopUpDialog* Pointer az újonnan létrehozott PopUpDialog példányra.
     */
    static void createDialog(PopupBase **dialogPointer, TFT_eSPI *pTft, uint16_t w, uint16_t h, const __FlashStringHelper *title, const __FlashStringHelper *message, ButtonCallback callback, const char *okText = "OK", const char *cancelText = nullptr) {
        *dialogPointer = new PopUpDialog(pTft, w, h, title, message, callback, okText, cancelText);
    }
};

#endif
