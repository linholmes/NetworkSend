#ifndef NETWORK_SEND_PACKET_TCPIP_H
#define NETWORK_SEND_PACKET_TCPIP_H
#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


uint8_t *build_ip_raw(const char* src_ip, const char* dst_ip,
    uint16_t src_port, uint16_t dst_port, size_t *packet_len);
	
uint8_t *build_ether_raw(uint8_t* src_mac, uint8_t* dst_mac, 
    const char* src_ip, const char* dst_ip, uint16_t src_port, uint16_t dst_port, size_t *packet_len);

#ifdef __cplusplus
}
#endif

#endif /* NETWORK_SEND_PACKET_TCPIP_H */
