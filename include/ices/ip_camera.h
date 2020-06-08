#pragma once

#include <string>

#include "config.h"

enum class IPC_REG_STATUS {
  IPC_OFFLINE = 0,
  IPC_ONLINE = 1,
};

enum class IPC_STREAM_STATUS {
  IPC_NOT_PUSHING_STREAM = 0,
  IPC_PUSHING_STREAM = 1,
};

struct IPCamera {
  // 系统分配的登录ID
  long login_id = -1;
  // 实时流句柄
  long stream_handle = -1;
  // 系统分配的预览会话ID (struPreviewOut.lSessionID)
  long preview_session_id = -1;
  // 设备ID
  std::string device_id;
  // 设备IP
  std::string device_ip;
  // 是否在推流
  IPC_STREAM_STATUS push_state = IPC_STREAM_STATUS::IPC_NOT_PUSHING_STREAM;
  // 是否在线
  IPC_REG_STATUS online_state = IPC_REG_STATUS::IPC_OFFLINE;
};
