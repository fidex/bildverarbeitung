#ifndef _SocketRead_H_
#define _SocketRead_H_

#include <winsock2.h>
#include <Img.h>
#include <RGB_Pixel.h>

class SocketRead {
    private:
        SOCKADDR_IN cam_sockaddr;
        SocketRead& operator= (const SocketRead&); // Zuweisung sperren
        SocketRead(const SocketRead&); // Kopierkonstruktor sperren
    public:
	typedef enum {BAYER, GRAY, RGB, BGR} colormode_t;
        SocketRead(const char* address, unsigned short port);
        ~SocketRead();
        const SocketRead& operator>> (Img<unsigned char>& img);
        const SocketRead& operator>> (Img<RGB_Pixel>& img);
        void getimg(Img<RGB_Pixel> & img, colormode_t colormode);
        void getimg(Img<unsigned char> & img, colormode_t colormode);
        void quitsrv();
};

#endif // #ifndef _SocketRead_H_
