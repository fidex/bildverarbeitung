// --------------------------------------------------------------------------------
// Uebung 4: Labelling, Merkmale
// B. Lang, HS-Osnabrueck
// Angepasst M. Weinahrdt, 04.12.13, 20.05.14
// --------------------------------------------------------------------------------


#include <iostream>
#include <cmath>
#include <queue>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <iomanip>
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


// ------------------------------------------------------------------------------------------------
// Aus Uebung 4 uebernehmen
// Optimale Schwelle
// ------------------------------------------------------------------------------------------------
/*
Img<bool> optimal_threshold(const Img<unsigned char> &gray_image) {
  const unsigned int Breite = gray_image.Width();
  const unsigned int Hoehe  = gray_image.Height();
  Img<bool> binary_image(Breite,Hoehe);
  cerr << "Funktion optimal_threshold aus Uebung 4 uebernehmen" << endl;
  return binary_image;
}
*/

Img<bool> optimal_threshold(const Img<unsigned char> &gray_image) {
  const unsigned int Breite = gray_image.Width();
  const unsigned int Hoehe  = gray_image.Height();
  Img<bool> binary_image(Breite,Hoehe);

    int threshold = calculateThreshold(gray_image);

    unsigned char maxValue = 0;
    for(int x = 0; x < Breite; x++){
        for(int y = 0; y < Hoehe; y++){
                if(gray_image[y][x] > maxValue){
                    maxValue = gray_image[y][x];
                }
        }
    }
    cout << "maxValue: " << (unsigned int) maxValue << endl;
    //threshold = maxValue / 2;

    for(int x = 0; x < Breite; x++){
        for(int y = 0; y < Hoehe; y++){
            const unsigned char &p = gray_image[y][x];
            binary_image[y][x] = (p > threshold);
        }
    }

  return binary_image;
}

int calculateThreshold(const Img<unsigned char> &img){
    // Calculate histogram
    long histogram[256];

    for(int i = 0; i < 256; i++){
        histogram[i]=0L;
    }

    for(int x = 0; x < img.Width(); x++){
        for(int y =0; y < img.Height(); y++){
            histogram[img[y][x]]++;
        }
    }

    // Calculate sums
    double S1[256];
    double S2[256];

    S1[0] = histogram[0];
    S2[0] = 0;

    for(int i = 1; i < 256; i++){
        S1[i] = S1[i-1] + histogram[i];
        S2[i] = S2[i-1] + i * histogram[i];
    }

    // Calculate quality criterion
    double pixels = img.Width() * img.Height();
    double average = S2[256-1]/pixels;
    double j[256];
    double maximum = 0.0;
    int threshold = 0;

    cout << "pixels: " << pixels << endl;
    cout << "average: " << average << endl;

    for(int i = 0; i < 256; i++){
        if((0 == S1[i]) || (S1[i] == pixels)){
            j[i] = 0;
        }
        else{
            double t = (S2[i] - S1[i] * average);
            j[i] = (t*t)/(S1[i] * (pixels - S1[i]));
        }

        if(j[i] > maximum){
            maximum = j[i];
            threshold = i;
            //cout << "treshold changed: " << threshold << endl;
        }
    }
    cout << "threshold: " << threshold << endl;
    return threshold;
}

// ---------------------------------------------------------------------------
// Aus Uebung 4 uebernehmen
// Vektor mit relativen Positionen der Pixel eines quadratischen SEs fuellen
// ---------------------------------------------------------------------------
/*
vector<pair<int,int> > create_square_SE(const unsigned int Diameter) {
  vector<pair<int,int> > ImageWindow;
  cerr << "Funktion create_square_SE aus Uebung 4 uebernehmen" << endl;
}
*/

vector<pair<int,int> > create_square_SE(const unsigned int Diameter) {
  vector<pair<int,int> > ImageWindow;
  const int Radius = Diameter/2;
  for (unsigned int dy = 0; dy<Diameter; dy++) {
    for (unsigned int dx = 0; dx<Diameter; dx++) {
      ImageWindow.push_back(pair<int,int>(dx-Radius,dy-Radius));
    }
  }
   print_SE(ImageWindow);
  return ImageWindow;
}

