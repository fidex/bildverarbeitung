#ifndef __CAMERA_H
#define __CAMERA_H

template<class T> class Img;
class RGB_Pixel;

Img<RGB_Pixel> get_rgb_img_from_camera(const char* cam_address_s);

#endif
