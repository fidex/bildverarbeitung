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

// --------------------------------------------------------------------
// Aufgabe 1: Sobel berechnen
// --------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// Sobel in xy-Darstellung der Gradienten berechnen
// ------------------------------------------------------------------------------------------------
Img<pair<double,double> > compute_Sobel_xy(const Img<double> src) {
    const unsigned int height(src.Height());
    const unsigned int width(src.Width());
    Img<pair<double,double> > sobel_xy_img(width,height);

    for(unsigned int x=0; x < src.Width(); x++){
        for(unsigned int y=0; y < src.Height(); y++){
            sobel_xy_img[y][x].first = (src[y][x+1] - src[y][x-1])/2.0;
            sobel_xy_img[y][x].second = (src[y+1][x] - src[y-1][x])/2.0;

            /*
            if(sobel_xy_img[y][x].first != 0 || sobel_xy_img[y][x].second != 0){
                cout << "x: " << sobel_xy_img[y][x].first << ", y: " << sobel_xy_img[y][x].second << endl;
            }
            */

        }
    }

    return sobel_xy_img;
}


// --------------------------------------------------------------------
// Aufgabe 2: Gradient in Polarkoordinatendarstellung konvertieren
// --------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// Gradient aus xy-Darstellung in Polardarstellung umrechnen
// ------------------------------------------------------------------------------------------------

Img<pair<double,double> > convert_Gradient_to_Polar(Img<pair<double,double> > Gradient_xy) {
    const unsigned int height(Gradient_xy.Height());
    const unsigned int width(Gradient_xy.Width());
    cout << "height: " << height << ", width: " << width << endl;
    Img<pair<double,double> > Gradient_mp(width,height);

    for(unsigned int x=0; x < width; x++){
        for(unsigned int y=0; y < height; y++){
            double x_value = Gradient_xy[y][x].first;
            double y_value = Gradient_xy[y][x].second;

            double r = sqrt(pow(x_value, 2) + pow(y_value, 2));
            double angle  = atan2 (y_value, x_value);

            /*
            double angle;

            if(r == 0){
                angle = 0;
            }
            else if(y_value >= 0){
                angle = acos(x_value/r);
            }
            else{
                angle = 2 * M_PI - acos(x_value/r);
            }
            */

            Gradient_mp[y][x].first = r;
            Gradient_mp[y][x].second = angle;
        }
    }


    // ...
    // ... Berechnung durchfuehren
    // ...
    return Gradient_mp;
}

// --------------------------------------------------------------------
// Aufgabe 3: Winkelhistogramm erstellen
// --------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// Aus Gradientenbild in Polarkoordinatendarstellung ein spezielles Histogramm erzeugen,
// welches die Beträge über den Winkeln aufsummiert.
// ------------------------------------------------------------------------------------------------
vector<double> create_AngleHistogram(Img<pair<double,double> > Gradient_mp, unsigned int segments) {
    // Summe der Gradientenbetraege ueber Winkelsegmenten berechnen
    vector<double> Hist(segments);
    //Elemente sind null
    const unsigned int height(Gradient_mp.Height());
    const unsigned int width(Gradient_mp.Width());
    int seg;
    int phi;
    for(int y=0; y < height; y++){
        for(int x=0; x < width; x++){
            phi = Gradient_mp[y][x].second;
            seg = (int) floor((phi/(2*M_PI))*256)%256;
            Hist[seg] = Hist[seg] + Gradient_mp[y][x].first;
        }
    }


    return Hist;
}

// --------------------------------------------------------------------
// Aufgabe 4: NMS-Bild erstellen
// --------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// Gradientenbild in NMS-Darstellung errechnen
// ------------------------------------------------------------------------------------------------
Img<pair<double,double> > create_NMS_Gradient(Img<pair<double,double> > Gradient_mp) {
    const unsigned int height(Gradient_mp.Height());
    const unsigned int width(Gradient_mp.Width());
    Img<pair<double,double> > Gradient_nms(width,height);
    // ...
    // ... Berechnung durchfuehren
    // ...
    return Gradient_nms;
}

