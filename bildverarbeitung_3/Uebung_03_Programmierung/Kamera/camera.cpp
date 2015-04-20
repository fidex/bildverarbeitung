#include <iostream>
#include <conio.h>
#include <camera.h>
#include <SocketWrite.h>
#include <SocketRead.h>
#include <RGB_Pixel.h>
#include <Img.h>


using std::cout;
using std::endl;
using std::cerr;

static const unsigned int IMG_WIDTH = 1280;
static const unsigned int IMG_HEIGHT = 1024;
static const unsigned int CAM_TCP_PORT = 2909;
static const char * SHOW_ADDRESS_STR = "127.0.0.1";
static const unsigned int SHOW_TCP_PORT = 29978;

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
    state_t state(gRg_PIXEL);
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


Img<RGB_Pixel> get_rgb_img_from_camera(const char* cam_address_str)
{
	cout << "Kamera-IP-Adresse: " << cam_address_str << endl;

	SocketWrite sw(SHOW_ADDRESS_STR, SHOW_TCP_PORT, cam_address_str);
	SocketRead sr(cam_address_str, CAM_TCP_PORT);

	Img<unsigned char> img_bayer(IMG_WIDTH, IMG_HEIGHT);
	Img<RGB_Pixel> img_color(IMG_WIDTH, IMG_HEIGHT);

	cout << "Aufnahme mit <Leertaste>" << endl;

	char c = '\0';
	do {
		do {
			try {
				sr >> img_bayer;
			} catch (const char *e) {
				cerr << e << endl;
				cout << "Aufnahme mit <Leertaste>" << endl;
			}
			bayer2rgb(img_bayer, img_color);
			sw << img_color;
		} while(!kbhit());
		c = getch();
	} while(' ' != c);

	cout << "Bild aufgenommen" << endl;

	return img_color;
}
