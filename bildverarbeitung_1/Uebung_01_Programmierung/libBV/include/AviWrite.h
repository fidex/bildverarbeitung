//------------------------------------------------------------------------------------------
// (c) Prof. Dr.-Ing. Bernhard Lang
//     HS Osnabrück
//------------------------------------------------------------------------------------------
#ifndef _AviWrite_h_
#define _AviWrite_h_

#include "FileIO.h"
#include "Img.h"
#include "RGB_Pixel.h"

class AviWrite: public FileOut, public Align {
 private:
  int  width;         // Bildbreite
  int  height;        // Bildhöhe
  int  framenumber;   // Aktuelle Frame-Nummer
  int  framerate;     // Bilder pro Sekunde
  int  error;
  const char* ErrorText;
  void WriteJunkHeader(const char* buffer, long size);
  void WriteTag(const char* buffer);
 public:
  AviWrite(const char* FileName, long w, long h, int rate=25);
  ~AviWrite();
  AviWrite& operator<<(Img<RGB_Pixel>&);

  const char* get_Error() const { return ErrorText; }

};

#endif

