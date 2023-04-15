#include "include/raw_socket.h"
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>

#include <net/ethernet.h>
#include <linux/if_packet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "include/tcpip.h"

int send_raw_eth_socket(const char* eth_name, uint8_t * src_mac, uint8_t * dst_mac, 
    const char* src_ip, const char* dst_ip, uint16_t src_port, uint16_t dst_port) {
    // 创建原始套接字
    int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0) {
        perror("Error creating raw socket");
        return -1;
    }

   // 获取接口索引
   /*
    struct ifreq ifr;
    strncpy(ifr.ifr_name, eth_name, IFNAMSIZ - 1);
    if (ioctl(sockfd, SIOCGIFINDEX, &ifr) < 0) {
        perror("Error getting interface index");
        return -1;
    }

    // 绑定网卡
    if (ioctl(sockfd, SIOCGIFINDEX, &ifr) < 0){
        perror("binding interface error ");
        return -1;
    }
*/
    size_t packet_len;
    unsigned char* packet = build_ether_raw(src_mac, dst_mac, src_ip,
         dst_ip,src_port, dst_port, &packet_len);


    struct sockaddr_ll dst_addr;
    memset(&dst_addr, 0, sizeof(dst_addr));
    dst_addr.sll_family = AF_PACKET;
    dst_addr.sll_ifindex = if_nametoindex(eth_name);
    dst_addr.sll_halen = ETH_ALEN;
    memcpy(dst_addr.sll_addr, dst_mac, ETH_ALEN);

    for (int i =0 ; i < 10000; ++i){
        // 发送数据包
        if (sendto(sockfd, packet, packet_len, 0, (struct sockaddr *)&dst_addr, sizeof(dst_addr)) < 0) {
            perror("Error sending packet");
            return -1;
        }
    }

    free(packet);
    // 关闭套接字
    close(sockfd);

    return 0;
}

int send_raw_ip_socket(const char *src_ip, const char *dst_ip, uint16_t src_port, uint16_t dst_port){
  // Create raw socket
    int s = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
    if (s == -1)
    {
        perror("Failed to create raw socket");
        return -1;
    }

    // IP header
    size_t packet_len = 0;
    unsigned char* packet = build_ip_raw(src_ip, dst_ip, src_port, dst_port, &packet_len);

    struct sockaddr_in dst_addr;
    dst_addr.sin_family = AF_INET;
    dst_addr.sin_port = htons(dst_port);
    dst_addr.sin_addr.s_addr = inet_addr(dst_ip);

    // Tell the kernel that we provide IP header
    int one = 1;
    if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0)
    {
        perror("Error setting IP_HDRINCL");
        return -1;
    }
    int sndbuf_size = 65535 ; // 1 MB
    if (setsockopt(s, SOL_SOCKET, SO_SNDBUF, &sndbuf_size, sizeof(sndbuf_size)) == -1) {
        return -1;
    }
    struct linger ling = {1, 0};
    if (setsockopt(s, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling)) == -1) {
        perror("setsockopt"); // 输出错误信息
        return -1;
    }

    for (int i = 0; i < 10000; ++i){
        // Send the packet
        if (sendto(s, packet, packet_len, 0, (struct sockaddr *)&dst_addr, sizeof(dst_addr)) < 0)
        {
            perror("Error sending SYN packet");
            return -1;
        }
    }
    free(packet);
   // close(s);
    return 0;
}
