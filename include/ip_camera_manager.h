#pragma once

#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <vector>

#include "config.h"
#if (SDK_TYPE == SDK_ISUP)
#include "HCISUPCMS.h"
#include "HCISUPStream.h"
#else
#include "HCEHomeCMS.h"
#include "HCEHomeStream.h"
#endif
#include "PlayM4.h"
#include "opencv2/opencv.hpp"

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

std::vector<IPCamera> cameras_(IPCS_MAX_NUM);
std::vector<cv::Mat> frames_(IPCS_MAX_NUM);
LONG port_ = SMS_PREVIEW_STREAM_SERVER_PORT;

BOOL CALLBACK RegistrationCallBack(LONG lUserID, DWORD dwDataType,
                                   void *pOutBuffer, DWORD dwOutLen,
                                   void *pInBuffer, DWORD dwInLen, void *pUser);
BOOL ProcessInputStreamData(LONG user_id, BYTE byDataType, char *pBuffer,
                            int iDataLen);
void CALLBACK PreviewDataCallback(LONG lPreviewHandle,
                                  NET_EHOME_PREVIEW_CB_MSG *pPreviewCBMsg,
                                  void *pUserData);
BOOL CALLBACK PreviewNewLinkCallback(LONG lPreviewHandle,
                                     NET_EHOME_NEWLINK_CB_MSG *pNewLinkCBMsg,
                                     void *pUserData);