void print_SE(vector<pair<int, int> > ImageWindow)
{
        int MinY = ImageWindow[0].first;
        int MaxY = ImageWindow[0].first;
        int MinX = ImageWindow[0].second;
        int MaxX = ImageWindow[0].second;

        // Extreme bestimmen
        for(unsigned int i = 1; i < ImageWindow.size(); i++) {
                if(ImageWindow[i].first < MinY)  MinY = ImageWindow[i].first;
                if(ImageWindow[i].first > MaxY)  MaxY = ImageWindow[i].first;
                if(ImageWindow[i].second < MinX) MinX = ImageWindow[i].second;
                if(ImageWindow[i].second > MaxX) MaxX = ImageWindow[i].second;
        }

        int Width  = MaxX - MinX + 1;
        int Height = MaxY - MinY + 1;

        char se_bitmap[Height][Width];
        memset(se_bitmap, ' ', sizeof(se_bitmap));
        for(unsigned int i = 0; i < ImageWindow.size(); i++) {
                pair<int, int> Position = ImageWindow[i];
                if((0 == Position.first) && (0 == Position.second))
                        se_bitmap[Position.first - MinY][Position.second - MinX] = '#';
                else
                        se_bitmap[Position.first - MinY][Position.second - MinX] = '-';
        }

        // Ausgabe
        for(int y = Height - 1; y >= 0; y--) {
                for(int x = 0; x < Width; x++)
                        cout << se_bitmap[y][x];
                cout << endl;
        }
}

// ---------------------------------------------------------------------------
// Aus Uebung 4 uebernehmen
// Erosion
// ---------------------------------------------------------------------------
/*
template<typename Pixel>
Img<Pixel> erode(const Img<Pixel> &src, const vector<pair<int,int> > &ImageWindow) {
  Img<Pixel> eroded(src.Width(),src.Height());
  cerr << "Template erode aus Uebung 4 uebernehmen" << endl;
  return eroded;
}
*/

template<typename Pixel>
Img<Pixel> erode(const Img<Pixel> &src, const vector<pair<int,int> > &ImageWindow) {
  Img<Pixel> eroded(src.Width(),src.Height());


  for(int y=0;y<src.Height();y++){
    for(int x=0;x<src.Width();x++){
                int minimum = src[y+ ImageWindow[0].second][x + ImageWindow[0].first];
        for(int i=1;i<ImageWindow.size();i++){
                int window_x = x + ImageWindow[i].first;
                int window_y = y + ImageWindow[i].second;
                if(src[window_y][window_x] < minimum){
                    minimum = src[window_y][window_x];
                }
        }

                eroded[y][x] = minimum;

    }
  }
  //cout << "Aufgabe 2: 'erode' noch nicht kodiert" << endl;
  return eroded;
}

// ---------------------------------------------------------------------------
// Aus Uebung 4 uebernehmen
// Dilation
// ---------------------------------------------------------------------------
/*
template<typename Pixel>
Img<Pixel> dilate(const Img<Pixel> &src, const vector<pair<int,int> > &ImageWindow) {
  Img<Pixel> dilated(src.Width(),src.Height());
  cerr << "Template dilate aus Uebung 4 uebernehmen" << endl;
  return dilated;
}
*/

template<typename Pixel>
Img<Pixel> dilate(const Img<Pixel> &src, const vector<pair<int,int> > &ImageWindow) {
  Img<Pixel> dilated(src.Width(),src.Height());

  for(int y=0;y<src.Height();y++){
    for(int x=0;x<src.Width();x++){
                int maximum = src[y+ ImageWindow[0].second][x + ImageWindow[0].first];
        for(int i=1;i<ImageWindow.size();i++){
                int window_x = x + ImageWindow[i].first;
                int window_y = y + ImageWindow[i].second;
                if(src[window_y][window_x] > maximum){
                    maximum = src[window_y][window_x];
                }

        }

                dilated[y][x] = maximum;

    }
  }

  //cout << "Aufgabe 2: 'dilate' noch nicht kodiert" << endl;
  return dilated;
}

// ---------------------------------------------------------------------------
// Aus Uebung 4 uebernehmen
// Bilddifferenz
// ---------------------------------------------------------------------------
/*
template<typename Pixel>
Img<Pixel> operator-(const Img<Pixel> &l, const Img<Pixel> &r) {
  Img<Pixel> d(l.Width(),l.Height());
  cerr << "Template operator- aus Uebung 4 uebernehmen" << endl;
  return d;
}
*/
template<typename Pixel>
Img<Pixel> operator-(const Img<Pixel> &l, const Img<Pixel> &r) {
  Img<Pixel> d(l.Width(),l.Height());
  cout << "Aufgabe 5: 'operator -' noch nicht kodiert" << endl;

  for(int x = 0; x < l.Width(); x++){
    for(int y = 0; y < l.Height(); y++){
        d[y][x] = l[y][x] - r[y][x];
    }
  }
  return d;
}

