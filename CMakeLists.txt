cmake_minimum_required(VERSION 3.10)
project(PacketSender)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include)

option(USE_PCAP "Use pcap for packet sending" ON)
option(USE_PFRING "Use pfring for packet sending" OFF)

if(0)
    add_definitions(-DUSE_PCAP)
    find_package(PCAP REQUIRED)
    message(${PCAP_LIBRARY} ${PCAP_INCLUDE_DIRS})

    if(NOT PCAP_LIBRARY)
        message(FATAL_ERROR "pcap library not found")
    endif()
    include_directories(${PCAP_INCLUDE_DIRS})
endif()

if(0)
    add_definitions(-DUSE_PFRING)
    find_library(PFRING_LIBRARY pfring)
    message(${PFRING_LIBRARY} ${PFRING_INCLUDE_DIRS})
    if(NOT PFRING_LIBRARY)
        message(FATAL_ERROR "pfring library not found")
    endif()
    include_directories(${PFRING_INCLUDE_DIRS})
endif()

add_executable(PacketSender
        main.cpp
        src/tcpip.c
        src/raw_socket.c
        src/tcp_socket.c
        src/eth.c
        src/mmsg_send.c
        ${EXTRA_SRC})

if(USE_PCAP)
    target_sources(PacketSender PRIVATE src/pcap_send.c)
    target_link_libraries(PacketSender -lpcap -lpthread)
endif()

if(USE_PFRING)
    target_sources(PacketSender PRIVATE src/pfring_send.c)
    target_link_libraries(PacketSender /usr/local/lib/libpfring.so)
    include_directories(/usr/local/include)

endif()
