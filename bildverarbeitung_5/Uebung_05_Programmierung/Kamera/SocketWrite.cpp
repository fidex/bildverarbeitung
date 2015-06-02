#include <cstring>
#include <Img.h>
#include <ConvImg.h>
#include <RGB_Pixel.h>
#include "SocketWrite.h"
#include "iep.h"
#include "IEPSocket.h"

void SocketWrite::imgrgb2bgrmat(Img<RGB_Pixel> src, uint8_t *dst)
{
	for(int y = 0; y < (int)src.Height(); ++y) {
		for(int x = 0; x < (int)src.Width(); ++x) {
			RGB_Pixel &p(src[src.Height()-y-1][x]);
			*(dst++) = p.Blue();
			*(dst++) = p.Green();
			*(dst++) = p.Red();
		}
	}
}

void SocketWrite::imguc2mat(Img<unsigned char> src, uint8_t *dst)
{
	#if 0
	for(int y = 0; y < src.Height(); ++y) {
		for(int x = 0; x < src.Width(); ++x) {
			*(dst++) = src[y][x];
		}
	}
	#endif
	#if 1
	for(int y = src.Height() - 1; y >= 0; --y) {
		for(int x = 0; x < (int)src.Width(); ++x) {
			*(dst++) = src[y][x];
		}
	}
	#endif
}


SocketWrite::SocketWrite(const char* addr, uint16_t port, const char * name)
{
    // initiate use of the Winsock DLL
    WSADATA wsa; // details of the Windows Sockets implementation
    if(WSAStartup(MAKEWORD(2, 0), &wsa)) {
        throw "ERROR: WSAStartup()";
	}

	// fill SOCKADDR structure
    memset(&server_addr, 0, sizeof(SOCKADDR_IN));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(addr);
    if(INADDR_NONE == server_addr.sin_addr.s_addr) {
        throw "ERROR: inet_addr()";
    }

	// establish socket connection
    SOCKET s(socket(AF_INET, SOCK_STREAM, 0));
    if(INVALID_SOCKET == s) {
        throw "ERROR: socket()";
    }

    if(SOCKET_ERROR == connect(s, (SOCKADDR*)&server_addr, sizeof(SOCKADDR))) {
        throw "ERROR: connect()";
    }

	IEPSocket sock(s);

	// fill IEP request structure
    iep_header_t rq; // memory for request
    memset(&rq, 0, sizeof(rq));
    rq.msg = htons(IEP_RQ_OPENWIN);
    size_t namelen(strlen(name)); // length of window name
    rq.datalen = htonl(namelen);

	sock.send_header(rq);
	if(!sock.is_connected()) {
		throw "Error while sending IEP header";
	}
	sock.send_data((uint8_t*)name, namelen);
	if(!sock.is_connected()) {
		throw "Error while sending IEP data";
	}

	// receive response (first header, then data)
	iep_header_t rp; // memory for response
	sock.receive_header(rp);
	if(!sock.is_connected()) {
		throw "Error while receiving IEP header";
	}

	// process response
	if(IEP_RP_OPENWIN != ntohs(rp.msg)) {
		throw "Wrong answer message type";
	}
	handle = ntohs(rp.handle);
}

SocketWrite::~SocketWrite()
{
	// establish socket connection
    SOCKET s(socket(AF_INET, SOCK_STREAM, 0));
    if(INVALID_SOCKET == s) {
        throw "ERROR: socket()";
    }

    if(SOCKET_ERROR == connect(s, (SOCKADDR*)&server_addr, sizeof(SOCKADDR))) {
        throw "ERROR: connect()";
    }

    IEPSocket sock(s);

    // fill request structure
    iep_header_t rq; // memory for request
    rq.msg = htons(IEP_RQ_CLOSEWIN);
    rq.handle = htons(handle);

	// send request (first header, then data)
	sock.send_header(rq);
	if(!sock.is_connected()) {
		throw "Error while sending IEP header";
	}

   	// receive response
	iep_header_t rp; // memory for response
	sock.receive_header(rp);
	if(!sock.is_connected()) {
		throw "Error while receiving IEP header";
	}
	if(IEP_RP_CLOSEWIN != ntohs(rp.msg)) {
		throw "Wrong answer message type";
	}
	if(IEP_OK != ntohs(rp.handle)) {
		throw "Server can not fulfill the request";
	}

    // terminate use of the Winsock 2 DLL (Ws2_32.dll).
    WSACleanup();
}

