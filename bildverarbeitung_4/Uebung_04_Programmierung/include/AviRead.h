//------------------------------------------------------------------------------------------
// (c) Prof. Dr.-Ing. Bernhard Lang
//     HS Osnabrück
//------------------------------------------------------------------------------------------
#include "FileIO.h"
#include "xa_avi.h"
#include "Img.h"
#include "RGB_Pixel.h"

class AviRead: public FileIn, public Align {
 private:
  int width;          // Bildbreite
  int height;         // Bildhöhe
  int framecount;     // Anzahl der Frames
  int framenumber;    // aktuelle Frame-Nummer
  //
  int BitsPerPixel;   // Anzahl der Bits pro Pixel
  unsigned char  vids_lut[256][3];
  int  BytesPerLine;  // Anzahl der Bytes pro Zeile
  int  linenumber;    // aktuelle Zeilennummer
  long MoviLength;    // Länge der "movi"-Liste
  unsigned char* buffer;
  long  buffersize;
  int   error;
  char  ErrorBuffer[200];
  long  FileLength;
  int   VideoStreamNumber;
  int   flush_data(unsigned int size);
  int   ReadJunkHeader(char* buffer, long& size);
  int   ReadTag(char* buffer);
 public:
  AviRead(const char* FileName);
  ~AviRead();
  // Status information
  int Width()       const {return width;}
  int Height()      const {return height;}
  int Frames()      const {return framecount;}
  int FrameNumber() const {return framenumber;}
  int LineNumber()  const {return linenumber;}
  //
  AviRead& operator>>(Img<RGB_Pixel> &I);
  //
  int ReadFrame(unsigned char* buffer);
  int ReadLine(unsigned char* buffer);

  const char* get_Error() const { return ErrorBuffer; }
};