// ------------------------------------------------------------------------------------------------
// Aus Histogramm ein BMP-Bild erzeugen
// ------------------------------------------------------------------------------------------------
Img<RGB_Pixel> create_HistogramImg(vector<double> Hist,unsigned int height,unsigned int margin) {
    Img<RGB_Pixel> HistImg(Hist.size()+2*margin,height+2*margin);
    double MagMax(0.0);
    for (unsigned int i=0; i<Hist.size(); i++) {
        if (Hist[i]>MagMax) {
            MagMax = Hist[i];
        }
    }
    for (unsigned int y = 0; y < HistImg.Height(); y++) {
        for (unsigned int x = 0; x < HistImg.Width(); x++) {
            if ((x>=margin)&&(x<HistImg.Width()-margin)&&(y>=margin)&&(y<HistImg.Height()-margin)) {
                if (Hist[x-margin]*height/MagMax > y-margin) {
                    HistImg[y][x] = RGB_Pixel(255,0,0);
                }
                else                                         {
                    HistImg[y][x] = RGB_Pixel(255,255,255);
                }
            } else {
                HistImg[y][x] = RGB_Pixel(180,180,180);
            }
        }
    }
    return HistImg;
}

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


#if BILDQUELLE==BILD_von_DATEI
    if (argc<2) {
        cerr << "Dateiname nicht angegeben" << endl;
        return 5;
    }
    string Bildname(argv[1]);

    // Bild einlesen und nach double konvertieren
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
    Img<double> src(ConvImg<double,RGB_Pixel>(rgb));

    // --------------------------------------------------------------------
    // Aufgabe 1: Sobel berechnen
    // --------------------------------------------------------------------
    // Sobel in xy-Darstellung berechnen
    src.Margin_Mirror();
    Img<pair<double,double> > sobel_xy = compute_Sobel_xy(src);

    // x-Komponente als Grauwertbild anzeigen
    Img<double> sobel_x = get_first<double,double>(sobel_xy);
    BmpWrite((Bildname+"_sobel_x.bmp").c_str(),ConvImg<unsigned char,double>(sobel_x,127.0,128.0));
    // y--Komponente als Grauwertbild anzeigen
    Img<double> sobel_y = get_second<double,double>(sobel_xy);
    BmpWrite((Bildname+"_sobel_y.bmp").c_str(),ConvImg<unsigned char,double>(sobel_y,127.0,128.0));


    // --------------------------------------------------------------------
    // Aufgabe 2: Gradient in Polarkoordinatendarstellung konvertieren
    // --------------------------------------------------------------------
    // In Polarkoordinaten konvertieren
    Img<pair<double,double> > sobel_mp = convert_Gradient_to_Polar(sobel_xy);

    // Gradientenbild mit Polarkoordinaten in Pseudofarben darstellen
    BmpWrite((Bildname+"_sobel_mp.bmp").c_str(),Polar_to_PseudoColor(sobel_mp));
    // Nur Betrag als Grauwertbild anzeigen
    Img<double> sobel_mag = get_first<double,double>(sobel_mp);
    BmpWrite((Bildname+"_sobel_m.bmp").c_str(),ConvImg<unsigned char,double>(sobel_mag,255.0,0.0));
    // Nur Winkel als Grauwertbild anzeigen
    Img<double> sobel_phi = get_second<double,double>(sobel_mp);
    BmpWrite((Bildname+"_sobel_p.bmp").c_str(),ConvImg<unsigned char,double>(sobel_phi,256.0/(2.0*M_PI),0.0));

    // --------------------------------------------------------------------
    // Aufgabe 3: Winkelhistogramm erstellen
    // --------------------------------------------------------------------
    // Winkelhistogramm: Amlituden über den Winkeln aufsummieren
    Img<unsigned char> uc_sobel_phi = ConvImg<unsigned char,double>(sobel_phi,256.0/(2.0*M_PI),0.0);
    vector<double> Hist = create_AngleHistogram(sobel_mp, 256);
    // Winkelhistogramm ausgeben
    const unsigned int hist_margin(10);
    const unsigned int hist_height(256);
    Img<RGB_Pixel> HistImg = create_HistogramImg(Hist,hist_height,hist_margin);
    BmpWrite((Bildname+"_HistImg.bmp").c_str(),HistImg);

    // --------------------------------------------------------------------
    // Aufgabe 4: NMS-Bild erstellen
    // --------------------------------------------------------------------
    Img<pair<double,double> > sobel_nms = create_NMS_Gradient(sobel_mp);
    BmpWrite((Bildname+"_Sobel_nms.bmp").c_str(),Polar_to_PseudoColor(sobel_nms));

    return 0;
}
