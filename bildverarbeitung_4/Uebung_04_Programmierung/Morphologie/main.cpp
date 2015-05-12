// Praktikum Bildverarbeitung, Versuch 4 (Morphologie)
// HS Osnabrueck, Stand Sommersemester 2014

#include <iostream>
#include <cmath>
#include <queue>
#include <string>
using namespace std;

#include "Img.h"
#include "BmpRead.h"
#include "ConvImg.h"
#include "BmpWrite.h"
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

// ---------------------------------------------------------------------------
// Vektor mit relativen Positionen der Pixel eines quadratischen SEs fuellen
// ---------------------------------------------------------------------------
vector<pair<int,int> > create_square_SE(const unsigned int Diameter) {
  vector<pair<int,int> > ImageWindow;
  const int Radius = Diameter/2;
  for (unsigned int dy = 0; dy<Diameter; dy++) {
    for (unsigned int dx = 0; dx<Diameter; dx++) {
      ImageWindow.push_back(pair<int,int>(dx-Radius,dy-Radius));
    }
  }
  return ImageWindow;
}

// ---------------------------------------------------------------------------
// Aufgabe 3: Code erstellen
// Vektor mit relativen Positionen der Pixel eines nahezu runden SEs fuellen
// ---------------------------------------------------------------------------
vector<pair<int,int> > create_round_SE(const unsigned int Diameter) {
  vector<pair<int,int> > ImageWindow;
  cout << "Aufgabe 3: 'create_round_SE' noch nicht kodiert" << endl;
  return ImageWindow;
}

// ---------------------------------------------------------------------------
// Aufgabe 4: Code erstellen
// SE spiegeln
// ---------------------------------------------------------------------------
vector<pair<int,int> > mirror_SE (const vector<pair<int,int> > ImageWindow) {
  vector<pair<int,int> > MirroredImageWindow;
  cout << "Aufgabe 4: 'mirror_SE' noch nicht kodiert" << endl;
  return MirroredImageWindow;
}

// ---------------------------------------------------------------------------
// Aufgabe 2: Code erstellen
// Erosion
// ---------------------------------------------------------------------------
template<typename Pixel>
Img<Pixel> erode(const Img<Pixel> &src, const vector<pair<int,int> > &ImageWindow) {
  Img<Pixel> eroded(src.Width(),src.Height());
  cout << "Aufgabe 2: 'erode' noch nicht kodiert" << endl;
  return eroded;
}

// ---------------------------------------------------------------------------
// Aufgabe 2: Code erstellen
// Dilation
// ---------------------------------------------------------------------------
template<typename Pixel>
Img<Pixel> dilate(const Img<Pixel> &src, const vector<pair<int,int> > &ImageWindow) {
  Img<Pixel> dilated(src.Width(),src.Height());
  cout << "Aufgabe 2: 'dilate' noch nicht kodiert" << endl;
  return dilated;
}

// ---------------------------------------------------------------------------
// Aufgabe 4: Code erstellen
// Opening
// ---------------------------------------------------------------------------
template<typename Pixel>
Img<Pixel> opening(const Img<Pixel> &src, const vector<pair<int,int> > &ImageWindow) {
  Img<Pixel> opened;
  cout << "Aufgabe 4: 'opening' noch nicht kodiert" << endl;
  return opened;
}

// ---------------------------------------------------------------------------
// Aufgabe 4: Code erstellen
// Closing
// ---------------------------------------------------------------------------
template<typename Pixel>
Img<Pixel> closing(const Img<Pixel> &src, const vector<pair<int,int> > &ImageWindow) {
  Img<Pixel> closed;
  cout << "Aufgabe 4: 'closing' noch nicht kodiert" << endl;
  return closed;
}

// ---------------------------------------------------------------------------
// Aufgabe 5: Code erstellen
// Bilddifferenz
// ---------------------------------------------------------------------------
template<typename Pixel>
Img<Pixel> operator-(const Img<Pixel> &l, const Img<Pixel> &r) {
  Img<Pixel> d(l.Width(),l.Height());
  cout << "Aufgabe 5: 'operator -' noch nicht kodiert" << endl;
  return d;
}


// ------------------------------------------------------------------------------------------------
// Aufgabe 1: Code erstellen
// Optimale Schwelle
// ------------------------------------------------------------------------------------------------
Img<bool> optimal_threshold(const Img<unsigned char> &gray_image) {
  const unsigned int Breite = gray_image.Width();
  const unsigned int Hoehe  = gray_image.Height();
  Img<bool> binary_image(Breite,Hoehe);
  cout << "Aufgabe 1: 'optimal_threshold' noch nicht kodiert" << endl;
  return binary_image;
}

