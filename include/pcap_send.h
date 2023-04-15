
#ifndef NETWORK_SEND_PACKET_PCAP_SEND
#define NETWORK_SEND_PACKET_PCAP_SEND
#ifdef __cplusplus
extern "C" {
#endif

int send_pcap_ip_packet(const char* eth_name, const char* src_ip, const char* dst_ip,
    uint16_t src_port, uint16_t dst_port);
    
int send_pcap_ether_packet(const char* eth_name, uint8_t* src_mac, uint8_t* dst_mac, 
    const char* src_ip, const char* dst_ip, uint16_t src_port, uint16_t dst_port);


#ifdef __cplusplus
}
#endif

#endif
