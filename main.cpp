#include "raw_socket.h"
#include "tcp_socket.h"
#include "pfring_send.h"
#include "eth.h"
#include "pcap_send.h"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdint.h> // uint_8/uint_16
#include <linux/if_packet.h>
#include <chrono>
#include <fcntl.h> // 包含 F_SETFL 宏定义

#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <tuple>
#include <functional>
#define SRC_IP  ""
#define DST_IP  ""
#define DST_MAC ""
#define SRC_PORT    12345
#define DST_PORT    22
#define ETH_NAME "eno1"


// 在C++11中定义index_sequence和index_sequence_for
namespace std
{
     
template<size_t... I>
struct index_sequence {};

template<size_t N, size_t... I>
struct make_index_sequence : make_index_sequence<N - 1, N - 1, I...> {};

template<size_t... I>
struct make_index_sequence<0, I...> : index_sequence<I...> {};

template<typename... Args>
using index_sequence_for = make_index_sequence<sizeof...(Args)>;

}
// 递归调用函数
template<typename Func, typename Tuple, size_t... I>
void call_func_with_tuple_impl(Func&& func, Tuple&& tuple, std::index_sequence<I...>) {
    func(std::get<I>(tuple)...);
}

// 参数展开调用函数
template<typename Func, typename... Args>
void call_func_with_tuple(Func&& func, const std::tuple<Args...>& tuple) {
    call_func_with_tuple_impl(std::forward<Func>(func), tuple, std::index_sequence_for<Args...>{});
}

template<typename Func, typename... Args>
void check_bench_mark(int count, int thread_num, const char* name, Func&& func, Args&&... args) {
    double cpu_time_used;
    auto ch_start = std::chrono::high_resolution_clock::now(); // 记录开始时间

    // 将参数打包为一个tuple
    std::tuple<Args...> arg_pack(std::forward<Args>(args)...);

    for (int i = 0; i < count; ++i) {
        std::vector<std::thread> threads;

        // Create and store the threads
        for (int i = 0; i < thread_num; ++i) {
            threads.emplace_back([&func, &arg_pack] {
                // 使用辅助函数递归展开参数并调用函数
                call_func_with_tuple(func, arg_pack);
            });
        }

        // Join the threads
        for (auto& t : threads) {
            t.join();
        }
    }

    auto stop = std::chrono::high_resolution_clock::now(); // 记录结束时间
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - ch_start); // 计算时间差
    std::cout << "Time taken by function: " << name << " thread_num " << thread_num << " cost time is " << duration.count() / 1000.0 << " milliseconds" << std::endl;
    return;
}



int main(){
    // 获取网卡
    uint8_t src_mac_sz[6] = {0};
    uint8_t dst_mac_sz[6] = {0};
    if (get_mac_address_by_eth(ETH_NAME, src_mac_sz) != 0){
        perror("Failed to get_mac_address_by_eth.");
        return -1;
    }
    //%02hhx 是一个转换说明符，用于读取或打印一个宽度为 2 个字符的无符号 16 进制整数（即，一个字节）
    //，如果字符数量不足 2 个，使用前导零进行填充。
    sscanf(DST_MAC, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx"
        , &dst_mac_sz[0], &dst_mac_sz[1], &dst_mac_sz[2], &dst_mac_sz[3], &dst_mac_sz[4], &dst_mac_sz[5]);

    int count = 20;
    for (int thread_num = 0; thread_num <= 20; thread_num += 5){
        if(thread_num == 0){
            thread_num = 1;
        }
        count = count/thread_num;
        check_bench_mark(count, thread_num, "send_raw_eth_socket", &send_raw_eth_socket ,ETH_NAME, src_mac_sz, dst_mac_sz, SRC_IP, DST_IP, SRC_PORT, DST_PORT);
        check_bench_mark(count, thread_num, "send_raw_ip_socket", &send_raw_ip_socket , SRC_IP, DST_IP, SRC_PORT, DST_PORT);
#ifdef USE_PCAP
        check_bench_mark(count, thread_num, "send_pcap_ether_packet", &send_pcap_ether_packet ,ETH_NAME, src_mac_sz, dst_mac_sz, SRC_IP, DST_IP, SRC_PORT, DST_PORT);
        check_bench_mark(count, thread_num, "send_pcap_ip_packet", &send_pcap_ip_packet ,ETH_NAME , SRC_IP, DST_IP, SRC_PORT, DST_PORT);
#endif

#ifdef USE_PFRING
        check_bench_mark(count, thread_num, "send_pfring_ether_packet", &send_pfring_ether_packet ,ETH_NAME, src_mac_sz, dst_mac_sz, SRC_IP, DST_IP, SRC_PORT, DST_PORT);
        check_bench_mark(count, thread_num, "send_pfring_ip_packet", &send_pfring_ip_packet ,ETH_NAME , SRC_IP, DST_IP, SRC_PORT, DST_PORT);
#endif
        if(thread_num == 1){
            thread_num = 0;
        }
        sleep(5);
    }
   // check_bench_mark(10, "send_tcp_socket_linger", &send_tcp_socket_linger,  DST_IP,  DST_PORT);
   // check_bench_mark(20000, "send_tcp_socket", &send_tcp_socket , DST_IP,  DST_PORT);
}