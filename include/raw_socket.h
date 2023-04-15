
#ifndef NETWORK_SEND_PACKET_RAW_SOCKET
#define NETWORK_SEND_PACKET_RAW_SOCKET
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

    int send_raw_ip_socket(const char *src_ip, const char *dst_ip, uint16_t src_port, uint16_t dst_port);
        
    int send_raw_eth_socket(const char* eth_name, uint8_t * src_mac, uint8_t * dst_mac, 
        const char* src_ip, const char* dst_ip, uint16_t src_port, uint16_t dst_port);
#ifdef __cplusplus
}
#endif
#endif