// ------------------------------------------------------------------------------------------------
// Hauptprogramm
// Aufrufe der Funktionen
// ------------------------------------------------------------------------------------------------
int main(int argc, char*argv[]) {

  cout << "BV-Praktikum: Uebung 3" << endl;


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


  // Quellbild in Luminanzbild konvertieren
  // (Luminanzbild im Wertebereich 0.0 bis 1.0)
  Img<unsigned char> src(ConvImg<unsigned char,RGB_Pixel>(rgb));
  src.Margin_Mirror();
  BmpWrite((Bildname+"_src.bmp").c_str(),src);
  cout << "." << flush;

  // --------------------------------------------------------------------------------
  // 1. Aufgabe: Optimale Schwelle
  // --------------------------------------------------------------------------------
  Img<bool> erstes_Binaerbild = optimal_threshold(src);
  BmpWrite((Bildname+"_binaer.bmp").c_str(),erstes_Binaerbild);
  cout << "." << flush;

  // --------------------------------------------------------------------------------
  // 2. Aufgabe: Erosion und Dilation mit quadratischen SE
  // --------------------------------------------------------------------------------
  const unsigned int SE_Groesse = ???; // geeigneten Wert einfuegen
  {
    // Quadratisches Bildfenster aufbauen
    vector<pair<int,int> > quadratisches_Bildfenster;
    quadratisches_Bildfenster = create_square_SE(SE_Groesse);
    cout << "." << flush;

    // Erosion durchfuehren
    Img<unsigned char> eroded = erode<unsigned char>(src,quadratisches_Bildfenster);
    BmpWrite((Bildname+"_eroded_sq.bmp").c_str(),eroded);
    cout << "." << flush;

    // Dilation durchfuehren
    Img<unsigned char> dilated = dilate<unsigned char>(src,quadratisches_Bildfenster);
    BmpWrite((Bildname+"_dilated_sq.bmp").c_str(),dilated);
    cout << "." << flush;
  }
  // --------------------------------------------------------------------------------
  // 3. Aufgabe: Erosion und Dilation mit rundem SE
  // --------------------------------------------------------------------------------
  {
    // Rundes Bildfenster aufbauen
    vector<pair<int,int> > rundes_Bildfenster;
    rundes_Bildfenster = create_round_SE(SE_Groesse);
    cout << "." << flush;

    // Erosion durchfuehren
    Img<unsigned char> eroded = erode<unsigned char>(src,rundes_Bildfenster);
    BmpWrite((Bildname+"_eroded_rd.bmp").c_str(),eroded);
    cout << "." << flush;

    // Dilation durchfuehren
    Img<unsigned char> dilated = dilate<unsigned char>(src,rundes_Bildfenster);
    BmpWrite((Bildname+"_dilated_rd.bmp").c_str(),dilated);
    cout << "." << flush;
  }

  // --------------------------------------------------------------------------------
  // 4. Aufgabe: Opening und Closing mit rundem SE
  // --------------------------------------------------------------------------------

  // Rundes Bildfenster aufbauen
  vector<pair<int,int> > rundes_Bildfenster;
  rundes_Bildfenster = create_round_SE(SE_Groesse);
  cout << "." << flush;

  // Opening durchfuehren
  Img<unsigned char> opened = opening<unsigned char>(src,rundes_Bildfenster);
  BmpWrite((Bildname+"_opened.bmp").c_str(),opened);
  cout << "." << flush;

  // Closing durchfuehren
  Img<unsigned char> closed = closing<unsigned char>(src,rundes_Bildfenster);
  BmpWrite((Bildname+"_closed.bmp").c_str(),closed);
  cout << "." << flush;


  // --------------------------------------------------------------------------------
  // 5. Aufgabe: Zylinderhut-Bilder berechnen und optimale Schwelle anwenden
  // --------------------------------------------------------------------------------

  // WZH berechnen
  Img<unsigned char> WZH;
  WZH = src-opened;
  BmpWrite((Bildname+"_WZH.bmp").c_str(),WZH);
  cout << "." << flush;
  Img<bool> WZH_bool = optimal_threshold(WZH);
  BmpWrite((Bildname+"_WZH_bool.bmp").c_str(),WZH_bool);
  cout << "." << flush;

  // SZH berechnen
  Img<unsigned char> SZH;
  SZH = closed-src;
  BmpWrite((Bildname+"_SZH.bmp").c_str(),SZH);
  cout << "." << flush;
  Img<bool> SZH_bool = optimal_threshold(SZH);
  BmpWrite((Bildname+"_SZH_bool.bmp").c_str(),SZH_bool);
  cout << "." << flush;

  cout << endl;
  return 0;
}
