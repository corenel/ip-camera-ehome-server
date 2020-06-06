#pragma once

#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <thread>
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
#include "ip_camera.h"
#include "opencv2/opencv.hpp"

class EHomeServer {
 public:
  EHomeServer();
  ~EHomeServer();
  BOOL Start();
  BOOL Stop();

 private:
  std::vector<IPCamera> cameras_;
  std::vector<cv::Mat> frames_;
  LONG port_ = SMS_LISTEN_PORT;
#if (SMS_RECORD)
  FILE *video_file_ = nullptr;
#endif

  LONG lListen = -1;
  LONG lHandle = -1;

  std::thread *thread_ = nullptr;
  BOOL stop_flag = FALSE;

  static BOOL InitCMS();
  static BOOL InitSMS();
  BOOL StartRegistrationListen();
  BOOL StopRegistrationListen();
  BOOL StartPreviewListen();
  BOOL StopPreviewListen();
  static void StopHandler(int signum);

  static void CALLBACK DecodeCallback(int nPort, char *pBuf, int nSize,
                                      FRAME_INFO *pFrameInfo, void *nUser,
                                      int nReserved2);
  static BOOL CALLBACK RegistrationCallBack(LONG lUserID, DWORD dwDataType,
                                            void *pOutBuffer, DWORD dwOutLen,
                                            void *pInBuffer, DWORD dwInLen,
                                            void *pUser);
  BOOL ProcessInputStreamData(LONG user_id, BYTE byDataType, char *pBuffer,
                              int iDataLen);
  static void CALLBACK
  PreviewDataCallback(LONG lPreviewHandle,
                      NET_EHOME_PREVIEW_CB_MSG *pPreviewCBMsg, void *pUserData);
  static BOOL CALLBACK PreviewNewLinkCallback(
      LONG lPreviewHandle, NET_EHOME_NEWLINK_CB_MSG *pNewLinkCBMsg,
      void *pUserData);
};
