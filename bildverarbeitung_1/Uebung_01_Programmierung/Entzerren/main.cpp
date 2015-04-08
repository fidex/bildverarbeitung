//------------------------------------------------------------------------
// Programm "Entzerren"
//   Das Programm dient zum Entzerren von BMP-Bildern
//   - Die Namen der zu entzerrenden Bilder werden (ohne ".bmp"-Endung)
//     dem Programm als Argumente uebergeben.
//     (Bei Code::Blocks werden die Argumente unter
//      "Project"->"Set programs' arguments..." angegeben)
//   - Die für die Entzerrung benötigten Kameraparameter werden mittels
//     der Datei "Kameraparameter.csv" uebergeben.
//
// B.Lang, HS Osnabrueck
//------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#include "Img.h"
#include "BmpRead.h"
#include "BmpWrite.h"
#include "Entzerren.h"
#include "Kameraparameter_einlesen.h"

int main(int argc, char* argv[])
{
	// Parameter einlesen (Namen der zu bearbeitenden Dateien)
	vector<string> ImageNames;
	for (int arg_number = 1; arg_number < argc; arg_number++) {
		ImageNames.push_back(string(argv[arg_number]));
	}

	// Kameraparameter von der Kalibrierung einlesen
	string ParameterFile("Kameraparameter.csv");
	double intrinsic_d[3][3]; // Intrinsische Parameter der realen (verzerrten) Kamera
	double distCoeffs[5];     // Verzerrungsparameter
	double rotVect[3];        // Rotationsvektor
	double transVect[3];      // Translationsvektor
	Kameraparameter_einlesen(ParameterFile, intrinsic_d, distCoeffs, rotVect, transVect);

	// Rotationsmatrix aus dem Rodriguez-Vektor berechnen
	double R[3][3];           // Rotationsmatrix
	RotMat_from_Rodriguez(R, rotVect);
	cout << "Rotationsmatrix zum senkrechten Ausrichten der Kamera:" << endl;
	cout << R[0][0] << ", " << R[0][1] << ", " << R[0][2] << endl;
	cout << R[1][0] << ", " << R[1][1] << ", " << R[1][2] << endl;
	cout << R[2][0] << ", " << R[2][1] << ", " << R[2][2] << endl;
	cout << endl;

	// Alle Dateien bearbeiten
	for (unsigned int ImgNum = 0; ImgNum < ImageNames.size(); ImgNum++) {

		// Name des zu entzerenden Bildes setzen
		string ImageFile(ImageNames[ImgNum]);
		cout << "Entzerre Bild: " << ImageFile + ".bmp" << endl;

		// Verzerrtes Bild einlesen
		Img<RGB_Pixel> img_d;                           // Verzerrtes Bild
		BmpRead((ImageFile + ".bmp").c_str()) >> img_d; // Verzerrtes Bild einlesen
		img_d.Margin_Constant(RGB_Pixel(0, 0, 0));      // Randbehandlung setzen

		//
		// Intrinsische Parameter der entzerrten, senkrecht ausgereichteten Kamera bestimmen
		// (Aufgabe 3)
		//
#if 1
		// 3x3-Matrix für die intrinsischen Parameter der neuen Kamera definieren und fuellen:
		// - fx vom verzerrten Bild als Brennweite in u- und in v-Richtung übernehmen
		// - cx,cy zunaechst zu 0 setzen
		double intrinsic[3][3] = {
			{intrinsic_d[0][0], 0.0 ,            0.0},
			{0.0,             intrinsic_d[0][0], 0.0},
			{0.0,             0.0,               1.0}
		};

/*03_Kamerakalibrierung.pdf 3.6.1 - 3.6.2*/

		// Vektor fuer Positionen der vier Eckpunkte des verzerrten Bilds definieren und fuellen
		vector<pair<double, double> > corners_d;
		corners_d.push_back(pair<double, double>( 0.0,                   0.0                   )); // left bottom
		corners_d.push_back(pair<double, double>( 0.0 ,                  img_d.Height()        )); // left top
		corners_d.push_back(pair<double, double>( img_d.Width() ,                  0.0           )); // right bottom
		corners_d.push_back(pair<double, double>( img_d.Width() ,                  img_d.Height())); // right top

		// Vektor fuer die Positionen der vier Eckpunkte des verzerrten Bilds im entzerrten Bild definieren
               // Vektor fuer die Positionen der vier Eckpunkte des verzerrten Bilds im entzerrten Bild definieren
        vector<pair<double, double> > corners;

                // Eckpunkte des verzerrten Bildes im unverzerrten Bild (noch mit cx=0,cy=0) bestimmen
        UndistorePoints( corners , intrinsic , corners_d , intrinsic_d , distCoeffs , rotVect );

		// Minimale um maximale Positionen der vier Eckpunkte des verzerrten Bilds
		// (umschließendes Rechteck) im entzerrten Bild bestimmen
        double leftest_x;
        double rightest_x;
        double top_y;
        double bottom_y;
         if(corners[0].first < corners[1].first){
            leftest_x = corners[1].first;
        }else{
            leftest_x = corners[0].first;
        }
        if(corners[2].first < corners[3].first){
            rightest_x = corners[3].first;
        }else{
            rightest_x = corners[2].first;
        }
        if(corners[1].second < corners[3].second){
            top_y = corners[3].second;
        }else{
            top_y = corners[1].second;
        }
        if(corners[0].second < corners[2].second){
            bottom_y = corners[0].second;
        }else{
            bottom_y = corners[2].second;
        }

        cout << "leftest_x" << leftest_x << " rightest_x:" << rightest_x << " top_y:" << top_y << " bottom_y:" << bottom_y << endl;
//		// Intrinsische Parameter cx und cy setzen und ausgeben
////		intrinsic[0][2] = ??;
////		intrinsic[1][2] = ??;
//		cout << "cx=" << intrinsic[0][2] << ", cy=" << intrinsic[1][2] << endl;
//
//		// Breite und Höhe des entzerrten Bilds ermitteln
////		unsigned int height( ?? );
////		unsigned int width ( ?? );
//		cout << "Breite=" << width << ", Hoehe=" << height << endl;

#endif

		//
		// Bild entzerren
		// (Aufgabe 4)
		//
#if 0
		// Entzerrtes "RGB_Pixel"-Bild "img" definieren
		Img<RGB_Pixel> img;

		// Entzerrung durchführen: "img" aus "img_d" berechnen
		UndistoreImage( &img, intrinsic , &img_d , intrinsic_d , distCoeffs , rotVect );

		// Entzerrtes Bild "img" wegspeichern
		cout << "Schreibe Bild: " << ImageFile + "_entzerrt.bmp" << endl;
		BmpWrite((ImageFile + "_entzerrt.bmp").c_str(), img);
		cout << endl;

#endif
	}
	return 0;
}
