#include "include/tcp_socket.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <stdio.h>
#include <fcntl.h> // 包含 F_SETFL 宏定义
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int send_tcp_socket_linger(const char * dst_ip, uint16_t dst_port){

    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    printf("socket s =%d\n",s);
    if (s == -1)
    {
        perror("Failed to create tcp socket");
        return -1;
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr)); // 清空结构体
    server_addr.sin_family = AF_INET; // 地址族为 IPv4
    server_addr.sin_port = htons(dst_port); // 端口号，需要使用网络字节序
    inet_pton(AF_INET, dst_ip, &server_addr.sin_addr); // 将 IP 地址转换为网络字节序
    // 设置 SO_LINGER 选项以发送 RST
    struct linger ling = {1, 0};
    fcntl(s, F_SETFL, O_NONBLOCK);
    if (setsockopt(s, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling)) == -1) {
        perror("setsockopt"); // 输出错误信息
        close(s);
        return -1;
    }
    if (connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect error!");
    }

    close(s);
}

int send_tcp_socket(const char * dst_ip, uint16_t dst_port){

    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == -1)
    {
        perror("Failed to create tcp socket");
        exit(1);
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr)); // 清空结构体
    server_addr.sin_family = AF_INET; // 地址族为 IPv4
    server_addr.sin_port = htons(dst_port); // 端口号，需要使用网络字节序
    inet_pton(AF_INET, dst_ip, &server_addr.sin_addr); // 将 IP 地址转换为网络字节序
    // 设置 SO_LINGER 选项以发送 RST
    struct linger ling = {1, 0};
    if (setsockopt(s, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling)) == -1) {
        perror("setsockopt"); // 输出错误信息
        close(s);
        return -1;
    }
    if (connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect error!");
    }

    close(s);
}