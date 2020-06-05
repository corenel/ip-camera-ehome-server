#pragma once

// SDK
#define SDK_ISUP 0
#define SDK_EHOME 1
#ifndef USE_EHOME
#define SDK_TYPE SDK_ISUP
#else
#define SDK_TYPE SDK_EHOME
#endif

// SMS
// 通道号
#define SMS_PREVIEW_CHANNEL 1
// 0-TCP, 1-UDP
#define LINK_MODE_TCP 0
#define LINK_MODE_UDP 1
#define SMS_PREVIEW_LINK_MODE LINK_MODE_TCP
// 码流类型：0- 主码流，1- 子码流, 2- 第三码流
#define STREAM_TYPE_MAIN 0
#define STREAM_TYPE_SUB 1
#define STREAM_TYPE_THIRD 2
#define SMS_PREVIEW_STREAM_TYPE STREAM_TYPE_MAIN
// SMS 的 IP 地址
#define SMS_LISTEN_IP "0.0.0.0"
#define SMS_PUBLIC_IP "192.168.199.5"
// SMS 的端口号，需和监听端口号一致
#define SMS_LISTEN_PORT 8000
// 是否录制视频文件
#define SMS_RECORD true

// CMS
#define CMS_LISTEN_IP "0.0.0.0"
#define CMS_LISTEN_PORT 7660

// IPCamera
#define IPCS_MAX_NUM 32
#define IPCS_PUSHING_STREAM 1
#define IPCS_NOT_PUSHING_STREAM 0
#define IPCS_ONLINE 1
#define IPCS_OFFLINE 0
