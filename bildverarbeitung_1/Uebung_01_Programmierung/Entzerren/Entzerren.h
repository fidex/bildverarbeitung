#ifndef ENTZERREN_H_INCLUDED
#define ENTZERREN_H_INCLUDED
//------------------------------------------------------------------------
// Programm "Entzerren"
//   Prototypen der Unterprogramme zum Entzerren von Bildpunkten und
//   ganzen Bildern
//
// B.Lang, HS Osnabrueck
//------------------------------------------------------------------------

#include <vector>
using namespace std;

#include "RGB_Pixel.h"
#include "Img.h"

void RotMat_from_Rodriguez(double R[3][3], const double rotVect[3]);

void UndistoreImage(Img<RGB_Pixel> &img,
                    const double intrinsic[3][3],
                    Img<RGB_Pixel> &img_d,
                    const double intrinsic_d[3][3],
                    const double distCoeffs[5],
                    const double rotVect[3]);

void UndistorePoints(vector<pair<double,double> >& Points,
                     const double intrinsic[3][3],
                     const vector<pair<double,double> >& Points_d,
                     const double intrinsic_d[3][3],
                     const double distCoeffs[5],
                     const double rotVect[3]);

#endif // ENTZERREN_H_INCLUDED
