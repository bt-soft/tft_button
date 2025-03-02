#ifndef __TOUCH_CALIBRATE_H
#define __TOUCH_CALIBRATE_H

#include <LittleFS.h>
#include <Streaming.h>
#include <TFT_eSPI.h>

/**
 * @class TouchCalibrate
 * @brief Egy osztály a TFT érintőképernyő kalibrálásához és a kalibrációs adatok kezeléséhez a LittleFS segítségével.
 *
 * Ez az osztály módszereket biztosít a TFT érintőképernyő kalibrálásához és a kalibrációs adatok tárolásához/lekéréséhez
 * a fájlrendszerből. Tartalmaz egy módszert is a megadott könyvtárban lévő fájlok listázásához.
 *
 * @method static void calibrate(TFT_eSPI *pTft, bool repeatCal = false)
 * @brief Kalibrálja a TFT érintőképernyőt és kezeli a kalibrációs adatokat.
 *
 * Ez a módszer ellenőrzi, hogy a fájlrendszer elérhető-e és hogy létezik-e a kalibrációs fájl.
 * Ha a kalibrációs adatok érvényesek és a repeatCal értéke false, beállítja az érintőképernyő kalibrációs adatait.
 * Ellenkező esetben új kalibrációt végez és tárolja az adatokat a fájlrendszerben.
 *
 * @param pTft Pointer a TFT_eSPI objektumra.
 * @param repeatCal Ha true, újrakalibrálást kényszerít, még akkor is, ha érvényes kalibrációs adatok léteznek.
 *
 * @method static void listFiles(const char *dirname = "/")
 * @brief Fájlok listázása a megadott könyvtárban.
 *
 * Ez a módszer megnyitja a megadott könyvtárat és listázza az összes benne lévő fájlt, megjelenítve azok nevét és méretét.
 *
 * @param dirname A könyvtár, amelyből a fájlokat listázni kell. Alapértelmezés szerint a gyökérkönyvtár ("/").
 */
class TouchCalibrate {

#define TFT_TOUCH_CALIBRATION_FILE "/TouchCalibrationData.txt"

public:
    /**
     * TFT touch kalibrálása, kalibrációs adatok kiírása/beolvasása a littleFs segítségével
     */
    static void calibrate(TFT_eSPI *pTft, bool repeatCal = false, bool format = false) {
        uint16_t calData[5];
        bool calDataOK = false;

        // Ellenőrizzük, hogy a fájlrendszer létezik-e
        if (!LittleFS.begin() or format) {
            Serial << F("Fájlrendszer formázása") << endl;
            LittleFS.format();
            LittleFS.begin();
        }

        // Ellenőrizzük, hogy a kalibrációs fájl létezik-e és a mérete megfelelő-e
        if (LittleFS.exists(TFT_TOUCH_CALIBRATION_FILE)) {
            if (repeatCal) {
                Serial << F("Kalibrációs adatfájl törlése, mert újra akarjuk kalibrálni") << endl;
                LittleFS.remove(TFT_TOUCH_CALIBRATION_FILE);
            } else {
                File file = LittleFS.open(TFT_TOUCH_CALIBRATION_FILE, "r");
                if (file) {
                    if (file.readBytes((char *)calData, 14) == 14) {
                        calDataOK = true;
                    }
                    file.close();
                }
                Serial << F("Kalibrációs adatok beolvasva a fájlrendszerből") << endl;
            }
        }

        if (calDataOK && !repeatCal) {
            // A kalibrációs adatok érvényesek
            pTft->setTouch(calData);

        } else {
            // Az adatok nem érvényesek, ezért újra kell kalibrálni
            const __FlashStringHelper *txt = F("Erintsd meg a sarkokat a jelzett helyeken!");
            pTft->fillScreen(TFT_BLACK);
            pTft->setCursor((pTft->width() - pTft->textWidth(txt)) / 2, pTft->height() / 2 - 40);
            pTft->setTextFont(2);
            pTft->setTextSize(1);
            pTft->setTextColor(TFT_WHITE, TFT_BLACK);
            pTft->println(txt);

            pTft->println();

            if (repeatCal) {
                txt = F("Allitsd a repeatCal parameter erteket false-ra, hogy ne fusson ujra!");
                pTft->setCursor((pTft->width() - pTft->textWidth(txt)) / 2, pTft->height() / 2 + 10);
                pTft->setTextColor(TFT_RED, TFT_BLACK);
                pTft->println(txt);
            }

            // TFT Kalibráció indítása
            pTft->calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

            txt = F("Kalibracio befejezodott!");
            pTft->fillScreen(TFT_BLACK);
            pTft->setCursor((pTft->width() - pTft->textWidth(txt)) / 2, pTft->height() / 2);
            pTft->setTextColor(TFT_GREEN, TFT_BLACK);
            pTft->setTextSize(1);
            pTft->println(txt);

            // Adatok tárolása
            File file = LittleFS.open(TFT_TOUCH_CALIBRATION_FILE, "w");
            if (file) {
                file.write((const unsigned char *)calData, 14);
                file.close();
                Serial << F("Kalibrációs adatok kiírva a fájlrendszerbe") << endl;
            }

            Serial << endl
                   << endl
                   << F("// Használja ezt a kalibrációs kódot a setup()-ban:") << endl
                   << F("  uint16_t calData[5] = { ");
            for (uint8_t i = 0; i < 5; i++) {
                Serial << calData[i] << (i < 4 ? ", " : "");
            }

            Serial << F(" };") << endl;
            Serial << F("  pTft->setTouch(calData);") << endl
                   << endl;

            delay(3000);
        }
    }

    /**
     *
     */
    static void listAllFilesInDir(const char *dirname = "/") {
        Dir dir = LittleFS.openDir(dirname);
        while (dir.next()) {
            if (dir.isFile()) {
                Serial << "Fájl: " << dirname << dir.fileName() << " - Méret: " << dir.fileSize() << " bájt" << endl;
            } else if (dir.isDirectory()) {
                Serial << "Dir: " << dirname << dir.fileName() << "/" << endl;
                listAllFilesInDir((String(dirname) + dir.fileName() + "/").c_str());
            }
        }
    }
};

#endif