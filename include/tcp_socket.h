#ifndef NETWORK_SEND_PACKET_TCP_SOCKET
#define NETWORK_SEND_PACKET_TCP_SOCKET
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int send_tcp_socket_linger(const char * dst_ip, uint16_t dst_port);
int send_tcp_socket(const char * dst_ip, uint16_t dst_port);
#ifdef __cplusplus
}
#endif
#endif // !NETWORK_SEND_PACKET_TCP_SOCKET
