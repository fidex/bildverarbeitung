#include <stdexcept>

#include "imagetool.h"

#ifdef IMAGETOOL_WXWIDGETS
#include <wx/wx.h>
#endif

#ifdef IMAGETOOL_OPENCV
#include <opencv2/opencv.hpp>
#endif

#ifdef IMAGETOOL_IPTLIB
#include <Img.h>
#include <RGB_Pixel.h>
#endif

#if defined(IMAGETOOL_IPTLIB)
void imgrgb2bgrmat(Img<RGB_Pixel> src, uint8_t *dst)
{
	for(int y = 0; y < (int)src.Height(); ++y) {
		for(int x = 0; x < (int)src.Width(); ++x) {
			RGB_Pixel &p(src[y][x]);
			*(dst++) = p.Blue();
			*(dst++) = p.Green();
			*(dst++) = p.Red();
		}
	}
}

void imguc2mat(Img<unsigned char> src, uint8_t *dst)
{
	for(int y = 0; y < (int)src.Height(); ++y) {
		for(int x = 0; x < (int)src.Width(); ++x) {
			*(dst++) = src[y][x];
		}
	}
}

void imgbool2mat(Img<bool> src, uint8_t *dst)
{
	for(int y = 0; y < (int)src.Height(); ++y) {
		for(int x = 0; x < (int)src.Width(); ++x) {
			if(true == src[y][x]) {
				*(dst++) = 255;
			} else {
				*(dst++) = 0;
			}
		}
	}
}


void bayer2rgb(Img<unsigned char>& src, Img<RGB_Pixel>& dst)
{
    enum state_t {
        gRg_PIXEL,
        gBg_PIXEL,
        bGb_PIXEL,
        rGr_PIXEL
    };
    int rows(src.Height());
    int cols(src.Width());
    state_t state(gBg_PIXEL);
    src.Margin_Mirror();


    for(int y = 0; y < rows; ++y) {
        for(int x = 0; x < cols; ++x) {
            RGB_Pixel& dst_pixel(dst[y][x]);
			int center(src[y][x]);
			int up_left(src[y-1][x-1]);
			int up(src[y-1][x]);
			int up_right(src[y-1][x+1]);
			int left(src[y][x-1]);
			int right(src[y][x+1]);
			int down_left(src[y+1][x-1]);
			int down(src[y+1][x]);
			int down_right(src[y+1][x+1]);
            switch(state) {
                case gRg_PIXEL:
                    dst_pixel.Red(center);
                    dst_pixel.Green((up + left + right + down)/4);
                    dst_pixel.Blue((up_left + up_right + down_left + down_right)/4);
                    if(x == cols - 1) {
                        state = gBg_PIXEL;
                    } else {
                        state = rGr_PIXEL;
                    }
                    break;
				case gBg_PIXEL:
                    dst_pixel.Red((up_left + up_right + down_left + down_right)/4);
                    dst_pixel.Green((up + left + right + down)/4);
                    dst_pixel.Blue(center);
                    if(x == cols - 1) {
                        state = rGr_PIXEL;
                    } else {
                        state = bGb_PIXEL;
                    }
                    break;
				case bGb_PIXEL:
                    dst_pixel.Red((up + down)/2);
                    dst_pixel.Green(center);
                    dst_pixel.Blue((left + right)/2);
                    if(x == cols - 1) {
                        state = rGr_PIXEL;
                    } else {
                        state = gBg_PIXEL;
                    }
					break;
				case rGr_PIXEL:
                    dst_pixel.Red((left + right)/2);
                    dst_pixel.Green(center);
                    dst_pixel.Blue((up + down)/2);
                    if(x == cols - 1) {
                        state = gBg_PIXEL;
                    } else {
                        state = gRg_PIXEL;
                    }
					break;
            }
        }
    }
}
#endif

#if defined(IMAGETOOL_WXWIDGETS) && defined(IMAGETOOL_IPTLIB)
void convert(wxImage & dst, Image<RGB_Pixel> & src)
{
    int width(src.Width());
    int height(src.Height());

    assert((width == dst.GetWidth()) &&
           (height == dst.GetHeight()));

    unsigned char * data = dst.GetData();
    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            RGB_Pixel &p = src[y][x];
            *(data++) = p.Red();
            *(data++) = p.Green();
            *(data++) = p.Blue();
        }
    }

    return;
}

void convert(Image<RGB_Pixel> & dst, wxImage & src)
{
    int width(src.GetWidth());
    int height(src.GetHeight());

    assert((width == dst.Width()) &&
           (height == dst.Height()));

    unsigned char *data = src.GetData();
    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            RGB_Pixel &p = dst[y][x];
            p.Red(*(data++));
            p.Green(*(data++));
            p.Blue(*(data++));
        }
    }

    return;
}
#endif

#if defined(IMAGETOOL_WXWIDGETS) && defined(IMAGETOOL_OPENCV)
void convert(cv::Mat & dst, wxImage & src)
{
    cv::Size size = dst.size();
    int width(src.GetWidth());
    int height(src.GetHeight());

    assert((width == size.width) &&
           (height == size.height) &&
           (dst.depth() == CV_8U) &&
           (dst.channels() == 3) &&
           (dst.isContinuous()));

    memcpy(dst.ptr(), src.GetData(), width*height*3);

    return;
}

void convert(wxImage & dst, cv::Mat & src)
{
    cv::Size size = src.size();
    int width(dst.GetWidth());
    int height(dst.GetHeight());

    assert((width == size.width) &&
           (height == size.height) &&
           (src.depth() == CV_8U) &&
           (src.channels() == 3) &&
           (src.isContinuous()));

    memcpy(dst.GetData(), src.ptr(), width*height*3);

    return;
}
#endif

#if defined(IMAGETOOL_IPTLIB) && defined(IMAGETOOL_OPENCV)
void convert(cv::Mat & dst, Image<RGB_Pixel> & src)
{
    cv::Size size = dst.size();
    int width(src.Width());
    int height(src.Height());

    assert((width == size.width) &&
           (height == size.height) &&
           (dst.depth() == CV_8U) &&
           (dst.channels() == 3) &&
           (dst.isContinuous()));

    unsigned char * data = dst.ptr();
    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            RGB_Pixel &p = src[y][x];
            *(data++) = p.Blue();
            *(data++) = p.Green();
            *(data++) = p.Red();
        }
    }

    return;
}

void convert(Image<RGB_Pixel> & dst, cv::Mat & src)
{
    cv::Size size = src.size();
    int width(dst.Width());
    int height(dst.Height());

    assert((width == size.width) &&
           (height == size.height) &&
           (src.depth() == CV_8U) &&
           (src.channels() == 3) &&
           (src.isContinuous()));

    unsigned char * data = src.ptr();
    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            RGB_Pixel &p = dst[y][x];
            p.Blue(*(data++));
            p.Green(*(data++));
            p.Red(*(data++));
        }
    }
    return;
}
#endif

