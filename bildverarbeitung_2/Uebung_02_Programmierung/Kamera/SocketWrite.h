#ifndef _SocketWrite_H_
#define _SocketWrite_H_

#include <stdint.h>
#include <winsock2.h>
#include <RGB_Pixel.h>

class SocketWrite
{
    private:
        SOCKADDR_IN server_addr;
        SocketWrite& operator= (const SocketWrite&); // Zuweisung sperren
        SocketWrite(const SocketWrite&); // Kopierkonstruktor sperren
        uint16_t handle;
		void imgrgb2bgrmat(Img<RGB_Pixel> src, uint8_t *dst);
		void imguc2mat(Img<unsigned char> src, uint8_t *dst);
    public:
        SocketWrite(const char* addr, uint16_t port, const char * name);
        ~SocketWrite();
        const SocketWrite& operator<< (Img<RGB_Pixel>& img);
        const SocketWrite& operator<< (Img<unsigned char>& img);
        const SocketWrite& operator<< (Img<bool>& img);
};

#endif
