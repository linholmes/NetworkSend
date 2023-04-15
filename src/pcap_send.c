#include <stdlib.h>
#include <pcap.h>
#include "include/tcpip.h"

int send_pcap_packet(const char *interface, const unsigned char *packet, int packet_size) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;

    // 打开网络接口
    handle = pcap_open_live(interface, 65535, 8, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", interface, errbuf);
        return -1;
    }

    for (int i =0 ; i < 10000; ++i){
        // 发送数据包
        if (pcap_sendpacket(handle, packet, packet_size) != 0) {
            fprintf(stderr, "Error sending packet: %s\n", pcap_geterr(handle));
        }
    }
    // 关闭pcap句柄
    pcap_close(handle);
    return 0;
    
}

int send_pcap_ip_packet(const char* eth_name, const char* src_ip, const char* dst_ip,
    uint16_t src_port, uint16_t dst_port){
    // 设置网络接口名称
    size_t packet_len = 0;
    unsigned char* packet = build_ip_raw(src_ip, dst_ip, src_port, dst_port, &packet_len);

    // 发送数据包
    int ret = send_pcap_packet(eth_name, packet, packet_len);
      // 释放内存
    free(packet);
    return ret;
}
	
int send_pcap_ether_packet(const char* eth_name, uint8_t* src_mac, uint8_t* dst_mac, 
    const char* src_ip, const char* dst_ip, uint16_t src_port, uint16_t dst_port){
    size_t packet_len;
    unsigned char* packet = build_ether_raw(src_mac, dst_mac,src_ip,
         dst_ip,src_port, dst_port, &packet_len);

    // 发送数据包
    int ret = send_pcap_packet(eth_name, packet, packet_len);
    // 释放内存
    free(packet);
    return ret;
}