// ------------------------------------------------------------------------------
// Aufgabe 1:
// Labelbild berechnen
// ------------------------------------------------------------------------------
unsigned int Labelling(Img<unsigned int>& label_image, vector <pair<int, int> >& touch_points,
     vector<unsigned int> &object_sizes, const unsigned int connectivity, const Img<bool>& binary_image)
{
    const Img<bool>& v16(binary_image);
    const unsigned int v0  = v16.Width();
    const unsigned int v1 = v16.Height();
    const unsigned int& v15(connectivity);
    if ((4 != v15) && (8 != v15)) {
        return -1;
    }
    unsigned int v2(0);
    vector <unsigned int> v3;
    Img<unsigned int>&  v12(label_image);
    v12.Resize(v0, v1);
    v12.Margin_Constant(0);
    v3.push_back(0);
    for(unsigned int v4 = 0; v4 < v1; v4++) {
        for(unsigned int v5  = 0; v5 < v0; v5++) {
            v12[v4][v5] = 0;
        }
    }
    vector <pair<int, int> >& v13(touch_points);
    for(unsigned int v4 = 0; v4 < v1; v4++) {
        for(unsigned int v5 = 0; v5 < v0; v5++) {
            if(v16[v4][v5]) {
                vector<unsigned int> v6;
                if(unsigned int v11 = v12[v4 - 1][v5    ]) {
                    v6.push_back(v11);
                }
                if(unsigned int v11 = v12[v4    ][v5 - 1]) {
                    v6.push_back(v11);
                }
                if(8 == v15) {
                    if(unsigned int v11 = v12[v4 - 1][v5 - 1]) {
                        v6.push_back(v11);
                    }
                    if(unsigned int v11 = v12[v4 - 1][v5 + 1]) {
                        v6.push_back(v11);
                    }
                }
                if(0 == v6.size()) {
                    v2++;
                    v12[v4][v5] = v2;
                    v3.push_back(v2);
                    v13.push_back(pair<int, int> (v5, v4));
                } else {
                    unsigned int v7 = v6.at(0);
                    for(unsigned int v10 = 0; v10 < v6.size(); v10++) {
                        unsigned int v8 = v6.at(v10);
                        if(v8 < v7) {
                            v7 = v8;
                        }
                    }
                    if(v3.at(v7) < v7) {
                        v7 = v3.at(v7);
                    }
                    v12[v4][v5] = v7;
                    for(unsigned int v18 = 0; v18 < v6.size(); v18++) {
                        unsigned int v8 = v6.at(v18);
                        v3.at(v8) = v7;
                    }
                }
            }
        }
    }
    for(unsigned int v17 = 0; v17 < v3.size(); v17++) {
        unsigned int v18 = v17;
        while(v18 != v3[v18]) {
            v18 = v3[v18];
        }
        v3[v17] = v18;
    }
    v2 = 0;
    for(unsigned int i = 0; i < v3.size(); i++) {
        if(v3[i] > v2) {
            v2++;
            unsigned int v9 = v3[i];
            for(unsigned int j = i; j < v3.size(); j++) {
                if(v3[j] == v9) {
                    v3[j] = v2;
                }
            }
            v13[v2 - 1] = v13[v9 - 1];
        }
    }
    vector<unsigned int>& v14(object_sizes);
    v14.resize(v2, 0);
    for(unsigned int v4 = 0; v4 < v1; v4++) {
        for(unsigned int v5 = 0; v5 < v0; v5++) {
            v12[v4][v5] = v3[v12[v4][v5]];
            if(v12[v4][v5] > 0) {
                v14[v12[v4][v5] - 1] += 1;
            }
        }
    }
    return v2;
}


