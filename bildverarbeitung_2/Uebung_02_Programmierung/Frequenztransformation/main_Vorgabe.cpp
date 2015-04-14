// --------------------------------------------------------------------------------
// Uebung 2: Frequenztransformation
// B. Lang, HS-Osnabrueck
// angepasst M. Weinhardt SS 2014
// --------------------------------------------------------------------------------

#include <iostream>
#include <cmath>
#include <queue>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

#include "Img.h"
#include "BmpRead.h"
#include "ConvImg.h"
#include "BmpWrite.h"
#include "FFT.h"
#include "AviWrite.h"
#include <camera.h>


// ------------------------------------------------------------------------------------------------
// Bildquellen definieren
// ------------------------------------------------------------------------------------------------
#define BILD_von_DATEI    1
#define BILD_von_KAMERA   2
// ------------------------------------------------------------------------------------------------
// Bildquelle auswaehlen: BMP-Bild oder Kamera
// ------------------------------------------------------------------------------------------------
#define BILDQUELLE BILD_von_DATEI
//#define BILDQUELLE BILD_von_KAMERA

// ------------------------------------------------------------------------------------------------
// Hauptprogramm
// Aufrufe der Funktionen
// ------------------------------------------------------------------------------------------------
int main(int argc, char*argv[]) {

  cout << "BV-Praktikum: Uebung 2" << endl;

  // --------------------------------------------------------------------------------
  // Bildaufnahme
  // --------------------------------------------------------------------------------
#if BILDQUELLE==BILD_von_DATEI
  if (argc<2) {
      cerr << "Dateiname nicht angegeben" << endl;
      return 5;
  }
  string Bildname(argv[1]);

  // Bild einlesen und nach BYTE konvertieren
  Img<RGB_Pixel> rgb;
  try {
      BmpRead((Bildname+".bmp").c_str()) >> rgb;
  }
  catch (...) {
      cerr << "Kann Datei" << argv[1] << "nicht oeffnen" << endl;
      return 5;
  }
#elif BILDQUELLE==BILD_von_KAMERA
  string Bildname("Kamera");
  Img<RGB_Pixel> rgb;
  try {
    rgb = get_rgb_img_from_camera("192.168.0.65");
  }
  catch (...) {
      cerr << "Bildaufnahme von Kamera nicht moeglich" << endl;
      return 5;
  }
#else
  cerr << "Keine Bildquelle ausgewaehlt" << endl;
  return 5;
#endif

  // --------------------------------------------------------------------------------
  // Grauwert Quellbild erzeugen
  // --------------------------------------------------------------------------------
  const unsigned int height = rgb.Height();
  const unsigned int width  = rgb.Width();
  unsigned int ld;
  unsigned int val;
  for(ld=0, val=width; val!=0; val=val>>1) { ld++; }
  if ((1<<(ld-1)) != width) {
    cerr << "Bildbreite ist keine Zweierpotenz, fft kann nicht verwendet werden" << endl;
    return 5;
  }
  for(ld=0, val=height; val!=0; val=val>>1) { ld++; }
  if ((1<<(ld-1)) != height) {
    cerr << "Bildhoehe ist keine Zweierpotenz, fft kann nicht verwendet werden" << endl;
    return 5;
  }
  Img<double> src = ConvImg<double,RGB_Pixel>(rgb);

  // -----------------------------------------------------
  // Aufgabe 1:
  // Zeilentransformation durchfuehren
  // -----------------------------------------------------
  Img<complex<double> > z_fft_img(width,height); // In den Zeilen FFT-transformiertes Bild
  Img<double> z_fft_mag(width,height);           // In den Zeilen FFT-transformiertes Bild (nur Betrag)
  vector<complex<double> > ori_zeile(width);     // Hilfsvektor fuer eine originale Bildzeile
  vector<complex<double> > fft_zeile(width);     // Hilfsvektor fuer eine FFT-transformierte Bildzeile

  // Zeilenweise das Quellbild src in den ori_zeile-Vektor kopieren,
  // Fouriertransformation durchführen und den Ergebnisvektor fft_zeile im
  // zeilentransformierten Bild z_fft_img abspeichern.
  // Zur Inspektion auch das Betragsbild z_fft_mag berechnen.

  // -------------------------------------------------------------------------------
  // Parameter:  src        - Quellbild
  // Ergebnis:   z_fft_img  - Zeilentransformiertes Bild
  //             z_fft_mag  - Betrag des Zeilentransformierten Bilds
  // ==== Ihr Code ==== //

  // Betragsbild ausgeben
  { pair<double,double> min_Max = get_min_Max<double>(z_fft_mag);
    Img<unsigned char> uc_z_fft_mag = ConvImg<unsigned char,double>(z_fft_mag, 255.0/(min_Max.second-min_Max.first), -min_Max.first);
    BmpWrite((Bildname+"_z_fft_mag.bmp").c_str(),uc_z_fft_mag);
  }

  // ----------------------------------------------------------------------------------------------------------
  // Aufgabe 2:
  // Die Spektren aller Zeilen zusammenfassen (addieren) und im aufsummierten Spektrum die Grundfrequenz suchen
  // ----------------------------------------------------------------------------------------------------------
  vector<double> SpektrenSumme(width);
  ofstream spsum((Bildname+"_SpektrenSumme.csv").c_str());
  // Spektrensumme berechnen
  // -------------------------------------------------------------------------------
  // Parameter: z_fft_mag     - Zeilentransformiertes Betragsbild
  // Ergebnis:  SpektrenSumme - Summe der Spektren aller Zeilen
  // ==== Ihr Code ==== //

  // Spektrensumme in .csv-Datei schreiben
  for (unsigned int x=0; x<width; x++) {
    spsum << SpektrenSumme[x] << endl;
  }

  // Suche Maximum
  const unsigned int k(5);  // Mindestanzahl von Perioden in einer Bildzeile
  unsigned int f_max(0);    // Position des Maximums
  // -------------------------------------------------------------------------------
  // Parameter: SpektrenSumme - Summe der Spektren aller Zeilen
  // Ergebnis:  f_max         - Position der gefundenen Grundfrequenz im Spektrum
  // ==== Ihr Code ==== //

  cout << "Grundfrequenz: " << f_max << " Perioden. Dies entspricht " << double(width)/double(f_max) << " Pixel pro Periode." << endl;

  // ----------------------------------------------------------------------------------------------------------
  // Aufgabe 3:
  // Pro Zeile nur Mittelwert und Grundfrequenz beibehalten und zurueck in den Originalraum transformieren
  // ----------------------------------------------------------------------------------------------------------
  Img<unsigned char> ifft_src(width,height); // Kopie des zeilentransformierten Bilds nur mit Mittelwert und Grundfrequenz

  // -------------------------------------------------------------------------------
  // Parameter:  z_fft_img - Zeilentransformiertes Bild
  // Ergebnis:   ifft_src  - Rücktransformiertes Bild aus Gleichanteil und Grundfrequenz
  // ==== Ihr Code ==== //

  BmpWrite((Bildname+"_ifft_src.bmp").c_str(),ifft_src);

  // ----------------------------------------------------------------------------------------------------------
  // Aufgabe 4:
  // Das zeilentransformierte Bild nun auch in den Spalten transformieren
  // ----------------------------------------------------------------------------------------------------------
  Img<complex<double> > fft_img(width,height); // FFT-transformiertes Bild
  Img<double>           fft_mag(width,height); // FFT-transformiertes Bild (nur Betrag)
  // -------------------------------------------------------------------------------
  // Parameter:  z_fft_img - Zeilentransformiertes Bild
  // Ergebnis:   fft_img   - Fouriertransformiertes Bild in Zeilen und Spalten
  // ==== Ihr Code ==== //

  // Betragsbild auf 256 Graustufen skalieren und ausgeben
  pair<double,double> min_Max;
  { // Minimal- und maximalen Wert suchen
    fft_mag[0][0] = fft_mag[0][1];  // Koeffizient 00 nicht in die Skalierung einbeziehen,
                                    // da er typischerweise deutlich groesser als die anderen Koeffizienten ist
    min_Max = get_min_Max<double>(fft_mag);
    fft_mag[0][0] = min_Max.second; // Koeffizient 00 auf den maximalen Wert
    // Bild auf 256 Graustufen skalieren und ausgeben
    Img<unsigned char> uc_fft_mag = ConvImg<unsigned char,double>(fft_mag, 255.0/(min_Max.second-min_Max.first), -min_Max.first);
    BmpWrite((Bildname+"_fft_mag.bmp").c_str(),uc_fft_mag);
    // Bild in Shifting-Darstellung ausgeben
    Img<unsigned char> uc_fft_mag_shifting(width,height);
    uc_fft_mag.Margin_Cyclic();
    for (unsigned int y=0; y<height; y++) {
      for (unsigned int x=0; x<width; x++) {
        uc_fft_mag_shifting[y][x] = uc_fft_mag[y-height/2][x-width/2];
      }
    }
    BmpWrite((Bildname+"_fft_mag_shifting.bmp").c_str(),uc_fft_mag_shifting);
  }

  // ----------------------------------------------------------------------------------------------------------
  // Aufgabe 5:
  // Beginnend mit dem Mittelwert die Frequenzen schrittweise in ein Ausgabebild kopieren
  // und in den Originalraum transformieren.
  // Die Bilder im Originalraum werden dann zu einem AVI-Film zusammengestellt.
  // ----------------------------------------------------------------------------------------------------------
  try {
    AviWrite avi((Bildname+"_frequenzanalyse.avi").c_str(),width,height,5); // AVI-Objekt oeffnen
    Img<complex<double> > fft_img_inc(width,height);   // FFT-transformiertes Bild, dem schrittweise die Frequenzkoeffizienten zugewiesen werden
    Img<complex<double> > src_img_i;                   // Rücktransformiertes complex-Originalbild
    Img<RGB_Pixel>        rgb_src_img_i(width,height); // Rücktransformiertes rgb-Originalbild
    fft_img_inc = complex<double>(0,0);      // Alle Pixel zu 0 setzen
    rgb_src_img_i = RGB_Pixel(255,255,255);  // Alle Pixel zu weiss setzen
    fft_img.Margin_Cyclic();                 // Randbehandlung einstellen
    fft_img_inc.Margin_Cyclic();             // Randbehandlung einstellen
    rgb_src_img_i.Margin_Cyclic();           // Randbehandlung einstellen

    // Mittelwert rücktransformieren
    fft_img_inc[0][0] = fft_img[0][0];  // Koefizient 0,0 (Mittelwert) kopieren
    src_img_i = iFFT_2D(fft_img_inc);   // Bild vom FFT-Raum in den Originalraum zuruecktransformieren
    // complex-Bild in Grauwertbild in RGB-Darstellung konvertieren
    for (unsigned int y=0; y<height; y++) {
      for (unsigned int x=0; x<width; x++) {
        unsigned char val = 256*src_img_i[y][x].real();
        if (val<0)   { val = 0;   }
        if (val>255) { val = 255; }
        rgb_src_img_i[y][x] = RGB_Pixel(val,val,val);
      }
    }
    // Bild in AVI-Datei einschreiben
    avi << rgb_src_img_i;

    // Frequenzkoeffizienten schrittweise umkopieren und Teil-FFT-Bild ruecktransformieren
    for (int f=1; f<=width/2; f++) {
      cout << "i" << flush;
      // Quadratische Schicht um 0,0 mit Abstand f aus fft_img in fft_img_inc umkopieren,
      // dann inverse Fouriertransformation vornehmen.
      // -------------------------------------------------------------------------------
      // Parameter:  fft_img     - Fouriertransformiertes Bild in Zeilen und Spalten
      // Hilfsbild:  fft_img_inc - Fouriertransformiertes Bild mit einem Teil der Koeffizienten
      // Ergebnis:   src_img_i   - Rücktransformiertes Bild eines Teils der Koeffizienten
      // ==== Ihr Code ==== //

      // complex-Bild src_img_i im Originalraum in Grauwertbild in RGB-Darstellung konvertieren
      for (unsigned int y=0; y<height; y++) {
        for (unsigned int x=0; x<width; x++) {
          int val = 256.0*src_img_i[y][x].real();
          if (val<0)   { val = 0;   }
          if (val>255) { val = 255; }
          rgb_src_img_i[y][x] = RGB_Pixel(val,val,val);
        }
      }
      // Bild in AVI-Datei einschreiben
      avi << rgb_src_img_i;
    }
  } catch (char const* message) {
    cerr << message << endl;
  }

  return 0;
}
