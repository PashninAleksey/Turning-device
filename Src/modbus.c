#include "modbus.h"

extern uint32_t y_pos;
extern uint32_t x_pos;
extern uint32_t y_max;
extern uint32_t x_max;

void modbus_thread(void *arg) {

	err_t err, recv_err;
	struct netconn *conn;
	struct netbuf *inbuf;
	struct netconn *newconn;
	struct_sock *arg_sock;
	arg_sock = (struct_sock*) arg;
	conn = arg_sock->conn;
	u16_t buflen, buflen_answer;
	uint8_t* buf;
	uint8_t data[100] = { '\0' };

	for (;;) {

		err = netconn_accept(conn, &newconn);
		if (err == ERR_OK) {
			for (;;) {

				recv_err = netconn_recv(newconn, &inbuf);
				if (recv_err == ERR_OK) {
					netbuf_data(inbuf, (void**) &buf, &buflen);
					if ((buf[0] == 0x0D) || (buf[0] == 0x0A)) {
						netbuf_delete(inbuf);
						continue;
					}

					memset(data, '\0', sizeof(data));
					memcpy(data, buf, buflen);
					//read analog output
					if (data[7] == 0x3) {

						data[8] = data[11] * 2;
						data[5] = data[8] + 3;
						buflen_answer = data[5] + 6;
						//read x_max
						if (data[9] == 0 && data[11] == 2) {

							data[9] = x_max >> 24;
							data[10] = (x_max & 0xffffff) >> 16;
							data[11] = (x_max & 0xffff) >> 8;
							data[12] = x_max & 0xff;

						}
						//read y_max
						else if (data[9] == 2 && data[11] == 2) {
							data[9] = y_max >> 24;
							data[10] = (y_max & 0xffffff) >> 16;
							data[11] = (y_max & 0xffff) >> 8;
							data[12] = y_max & 0xff;

						}
						//read x_pos
						else if (data[9] == 4 && data[11] == 2) {
							data[9] = x_pos >> 24;
							data[10] = (x_pos & 0xffffff) >> 16;
							data[11] = (x_pos & 0xffff) >> 8;
							data[12] = x_pos & 0xff;
						}
						//read y_pos
						else if (data[9] == 6 && data[11] == 2) {
							data[9] = y_pos >> 24;
							data[10] = (y_pos & 0xffffff) >> 16;
							data[11] = (y_pos & 0xffff) >> 8;
							data[12] = y_pos & 0xff;
						} else {
							//if incorrect parameter
							buflen_answer = 9;
							data[5] = 3;
							data[7] = data[7] | 128;
							data[8] = 1;
						}
					}

					netconn_write(newconn, data, buflen_answer, NETCONN_COPY);

					netbuf_delete(inbuf);

				} else {
					netbuf_delete(inbuf);
					netconn_close(newconn);
					netconn_delete(newconn);
					break;
				}

			}

		} else {
			osDelay(1);
		}
	}
}