// ----------------------------------------------------------------------------------------------------
// Vektor verschiedener RGB-Farben fuer die Label eines Labelbilds erzeugen
// ----------------------------------------------------------------------------------------------------
vector<RGB_Pixel> create_LabelColors(const vector<unsigned int> object_sizes) {
  // Farbe wird abhaengig von der Objektgroesse vergeben
  vector<unsigned int> sort_object_sizes(object_sizes);
  sort(sort_object_sizes.begin(),sort_object_sizes.end());
  unsigned int all_obj_pixels(0);
  for (unsigned int i=0; i<object_sizes.size(); i++) { all_obj_pixels += object_sizes[i]; }
  map<unsigned int,unsigned int> size_to_int;
  vector<RGB_Pixel> sort_colors; // enthaelt Farben in Reihenfolge der Objektgroessen
  unsigned int obj_pixels(0);
  for (unsigned int i=0; i<sort_object_sizes.size(); i++) {
    obj_pixels += sort_object_sizes[i];
    double phase((2.0/3.0)*2.0*M_PI*double(obj_pixels)/double(all_obj_pixels));
    unsigned int blue  = static_cast<unsigned int>(255*(sin(phase+M_PI/2.0+0.0*2*M_PI/3.0)+1.0)/2.0+0.5);
    unsigned int green = static_cast<unsigned int>(255*(sin(phase+M_PI/2.0+2.0*2*M_PI/3.0)+1.0)/2.0+0.5);
    unsigned int red   = static_cast<unsigned int>(255*(sin(phase+M_PI/2.0+1.0*2*M_PI/3.0)+1.0)/2.0+0.5);
    sort_colors.push_back(RGB_Pixel(red,green,blue));
    size_to_int[sort_object_sizes[i]] = i;
  }
  // Farben werden nun nach Labelnummern sortiert
  vector<RGB_Pixel> colors;
  for (unsigned int i=0; i<object_sizes.size(); i++) {
    colors.push_back(sort_colors[size_to_int[object_sizes[i]]]);
  }
  return colors;
}


// ----------------------------------------------------------------------------------------------------
// Vektor aufsteigender RGB-Farben einen Vektor mit Werten erzeugen
// ----------------------------------------------------------------------------------------------------
template<typename VT>
vector<RGB_Pixel> Values2ColorVector(const vector<VT> values) {
  // Farbe wird abhaengig von der Objektgroesse vergeben
  vector<VT> sort_values(values);
  sort(sort_values.begin(),sort_values.end());
  VT Max(values[0]);
  for (unsigned int i=1; i<values.size(); i++) { if (values[i]>Max) { Max=values[i]; } }
  map<VT,unsigned int> size_to_int;
  vector<RGB_Pixel> sort_colors; // enthaelt Farben in Reihenfolge der Objektgroessen
  for (unsigned int i=0; i<sort_values.size(); i++) {
    double phase((2.0/3.0)*2.0*M_PI*double(sort_values[i])/double(Max));
    unsigned int blue  = static_cast<unsigned int>(255*(sin(phase+M_PI/2.0+0.0*2*M_PI/3.0)+1.0)/2.0+0.5);
    unsigned int green = static_cast<unsigned int>(255*(sin(phase+M_PI/2.0+2.0*2*M_PI/3.0)+1.0)/2.0+0.5);
    unsigned int red   = static_cast<unsigned int>(255*(sin(phase+M_PI/2.0+1.0*2*M_PI/3.0)+1.0)/2.0+0.5);
    sort_colors.push_back(RGB_Pixel(red,green,blue));
    size_to_int[sort_values[i]] = i;
  }
  // Farben werden nun nach Labelnummern sortiert
  vector<RGB_Pixel> colors;
  for (unsigned int i=0; i<values.size(); i++) {
    colors.push_back(sort_colors[size_to_int[values[i]]]);
  }
  return colors;
}

// ----------------------------------------------------------------------------------------------------
// Labelbild in RGB-Bild konvertieren unter Verwendung eines Vektors,
// der den Labelwerten RGB-Farben zuordnet
// ----------------------------------------------------------------------------------------------------
Img<RGB_Pixel> Labelimage_to_RGB(const Img<unsigned int>& label_image, vector<RGB_Pixel> colors) {
  const unsigned int width  = label_image.Width();
  const unsigned int height = label_image.Height();
  Img<RGB_Pixel> Label_RGB(width,height);
  for (unsigned int y=0; y<height; y++) {
    for (unsigned int x=0; x<width; x++) {
      const unsigned int & label = label_image[y][x];
      if (label==0) { Label_RGB[y][x] = RGB_Pixel(255,255,255); }
      else {
        RGB_Pixel &color = colors[label-1];
        Label_RGB[y][x] = color;
      }
    }
  }
  return Label_RGB;
}