const SocketWrite& SocketWrite::operator<< (Img<RGB_Pixel>& img)
{
	const uint8_t img_channels = 3;

	// establish socket connection
    SOCKET s(socket(AF_INET, SOCK_STREAM, 0));
    if(INVALID_SOCKET == s) {
        throw "ERROR: socket()";
    }

    if(SOCKET_ERROR == connect(s, (SOCKADDR*)&server_addr, sizeof(SOCKADDR))) {
        throw "ERROR: connect()";
    }

    IEPSocket sock(s);

    // fill request structure
    iep_header_t rq; // memory for request
    memset(&rq, 0, sizeof(rq));
    uint16_t width(img.Width());
    uint16_t height(img.Height());
    uint32_t datalen(width * height * img_channels);
    rq.msg     = htons(IEP_RQ_PUTIMG);
    rq.type    = htons(IEP_8UC3);
    rq.color   = htons(IEP_COLOR_BGR);
    rq.width   = htons(width);
    rq.height  = htons(height);
    rq.datalen = htonl(datalen);
    rq.handle  = htons(handle);

	// send request (first header, then data)
	sock.send_header(rq);
	if(!sock.is_connected()) {
		throw "Error while sending IEP header";
	}

    uint8_t *data = new uint8_t[datalen];

	//memset(data, 0, datalen);
	imgrgb2bgrmat(img, data);

	sock.send_data(data, datalen);
	if(!sock.is_connected()) {
		throw "Error while sending IEP data";
	}

    delete[] data;

   	// receive response
	iep_header_t rp; // memory for response
	sock.receive_header(rp);
	if(!sock.is_connected()) {
		throw "Error while receiving IEP header";
	}
	if(IEP_RP_PUTIMG != ntohs(rp.msg)) {
		throw "Wrong answer message type";
	}
	if(IEP_OK != ntohs(rp.handle)) {
		throw "Server can not fulfill the request";
	}

    return *this;
}

const SocketWrite& SocketWrite::operator<< (Img<unsigned char>& img)
 {
	const uint8_t img_channels = 1;

	// establish socket connection
    SOCKET s(socket(AF_INET, SOCK_STREAM, 0));
    if(INVALID_SOCKET == s) {
        throw "ERROR: socket()";
    }

    if(SOCKET_ERROR == connect(s, (SOCKADDR*)&server_addr, sizeof(SOCKADDR))) {
        throw "ERROR: connect()";
    }

    IEPSocket sock(s);

    // fill request structure
    iep_header_t rq; // memory for request
    memset(&rq, 0, sizeof(rq));
    uint16_t width(img.Width());
    uint16_t height(img.Height());
    uint32_t datalen(width * height * img_channels);
    rq.msg     = htons(IEP_RQ_PUTIMG);
    rq.type    = htons(IEP_8UC1);
    rq.color   = htons(IEP_COLOR_GRAY);
    rq.width   = htons(width);
    rq.height  = htons(height);
    rq.datalen = htonl(datalen);
    rq.handle  = htons(handle);

	// send request (first header, then data)
	sock.send_header(rq);
	if(!sock.is_connected()) {
		throw "Error while sending IEP header";
	}

    uint8_t *data = new uint8_t[datalen];

	//memset(data, 0, datalen);
	imguc2mat(img, data);

	sock.send_data(data, datalen);
	if(!sock.is_connected()) {
		throw "Error while sending IEP data";
	}

    delete[] data;

   	// receive response (first header, then data)
	iep_header_t rp; // memory for response
	sock.receive_header(rp);
	if(!sock.is_connected()) {
		throw "Error while receiving IEP header";
	}
	if(IEP_RP_PUTIMG != ntohs(rp.msg)) {
		throw "Wrong answer message type";
	}
	if(IEP_OK != ntohs(rp.handle)) {
		throw "Server can not fulfill the request";
	}

    return *this;
 }

const SocketWrite& SocketWrite::operator<< (Img<bool>& img)
 {
	Img<unsigned char> imguc(img.Width(), img.Height());
	for(int y = 0; y < (int)img.Height(); ++y) {
		for(int x = 0; x < (int)img.Width(); ++x) {
			imguc[y][x] = true == img[y][x] ? 255 : 0;
		}
	}
	*this << imguc;
    return *this;
 }

