#include <stdint.h>
#include "iep.h"
#include "IEPSocket.h"
#include "SocketRead.h"

SocketRead::SocketRead(const char* address, unsigned short port)
{
    WSADATA wsa;
    long startup_result = WSAStartup(MAKEWORD(2, 0), &wsa);
    if(startup_result) {
        throw "ERROR: WSAStartup()";
    }

    memset(&cam_sockaddr, 0, sizeof(SOCKADDR_IN));
    cam_sockaddr.sin_family = AF_INET;
    cam_sockaddr.sin_port = htons(port);
    cam_sockaddr.sin_addr.s_addr = inet_addr(address);
    if(INADDR_NONE == cam_sockaddr.sin_addr.s_addr) {
        throw "ERROR: inet_addr()";
    }
}

SocketRead::~SocketRead()
{
    WSACleanup();
}

void SocketRead::getimg(Img<RGB_Pixel> & img, colormode_t colormode)
{
    SOCKET s;
    int height(img.Height());
    int width(img.Width());

    s = socket(AF_INET, SOCK_STREAM, 0);
    if(INVALID_SOCKET == s) {
        throw "ERROR: socket()";
    }

    long connect_result = connect(s, (SOCKADDR*)&cam_sockaddr, sizeof(SOCKADDR));
    if(SOCKET_ERROR == connect_result) {
        throw "ERROR: connect()";
    }

    IEPSocket sock(s);

    iep_header_t msg1;
    memset(&msg1, 0, sizeof(msg1));
	msg1.msg = htons(IEP_RQ_GETIMG);
	msg1.type = htons(IEP_8UC3);
	switch(colormode) {
		case RGB:
			msg1.color = htons(IEP_COLOR_RGB);
			break;
		case BGR:
			msg1.color = htons(IEP_COLOR_BGR);
			break;
		default:
			throw "colormode not supported";
	}
	msg1.width = htons(img.Width());
	msg1.height = htons(img.Height());

    sock.send_header(msg1);
	if(!sock.is_connected()) {
		throw "Error while sending IEP header";
	}

	iep_header_t rp;
    sock.receive_header(rp);
	if(!sock.is_connected()) {
		throw "Error while receiving IEP data";
	}

	if(rp.handle == htons(IEP_ERR)) {
		throw "Source can not fulfill this request";
	}

    uint8_t *img_bgr = new uint8_t[3*height*width];


    sock.receive_data(img_bgr, 3*height*width);
	if(!sock.is_connected()) {
		throw "Error while receiving IEP data";
	}

	// convert image to IPTLIB image
    unsigned char *cur = img_bgr;
    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
        	RGB_Pixel p;
        	switch(colormode) {
				case BGR:
					p.Blue(*(cur++));
					p.Green(*(cur++));
					p.Red(*(cur++));
					break;
				case RGB:
					p.Red(*(cur++));
					p.Green(*(cur++));
					p.Blue(*(cur++));
					break;
				case BAYER:
					throw "Not Implemented yet.";
					break;
				case GRAY:
					throw "Not Implemented yet.";
					break;
        	}
            img[height - y - 1][x] = p;
        }
    }

    delete[] img_bgr;
}

void SocketRead::getimg(Img<unsigned char> & img, colormode_t colormode)
{
    SOCKET s;
    int height(img.Height());
    int width(img.Width());

    s = socket(AF_INET, SOCK_STREAM, 0);
    if(INVALID_SOCKET == s) {
        throw "ERROR: socket()";
    }

    long connect_result = connect(s, (SOCKADDR*)&cam_sockaddr, sizeof(SOCKADDR));
    if(SOCKET_ERROR == connect_result) {
        throw "ERROR: connect()";
    }

    IEPSocket sock(s);

    iep_header_t msg1;
    memset(&msg1, 0, sizeof(msg1));
	msg1.msg = htons(IEP_RQ_GETIMG);
	msg1.type = htons(IEP_8UC1);
	switch(colormode) {
		case BAYER:
			msg1.color = htons(IEP_COLOR_BAYER);
			break;
		case GRAY:
			msg1.color = htons(IEP_COLOR_GRAY);
			break;
		default:
			throw "colormode not supported";
	}
	msg1.width = htons(img.Width());
	msg1.height = htons(img.Height());

    sock.send_header(msg1);
	if(!sock.is_connected()) {
		throw "Error while sending IEP header";
	}

	iep_header_t rp;
    sock.receive_header(rp);
	if(!sock.is_connected()) {
		throw "Error while receiving IEP data";
	}

	if(rp.handle == htons(IEP_ERR)) {
		throw "Source can not fulfill this request";
	}

    uint8_t *img_bayer = new uint8_t[height*width];


    sock.receive_data(img_bayer, height*width);
	if(!sock.is_connected()) {
		throw "Error while receiving IEP data";
	}

	// convert image to IPTLIB image
    unsigned char *cur = img_bayer;
    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            img[height - y - 1][x] = *(cur++);
        }
    }

    delete[] img_bayer;
}

const SocketRead& SocketRead::operator>> (Img<RGB_Pixel>& img)
{
	getimg(img, RGB);
    return *this;
}

const SocketRead& SocketRead::operator>> (Img<unsigned char>& img)
{
	getimg(img, BAYER);
    return *this;
}


void SocketRead::quitsrv()
{
    SOCKET s;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if(INVALID_SOCKET == s) {
        throw "ERROR: socket()";
    }

    long connect_result = connect(s, (SOCKADDR*)&cam_sockaddr, sizeof(SOCKADDR));
    if(SOCKET_ERROR == connect_result) {
        throw "ERROR: connect()";
    }

    IEPSocket sock(s);

    iep_header_t rq;
    memset(&rq, 0, sizeof(rq));
	rq.msg = htons(IEP_RQ_QUITSRV);
    sock.send_header(rq);
	if(!sock.is_connected()) {
		throw "Error while sending IEP header";
	}

	iep_header_t rp;
    sock.receive_header(rp);
	if(!sock.is_connected()) {
		throw "Error while receiving IEP data";
	}

	if((rp.msg != htons(IEP_RP_QUITSRV))) {
		throw "Source can not fulfill this request";
	}
}