// ------------------------------------------------------------------------------
// Aufgabe 2:
// Anzahl der Randpixel pro Objekt berechnen
// ------------------------------------------------------------------------------
vector<unsigned int> count_MarginPixels(const Img<unsigned int>& label_image) {
  const unsigned int height = label_image.Height();
  const unsigned int width  = label_image.Width();
  // Internes Gradientenbild aus dem Labelbild berechnen:
  // - Dieses Bild enthält nur die Randpixel der Objekte.
  // - Diese Randpixel sind noch immer mit der Labelnummer gekennzeichent.
  // - Fuer das Objekt wird nachfolgend 8er-Konnektivitaet angenommen.
  Img<unsigned int> inner_gradient_label_image;
  vector<unsigned int> Pixels;  // Vektor mit Anzahl-Zählern der einzelnen Objekte

    bild x = erode(labelbild,3x3SE);
    interner_gradient = lablebild - x;

    int array[256] histogramm;

    for(each pixel in interner_gradient){
    //if( pixle_wert > schwellwert)
       histogramm[pixel_wert]++;

    }

  return Pixels;
}

// ------------------------------------------------------------------------------
// Aufgabe 3:
// Kompaktheit fuer jedes Objekt berechnen
// - Formel: (Anzahl der Randpixel)hoch 2 / Pixelanzahl
// ------------------------------------------------------------------------------
vector<double> compute_Compactness(const vector<unsigned int> &margin_pixels, const vector<unsigned int> &object_sizes) {
  const unsigned int number_of_objects = margin_pixels.size();
  if (object_sizes.size() != number_of_objects) {
    return vector<double>();
  }
  // Berechnung der Kompaktheiten
  vector<double> Compactness(number_of_objects);

  cerr << "Funktion compute_Compactness in Aufgabe 3 erstellen" << endl;

  return Compactness;
}

// ------------------------------------------------------------------------------
// Aufgabe 4:
// Anzahl der Randpixel pro Objekt berechnen
// ------------------------------------------------------------------------------
vector<double> compute_Excentricity(const Img<unsigned int> &LabelImage) {
  const unsigned int height = LabelImage.Height();
  const unsigned int width  = LabelImage.Width();
  vector<unsigned int> m00;
  vector<unsigned int> m01;
  vector<unsigned int> m10;
  vector<unsigned int> m11;
  vector<unsigned int> m02;
  vector<unsigned int> m20;
  vector<double> Excenticities;

  cerr << "Funktion compute_Excentricity in Aufgabe 4 erstellen" << endl;

  return Excenticities;
}


