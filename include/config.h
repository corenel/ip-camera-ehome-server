#pragma once

// Preview
// 通道号
#define PREVIEW_CHANNEL 1
// 0-TCP, 1-UDP
#define LINK_MODE_TCP 0
#define LINK_MODE_UDP 1
#define PREVIEW_LINK_MODE LINK_MODE_TCP
// 码流类型：0- 主码流，1- 子码流, 2- 第三码流
#define STREAM_TYPE_MAIN 0
#define STREAM_TYPE_SUB 1
#define STREAM_TYPE_THIRD 2
#define PREVIEW_STREAM_TYPE STREAM_TYPE_MAIN
// SMS 的 IP 地址
#define PREVIEW_STREAM_SERVER_IP "192.168.199.5"
// SMS 的端口号，需和监听端口号一致
#define PREVIEW_STREAM_SERVER_PORT 8003

// Register
#define CMS_LISTEN_IP "0.0.0.0"
#define CMS_LISTEN_PORT 7660

