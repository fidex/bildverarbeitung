//------------------------------------------------------------------------------------------
// Funktionen zur Ermittlung von Kantenbildern
// (c) Prof. Dr.-Ing. Bernhard Lang
//     HS Osnabrück
//------------------------------------------------------------------------------------------
#include <utility>
#include <cmath>
using namespace std;

#include <Img.h>

//---------------------------------------------------------------------------
// Spezielle Ausgaberoutine für Polarbilder:
//---------------------------------------------------------------------------
template <typename Pixel>
Img<RGB_Pixel> &Polar_to_PseudoColor(Img<pair<Pixel,Pixel> >& GradientImage)  {
  static Img<RGB_Pixel> RGB_out;
  RGB_out.Resize(GradientImage.Width(),GradientImage.Height());

  Pixel& p0_mag = GradientImage[0][0].first;
  Pixel min = p0_mag;
  Pixel Max = p0_mag;
  for (unsigned int y=0;y<GradientImage.Height();y++) {
    for (unsigned int x=0;x<GradientImage.Width();x++) {
      Pixel& Pixel_mag = GradientImage[y][x].first;
      if      (Pixel_mag>Max) { Max = Pixel_mag; }
      else if (Pixel_mag<min) { min = Pixel_mag; }
    }
  }
  for (unsigned int x=0;x<GradientImage.Width();x++) {
    for (unsigned int y=0;y<GradientImage.Height();y++) {
      RGB_Pixel& rgb_Pixel = RGB_out[y][x];
      Pixel& Pixel_mag   = GradientImage[y][x].first;
      Pixel& Pixel_Phase = GradientImage[y][x].second;
      // Bei Betrag 0 ist die Grundfarbe weiss. Gradienten werden durch Farben steigender Intensität gekennzeichnet
      rgb_Pixel.Red(static_cast<unsigned char>(255.0*(2.0-(Pixel_mag-min)/(Max-min)*(-cos(Pixel_Phase)+1))*0.5)+0.5);
      rgb_Pixel.Green(static_cast<unsigned char>(255.0*(2.0-(Pixel_mag-min)/(Max-min)*(-cos(Pixel_Phase-2.0*M_PI/3.0)+1))*0.5)+0.5);
      rgb_Pixel.Blue(static_cast<unsigned char>(255.0*(2.0-(Pixel_mag-min)/(Max-min)*(-cos(Pixel_Phase-4.0*M_PI/3.0)+1))*0.5)+0.5);
    }
  }
  return RGB_out;
}

//---------------------------------------------------------------------------
// Sobel Filter
//---------------------------------------------------------------------------
template<class Pixel>
Img<pair<Pixel,Pixel> >& Sobel(Img<Pixel> src_img) {
  static Img<pair<Pixel,Pixel> > sobel_img;
  sobel_img.Resize(src_img.Width(),src_img.Height());

  // Binominialfilterung in y-Richtung
  Img<Pixel> binominial_y(src_img.Width(),src_img.Height());
  for (unsigned int x=0;x<src_img.Width();x++) {
    binominial_y[0][x] =src_img[0][x] + src_img[0][x]*2.0 + src_img[1][x];
    for (unsigned int y=1;y<src_img.Height()-1;y++) {
      binominial_y[y][x] = src_img[y-1][x] + src_img[y][x]*2.0 + src_img[y+1][x];
    }
    binominial_y[src_img.Height()-1][x] = src_img[src_img.Height()-2][x] + src_img[src_img.Height()-1][x]*2.0
                                  + src_img[src_img.Height()-1][x];
  }
  // Ableitung in x-Richtung
  for (unsigned int y=0;y<src_img.Height();y++) { // Am Rand nur durch 4.0 dividieren, da Pixelabstand nur 1
    sobel_img[y][0].first = (binominial_y[y][1] - binominial_y[y][0]) / 4.0;
    for (unsigned int x=1;x<src_img.Width()-1;x++) {
      sobel_img[y][x].first = (binominial_y[y][x+1] - binominial_y[y][x-1]) / 8.0;
    }
    sobel_img[y][src_img.Width()-1].first = (binominial_y[y][src_img.Width()-1] - binominial_y[y][src_img.Width()-2]) / 4.0;
  }

  // Binominialfilterung in x-Richtung
  Img<Pixel> binominial_x(src_img.Width(),src_img.Height());
  for (unsigned int y=0;y<src_img.Height();y++) {
    binominial_x[y][0] =  src_img[y][0] + src_img[y][0]*2.0 + src_img[y][1];
    for (unsigned int x=1;x<src_img.Width()-1;x++) {
      binominial_x[y][x] =  src_img[y][x-1] + src_img[y][x]*2.0 + src_img[y][x+1];
    }
    binominial_x[y][src_img.Width()-1] =  src_img[y][src_img.Width()-2] + src_img[y][src_img.Width()-1]*2.0
                                  + src_img[y][src_img.Width()-1];
  }
  // Ableitung in y-Richtung
  for (unsigned int x=0;x<src_img.Width();x++) { // Am Rand nur durch 4.0 dividieren, da Pixelabstand nur 1
    sobel_img[0][x].second = (binominial_x[1][x] - binominial_x[0][x]) / 4.0;
    for (unsigned int y=1;y<src_img.Height()-1;y++) {
      sobel_img[y][x].second = (binominial_x[y+1][x] - binominial_x[y-1][x]) / 8.0;
    }
    sobel_img[src_img.Height()-1][x].second = (binominial_x[src_img.Height()-1][x] - binominial_x[src_img.Height()-2][x]) / 4.0;
  }

  return sobel_img;
}

//--------------------------------------------------------------------------------------------------
// Konvertierung in Polarkoordinaten (Exakter Betrag und Phase im Bereich 0..2*Pi
//--------------------------------------------------------------------------------------------------
template<class Pixel>
Img<pair<Pixel,Pixel> >& ToPolar(Img<pair<Pixel,Pixel> > src_img) {
  static Img<pair<Pixel,Pixel> > polar_img;
  polar_img.Resize(src_img.Width(),src_img.Height());
  for (unsigned int x=0;x<src_img.Width();x++) {
    for (unsigned int y=0;y<src_img.Height();y++) {
      Pixel& sx = src_img[y][x].first;
      Pixel& sy = src_img[y][x].second;
      Pixel& mag = polar_img[y][x].first;
      Pixel& phi = polar_img[y][x].second;
      mag  = sqrt(sx*sx+sy*sy);
      if (mag==0.0)   { phi = 0; } // Betrag ist 0, Phase wird zu 0 gesetzt
      else if (sy>=0) { phi = acos(sx/mag);            } // 1.-2. Quadrant
      else            { phi = 2*M_PI - acos((sx)/mag); } // 3.-4. Quadrant
    }
  }
  return polar_img;
}
