#include <stdlib.h>
#include <pfring.h>
#include "include/tcpip.h"

int send_pfring_packet(const char *interface, const unsigned char *packet, int packet_size){
     // 初始化 PF_RING
    pfring *pd = pfring_open(interface, 65535, 0);
    if (pd == NULL) {
        fprintf(stderr, "pfring_open() failed: %s\n", strerror(errno));
        return -1;
    }
   //pfring_set_application_name(pd, "test");
   //pfring_set_direction(pd, rx_and_tx_direction);

    // Enable PF_RING
    if (pfring_enable_ring(pd) != 0) {
        printf("pfring_enable() failed\n");
        pfring_close(pd);
        return -1;
    }

    // 使用 PF_RING 发送数据包
    for (int i =0; i < 10000;++i){
        size_t bytes_sent = pfring_send(pd, (char *)packet, 64, 1 );
        if (bytes_sent < 0) {
             fprintf(stderr, "pfring_send() failed with error code %d \n", bytes_sent);
        } else {
           // printf("Sent %zd bytes\n", bytes_sent);
        }
    }
    // 关闭套接字和 PF_RING
    pfring_close(pd);
    return 0;
}


int send_pfring_ip_packet(const char* eth_name, const char* src_ip, const char* dst_ip,
    uint16_t src_port, uint16_t dst_port){
    // 设置网络接口名称
     size_t packet_len = 0;
    unsigned char* packet = build_ip_raw(src_ip, dst_ip, src_port, dst_port, &packet_len);

    // 发送数据包
    int ret = send_pfring_packet(eth_name, packet, packet_len);
      // 释放内存
    free(packet);
    return ret;
}
	
int send_pfring_ether_packet(const char* eth_name, uint8_t* src_mac, uint8_t* dst_mac, 
    const char* src_ip, const char* dst_ip, uint16_t src_port, uint16_t dst_port){
    size_t packet_len;
    unsigned char* packet = build_ether_raw(src_mac, dst_mac,src_ip,
         dst_ip,src_port, dst_port, &packet_len);

    // 发送数据包
    int ret = send_pfring_packet(eth_name, packet, packet_len);
    // 释放内存
    free(packet);
    return ret;
}