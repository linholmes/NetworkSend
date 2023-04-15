#ifndef NETWORK_SEND_PACKET_MMSG_SEND_H
#define NETWORK_SEND_PACKET_MMSG_SEND_H
#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int send_mmsg_ip_packet(const char* src_ip, const char* dst_ip, uint16_t src_port, uint16_t dst_port);

#ifdef __cplusplus
}
#endif

#endif /* NETWORK_SEND_PACKET_MMSG_SEND_H */
