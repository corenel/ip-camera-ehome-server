#pragma once

#include <string>

#include "config.h"

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
  char push_state = IPCS_NOT_PUSHING_STREAM;
  // 是否在线
  char online_state = IPCS_OFFLINE;
};
