#include "IEPSocket.h"

void IEPSocket::receive_header(iep_header_t &header)
{
	receive_data((uint8_t*)&header, sizeof(iep_header_t));
}

bool IEPSocket::is_connected()
{
	return CONNECTED == state;
}

void IEPSocket::receive_data(uint8_t *data, uint32_t datalen)
{
	if(!(CONNECTED==state)) {
		throw "Socket not Connected";
	}
	uint32_t len(0);
	do{
		fd_set readfds;
		int select_result;
		do {
			struct timeval timeout;
			timeout.tv_sec = 4;
			timeout.tv_usec = 0;
			FD_ZERO(&readfds);
			FD_SET(s, &readfds);
			select_result = select(0, &readfds, 0, 0, &timeout);
		} while(0 == select_result); // loop while timeouts occur

		if(SOCKET_ERROR == select_result) {
			close();
		} else {
			int recv_result = recv(s, ((char*)data) + len, datalen - len, 0);
			if(0 == recv_result) {
				close();
			} else if (SOCKET_ERROR == recv_result) {
				close();
			} else {
				len += recv_result;
				if(datalen == len) {
					state = DATA_RECEIVED;
				}
			}
		}
	} while(CONNECTED == state);

	if(DATA_RECEIVED == state) {
		state = CONNECTED;
	}
}

void IEPSocket::send_data(uint8_t *data, uint32_t datalen)
{
	if(!(CONNECTED==state)) {
		throw "Socket not Connected";
	}
	uint32_t len = 0;
	while(CONNECTED == state) {
		int send_result;
		send_result = send(s, ((char*)data) + len, datalen - len, 0);
		if(SOCKET_ERROR == send_result) {
			throw "Error while executing send()";
			close();
		} else {
			len += send_result;
			if(datalen == len) {
				state = DATA_SENT;
			}
		}
	}

	if(DATA_SENT == state) {
		state = CONNECTED;
	}
}

void IEPSocket::send_header(iep_header_t &header)
{
	send_data((uint8_t*)&header, sizeof(iep_header_t));
}

void IEPSocket::close()
{
	closesocket(s);
	state = CLOSED;
}

IEPSocket::IEPSocket(SOCKET s)
	: s(s), state(CONNECTED)
{
}


IEPSocket::~IEPSocket()
{
	close();
}


