#ifndef __IMAGETOOL_H
#define __IMAGETOOL_H

//#define IMAGETOOL_OPENCV
#define IMAGETOOL_IPTLIB
//#define IMAGETOOL_WXWIDGETS

class wxImage;
class RGB_Pixel;
template <class RGB_Pixel> class Image;
namespace cv {
	class Mat;
}
void imgrgb2bgrmat(Image<RGB_Pixel> src, uint8_t *dst);
void convert(wxImage & dst, Image<RGB_Pixel> & src);
void convert(Image<RGB_Pixel> & dst, wxImage & src);
void convert(cv::Mat & dst, wxImage & src);
void convert(wxImage & dst, cv::Mat & src);
void convert(cv::Mat & dst, Image<RGB_Pixel> & src);
void convert(Image<RGB_Pixel> & dst, cv::Mat & src);
void bayer2rgb(Image<unsigned char>& src, Image<RGB_Pixel>& dst);
void imguc2mat(Image<unsigned char> src, uint8_t *dst);
void imgbool2mat(Image<bool> src, uint8_t *dst);

#endif

