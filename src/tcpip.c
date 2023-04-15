#include <arpa/inet.h> // inet_aton
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <net/ethernet.h>
#include <stdlib.h> //calloc 
#include <string.h> // memcpy

unsigned short csum(const uint16_t *ptr, int nbytes) {
    register unsigned long sum;
    for (sum = 0; nbytes > 0; nbytes--) {
        sum += *ptr++;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

// Calculate the checksum
unsigned short csum2(const uint16_t *ptr, int nbytes)
{
    register long sum;
    unsigned short oddbyte;
    register short answer;

    sum = 0;
    while (nbytes > 1)
    {
        sum += *ptr++;
        nbytes -= 2;
    }
    if (nbytes == 1)
    {
        oddbyte = 0;
        *((u_char *)&oddbyte) = *(u_char *)ptr;
        sum += oddbyte;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum = sum + (sum >> 16);
    answer = (short)~sum;

    return (answer);
}

// 计算TCP校验和的函数
uint16_t tcp_checksum(const void *buff, size_t len, in_addr_t src_addr, in_addr_t dst_addr) {
    const uint16_t *buf = (const uint16_t*)(buff);
    uint32_t sum;
    size_t length = len;

    // 计算伪报头和
    sum = src_addr & 0xFFFF;
    sum += src_addr >> 16;
    sum += dst_addr & 0xFFFF;
    sum += dst_addr >> 16;
    sum += htons(IPPROTO_TCP);
    sum += htons(len);

    // 计算TCP报头和数据的和
    while (length > 1) {
        sum += *buf++;
        length -= 2;
    }

    if (length > 0) {
        sum += *(const uint8_t*)(buf);
    }

    // 折叠校验和
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return ~sum;
}

/*
@input dst_mac  ex:00:0c:29:4d:da:4d
@input eth_name ex:eno1
@output ether_header
*/
struct ether_header *build_ether_header(uint8_t* src_mac, uint8_t* dst_mac){
    // 构造以太网帧头-原始mac地址，目标mac地址，2个字节的以payload类型 ETH_P_IP表示上层使用IPv4
    // 详见linux/if_ether.h
    struct ether_header *eth_header = (struct ether_header *)calloc(sizeof(struct ether_header), 1);
    memcpy(eth_header->ether_dhost, dst_mac, ETH_ALEN);
    memcpy(eth_header->ether_shost, src_mac, ETH_ALEN);
    eth_header->ether_type = htons(ETH_P_IP);
    return eth_header;
}

struct iphdr * build_ip_header(const char* src_ip, const char* dst_ip){
    struct iphdr* ip_header = (struct iphdr*)calloc(sizeof(struct iphdr), 1);
    ip_header->ihl = 5;
    ip_header->version = 4;
    ip_header->tos = 0;
    ip_header->tot_len = htons(sizeof(struct iphdr) + sizeof(struct tcphdr));
    ip_header->id = 0;
    ip_header->frag_off = 0;
    ip_header->ttl = 64;
    ip_header->protocol= IPPROTO_TCP;
    ip_header->check = 0;
    inet_aton(src_ip, (struct in_addr*)&ip_header->saddr);
    inet_aton(dst_ip, (struct in_addr*)&ip_header->daddr);
    return ip_header;
}

struct tcphdr *build_tcp_header(uint16_t src_port, uint16_t dst_port){
    struct tcphdr* tcp_header = (struct tcphdr*)calloc(sizeof(tcp_header), 1);
    tcp_header->source = htons(src_port);
    tcp_header->dest = htons(dst_port);
    tcp_header->seq = htonl(1);
    tcp_header->ack_seq = 0;
    tcp_header->doff = sizeof(struct tcphdr) / 4;
    tcp_header->syn = 1;
    tcp_header->window = htons(65535);
    tcp_header->check = 0;
    tcp_header->urg_ptr = 0;
    return tcp_header;
}

uint8_t *build_ip_raw(const char* src_ip, const char* dst_ip,
    uint16_t src_port, uint16_t dst_port, size_t *packet_len){
    *packet_len = sizeof(struct iphdr) + sizeof(struct tcphdr);
    // IP header
    uint8_t *packet = (uint8_t*)calloc(*packet_len, 1);
    // 构造IP报头
    struct iphdr * ip_header = build_ip_header(src_ip, dst_ip);
    ip_header->check = csum((unsigned short *)ip_header, sizeof(struct iphdr));
    
    // 构造TCP报头
    struct tcphdr *tcp_header = build_tcp_header(src_port, dst_port);

    // 计算并设置TCP校验和
    tcp_header->check = tcp_checksum(tcp_header, sizeof(struct tcphdr), ip_header->saddr, ip_header->daddr);

    memcpy(packet, ip_header, sizeof(struct iphdr));
    memcpy(packet +  sizeof(struct iphdr), tcp_header, sizeof(struct tcphdr));
    // IP checksum

    free(ip_header);
    free(tcp_header);

    return packet;
}

uint8_t *build_ether_raw(uint8_t* src_mac, uint8_t* dst_mac, 
    const char* src_ip, const char* dst_ip, uint16_t src_port, uint16_t dst_port, size_t *packet_len){
 
    *packet_len = sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct tcphdr);
    // IP header
    uint8_t *packet = (uint8_t*)calloc(*packet_len, 1);
    // 构造以太网帧头
    struct ether_header * eth_header  = build_ether_header(src_mac, dst_mac);

    // 构造IP报头
    struct iphdr * ip_header = build_ip_header(src_ip, dst_ip);;
    ip_header->check = csum((unsigned short *)ip_header, sizeof(struct iphdr));
 
    // 构造TCP报头
    struct tcphdr *tcp_header = build_tcp_header(src_port, dst_port);
    tcp_header->check = tcp_checksum(tcp_header, sizeof(struct tcphdr), ip_header->saddr, ip_header->daddr);

    // 构造数据包
    memcpy(packet, eth_header, sizeof(struct ether_header));
    memcpy(packet + sizeof(struct ether_header), ip_header, sizeof(struct iphdr));
    memcpy(packet + sizeof(struct ether_header) + sizeof(struct iphdr), tcp_header, sizeof(struct tcphdr));

    free(eth_header);
    free(ip_header);
    free(tcp_header);
    return packet;
}


