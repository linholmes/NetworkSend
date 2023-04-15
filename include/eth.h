#ifndef NETWORK_SEND_PACKET_ETH
#define NETWORK_SEND_PACKET_ETH
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int get_mac_address_by_eth(const char* eth_name, uint8_t* mac);

#ifdef __cplusplus
}
#endif

#endif /* NETWORK_SEND_PACKET_PCAP_SEND */
