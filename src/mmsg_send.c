
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#ubckyde <linux/socket.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <net/if.h>
#include "include/tcpip.h"
#define PACKET_COUNT 10

int send_mmsg_ether_packet(const char* eth_name, uint8_t * src_mac, uint8_t * dst_mac, 
    const char* src_ip, const char* dst_ip, uint16_t src_port, uint16_t dst_port)  {
  // 创建原始套接字
    int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    // 构建目标地址
    struct sockaddr_ll dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sll_family = AF_PACKET;
    dest_addr.sll_protocol = htons(ETH_P_IP);
    dest_addr.sll_ifindex = if_nametoindex(eth_name); 
    dest_addr.sll_halen = ETH_ALEN;
    memcpy(dest_addr.sll_addr, dst_mac, ETH_ALEN);  // 使用广播MAC地址

    // 构建数据包s
    size_t  packets_len[PACKET_COUNT];
    uint8_t* packets[PACKET_COUNT];
    for (int i = 0; i < PACKET_COUNT; i++) {
        packets[i] =  build_ether_raw(src_mac, dst_mac,src_ip, dst_ip,src_port, dst_port, &packets_len[i]);
    }
    // 准备 mmsghdr 结构数组
    struct mmsghdr msg_hdrs[PACKET_COUNT];

    for (int i = 0; i < PACKET_COUNT; i++) {
        memset(msg_hdrs[i], 0, sizeof(msg_hdrs));

        msg_hdrs[i].msg_hdr.msg_name = &dest_addr;
        msg_hdrs[i].msg_hdr.msg_namelen = sizeof(dest_addr);

        struct iovec iov[1];
        iov[0].iov_base = packets[i];
        iov[0].iov_len = packets_len[i];
        msg_hdrs[i].msg_hdr.msg_iov = iov;
        msg_hdrs[i].msg_hdr.msg_iovlen = 1;
    }

    // 使用 sendmmsg 发送数据包
    int sent_packets = sendmmsg(sockfd, msg_hdrs, PACKET_COUNT, 0);
    if (sent_packets == -1) {
        perror("sendmmsg");
    } else {
        printf("Successfully sent %d packets\n", sent_packets);
    }

    close(sockfd);
    return 0;

}

int send_mmsg_ip_packet(const char* src_ip, const char* dst_ip, uint16_t src_port, uint16_t dst_port) {
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sockfd == -1) {
        perror("Failed to create socket");
        return -1;
    }
    size_t packet_len = 0;
    unsigned char* packet = build_ip_raw(src_ip, dst_ip, src_port, dst_port, &packet_len);
    struct mmsghdr msg_hdrs[PACKET_COUNT];
    struct iovec iov[PACKET_COUNT];
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(dst_ip);
    dest_addr.sin_port = htons(dst_port);

    for (int i = 0; i < PACKET_COUNT; i++) {
        memset(&msg_hdrs[i], 0, sizeof(struct mmsghdr));
        memset(&iov[i], 0, sizeof(struct iovec));

        iov[i].iov_base = packet;
        iov[i].iov_len = packet_len;
        msg_hdrs[i].msg_hdr.msg_iov = &iov[i];
        msg_hdrs[i].msg_hdr.msg_iovlen = 1;
        msg_hdrs[i].msg_hdr.msg_name = &dest_addr;
        msg_hdrs[i].msg_hdr.msg_namelen = sizeof(dest_addr);
    }

    int sent_packets = 0;
    if ((sent_packets = sendmmsg(sockfd, msg_hdrs, PACKET_COUNT, 0)) == -1) {
        perror("Failed to send packets");
        close(sockfd);
        return 1;
    }

    printf("Successfully sent %d packets.\n", sent_packets);

    close(sockfd);
    return 0;
}
