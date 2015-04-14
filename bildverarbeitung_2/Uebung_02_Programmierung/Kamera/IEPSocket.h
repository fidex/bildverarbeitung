#ifndef _IEPSOCKET_H_
#define _IEPSOCKET_H_

#include <stdint.h>
#include <winsock2.h>
#include "iep.h"

class IEPSocket
{
	private:
		typedef enum {CONNECTED, HEADER_RECEIVED, DATA_RECEIVED, HEADER_SENT,
		              DATA_SENT, CLOSED, FATAL_ERROR} state_t;
		SOCKET s;
		state_t state;
	public:
		IEPSocket(SOCKET s);
		void send_header(iep_header_t &header);
		void receive_header(iep_header_t &header);
		void send_data(uint8_t *data, uint32_t datalen);
		void receive_data(uint8_t *data, uint32_t datalen);
		bool is_connected();
		void close();
		~IEPSocket();
};

#endif