// ------------------------------------------------------------------------------------------------
// Hauptprogramm
// Aufrufe der Funktionen
// ------------------------------------------------------------------------------------------------
int main(int argc, char*argv[]) {

  cout << "BV-Praktikum: Uebung 5" << endl;

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
  // Binaeres Quellbild erzeugen
  // --------------------------------------------------------------------------------
  const unsigned int height = rgb.Height();
  const unsigned int width  = rgb.Width();
  //Img<bool> Binaerbild = ConvImg<bool,unsigned char>(ConvImg<unsigned char,RGB_Pixel>(rgb));
  Img<unsigned char> uc = ConvImg<unsigned char,RGB_Pixel>(rgb);
  BmpWrite((Bildname+"_uc.bmp").c_str(),uc);
  Img<bool> Binaerbild = optimal_threshold(uc); // Funktion "optimal_threshold" aus Uebung 3 verwenden
  BmpWrite((Bildname+"_bool.bmp").c_str(),Binaerbild);
  #if 1
  // Bei Bedarf Binaerbild invertieren: Objektpixel muessen "true" sein
  for (unsigned int y=0; y<height; y++) {
    for (unsigned int x=0; x<width; x++) {
      bool &p = Binaerbild[y][x];
      p = not p;
    }
  }
  #endif


  // --------------------------------------------------------------------------------
  // Referenzbild mit den Farben zum Einfaerben der Merkmale erzeugen
  // --------------------------------------------------------------------------------
  { const unsigned int H(40), B(500);
    vector<unsigned int> ReferenzWerte;
    for (unsigned int i=0; i<B; i++) {
      ReferenzWerte.push_back(i);
    }
    vector<RGB_Pixel> ReferenzFarben = Values2ColorVector<unsigned int>(ReferenzWerte);
    Img<RGB_Pixel> Referenzbild(B,H);
    for (unsigned int y=0; y<H; y++) {
      for (unsigned int x=0; x<B; x++) {
        Referenzbild[y][x] = ReferenzFarben[x];
      }
    }
    BmpWrite("Referenzbild.bmp",Referenzbild);
  }


  // ------------------------------------------------
  // Zu Aufgabe 1: Labelling des Bildes durchfuehren
  // ------------------------------------------------

  Img<unsigned int>       Labelbild;
  vector <pair<int,int> > Antastpunkte;
  vector<unsigned int>    Objektgroessen;
  int Objekte = Labelling(Labelbild, Antastpunkte, Objektgroessen, 8, Binaerbild);
  // Fehlebehandlung
  if (Objekte<0) {
    cerr << "Fehler beim Labelling" << endl;
    return 5;
  } else if (Objekte==0) {
    cerr << "Keine Objekte gefunden" << endl;
    return 2;
  }
  cout << "Gefundene Objekte:" << Objektgroessen.size() << endl;

  // Labelbild mit verschiedenen Farben fuer die Objekte erzeugen und ausgeben
  vector<RGB_Pixel> Farben = create_LabelColors(Objektgroessen);
  Img<RGB_Pixel> LabelAnzeige = Labelimage_to_RGB(Labelbild,Farben);
  for (unsigned int i=0; i<Objektgroessen.size(); i++) { // Antastpunkte schwarz einzeichnen
    LabelAnzeige[Antastpunkte[i].second][Antastpunkte[i].first] = RGB_Pixel(0,0,0);
  }
  BmpWrite((Bildname+"_Labelbild.bmp").c_str(),LabelAnzeige);

  // Objektbild erzeugen, in dem die Objekte gemaess ihrer Groesse eingefaerbt sind
  // (Einfaerbung 0:blau, Mitte:gruen, Maximum:rot)
  vector<RGB_Pixel> GroesseFarben = Values2ColorVector<unsigned int>(Objektgroessen);
  Img<RGB_Pixel> GroesseAnzeige = Labelimage_to_RGB(Labelbild,GroesseFarben);
  BmpWrite((Bildname+"_Groesse.bmp").c_str(),GroesseAnzeige);

  // ------------------------------------------------------
  // Zu Aufgabe 2: Anzahl der Randpixel fuer alle Objekte berechnen
  // ------------------------------------------------------

  // Randpixel aller Objekte zaehlen
  vector<unsigned int> RandPixel = count_MarginPixels(Labelbild);
  if (RandPixel.size()!=Objektgroessen.size()) {
    cerr << "Fehler in der Groesse der Vektoren mit Objektmerkmalen" << endl;
    return -5;
  }
  // Objektbild mit Einfaerbung gemaess Anzahl der Randpixel erzeugen
  vector<RGB_Pixel> RandPixelFarben;
  RandPixelFarben = Values2ColorVector<unsigned int>(RandPixel);
  Img<RGB_Pixel> RandPixelAnzeige = Labelimage_to_RGB(Labelbild,RandPixelFarben);
  BmpWrite((Bildname+"_Randpixel.bmp").c_str(),RandPixelAnzeige);

  // ------------------------------------------------------
  // Zu Aufgabe 3: Kompaktheit fuer alle Objekte berechnen
  // ------------------------------------------------------

  vector<double> Kompaktheiten = compute_Compactness(RandPixel, Objektgroessen);
  vector<RGB_Pixel> KompaktFarben = Values2ColorVector<double>(Kompaktheiten);
  Img<RGB_Pixel> KompaktAnzeige = Labelimage_to_RGB(Labelbild,KompaktFarben);
  BmpWrite((Bildname+"_Kompakt.bmp").c_str(),KompaktAnzeige);

  // ------------------------------------------------------
  // Aufgabe 4: Exzentrizitaet fuer alle Objekte berechnen
  // ------------------------------------------------------
  vector<double> Exzentizitaeten = compute_Excentricity(Labelbild);
  vector<RGB_Pixel> ExzentizitaetenFarben = Values2ColorVector<double>(Exzentizitaeten);
  Img<RGB_Pixel> ExzentizitaetenAnzeige = Labelimage_to_RGB(Labelbild,ExzentizitaetenFarben);
  BmpWrite((Bildname+"_Exzentizitaeten.bmp").c_str(),ExzentizitaetenAnzeige);


  return 0;
}
