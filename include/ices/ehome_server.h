#pragma once

#include <unistd.h>

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <memory>
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
  explicit EHomeServer(std::string sms_server_public_ip);
  ~EHomeServer();

  // Operations
  void Start();
  void Stop();
  void StopCamera(const int &index);

  // Getters
  IPCamera GetCamera(const int &index) const;
  cv::Mat GetFrame(const int &index) const;
  const std::vector<IPCamera> &GetCameras() const;
  const std::vector<cv::Mat> &GetFrames() const;

  // Status
  bool IsOnline(const int &index) const;
  bool IsPushingStream(const int &index) const;
  bool IsReceivingFrame(const int &index) const;

 private:
  std::string sms_public_ip_ = SMS_PUBLIC_IP;
  LONG sms_port_ = SMS_LISTEN_PORT;

  std::vector<IPCamera> cameras_;
  std::vector<cv::Mat> frames_;
#if (SMS_RECORD)
  std::vector<FILE *> video_files_;
#endif

  LONG reg_listen_handle_ = -1;
  LONG stream_listen_handle_ = -1;

  std::shared_ptr<std::thread> loop_thread_ = nullptr;
  std::atomic_bool stop_flag_{false};

  void EventLoop();
  void ValidateIndex(const int &index) const;

  static BOOL InitCMS();
  static BOOL InitSMS();
  BOOL StartRegistrationListen();
  BOOL StopRegistrationListen();
  BOOL StartPreviewListen();
  BOOL StopPreviewListen();
  static void StopHandler(int signum);

  static void DecodeCallback(int nPort, char *pBuf, int nSize,
                             FRAME_INFO *pFrameInfo, void *nUser,
                             int nReserved2);
  static BOOL RegistrationCallBack(LONG lUserID, DWORD dwDataType,
                                   void *pOutBuffer, DWORD dwOutLen,
                                   void *pInBuffer, DWORD dwInLen, void *pUser);
  BOOL ProcessInputStreamData(LONG user_id, BYTE byDataType, char *pBuffer,
                              int iDataLen);
  static void PreviewDataCallback(LONG lPreviewHandle,
                                  NET_EHOME_PREVIEW_CB_MSG *pPreviewCBMsg,
                                  void *pUserData);
  static BOOL PreviewNewLinkCallback(LONG lPreviewHandle,
                                     NET_EHOME_NEWLINK_CB_MSG *pNewLinkCBMsg,
                                     void *pUserData);
};
