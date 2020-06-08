#include "ices/ehome_server.h"

#include <csignal>
#include <thread>

EHomeServer *server_ = nullptr;

EHomeServer::EHomeServer() {
  if (server_ == nullptr) {
    server_ = this;
  }

  cameras_.resize(IPCS_MAX_NUM);
  frames_.resize(IPCS_MAX_NUM);
#if (SMS_RECORD)
  video_files_.resize(IPCS_MAX_NUM);
#endif

  InitCMS();
  InitSMS();

  signal(SIGINT, EHomeServer::StopHandler);
}

EHomeServer::EHomeServer(std::string sms_public_ip) : EHomeServer() {
  sms_public_ip_ = std::move(sms_public_ip);
}

EHomeServer::~EHomeServer() { Stop(); }

void EHomeServer::Start() {
  StartRegistrationListen();
  StartPreviewListen();
  stop_flag_ = false;
  loop_thread_ = std::make_shared<std::thread>(&EHomeServer::EventLoop, this);
}

void EHomeServer::Stop() {
  stop_flag_ = true;
  if (loop_thread_ && loop_thread_->joinable()) {
    loop_thread_->join();
  }
  StopRegistrationListen();
  StopPreviewListen();
}

void EHomeServer::StopCamera(const int &index) {
  // 停止录制
#if (SMS_RECORD)
  if (video_files_[index] != nullptr) {
    fclose(video_files_[index]);
    video_files_[index] = nullptr;
  }
#endif

  // 释放被 CMS 预览请求占用的资源
#if (SDK_TYPE == SDK_ISUP)
  if (cameras_[index].preview_session_id >= 0) {
    NET_EHOME_STOPSTREAM_PARAM struStopParam = {0};
    struStopParam.lSessionID = cameras_[index].preview_session_id;
    if (!NET_ECMS_StopGetRealStreamEx(cameras_[index].login_id,
                                      &struStopParam)) {
      printf("NET_ECMS_StopGetRealStreamEx failed, error code: %d\n",
             NET_ESTREAM_GetLastError());
    }
  }
#else
  if (cameras_[index].preview_session_id >= 0) {
    if (!NET_ECMS_StopGetRealStream(cameras_[index].login_id,
                                    cameras_[index].preview_session_id)) {
      printf("NET_ECMS_StopGetRealStream failed, error code: %d\n",
             NET_ECMS_GetLastError());
    }
  }
#endif

  // SMS 停止转发码流
  if (cameras_[index].stream_handle >= 0) {
    if (!NET_ESTREAM_StopPreview(cameras_[index].stream_handle)) {
      printf("NET_ESTREAM_StopPreview failed, error code: %d\n",
             NET_ESTREAM_GetLastError());
    }
    if (!NET_ESTREAM_StopListenPreview(cameras_[index].stream_handle)) {
      printf("NET_ESTREAM_StopListenPreview failed, error code: %d\n",
             NET_ESTREAM_GetLastError());
    }
  }
}

IPCamera EHomeServer::GetCamera(const int &index) const {
  assert(index < IPCS_MAX_NUM);
  return cameras_[index];
}

const std::vector<IPCamera> &EHomeServer::GetCameras() const {
  return cameras_;
}

cv::Mat EHomeServer::GetFrame(const int &index) const {
  assert(index < IPCS_MAX_NUM);
  return frames_[index];
}

const std::vector<cv::Mat> &EHomeServer::GetFrames() const { return frames_; }

bool EHomeServer::IsOnline(const int &index) const {
  ValidateIndex(index);
  return cameras_[index].online_state == IPC_REG_STATUS::IPC_ONLINE;
}

bool EHomeServer::IsPushingStream(const int &index) const {
  ValidateIndex(index);
  return cameras_[index].push_state == IPC_STREAM_STATUS::IPC_PUSHING_STREAM;
}

bool EHomeServer::IsReceivingFrame(const int &index) const {
  ValidateIndex(index);
  return !frames_[index].empty();
}

void EHomeServer::EventLoop() {
  while (!stop_flag_) {
    // The following operations should be done when the
    // registration is completed
    usleep(100000);
    //      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    for (auto i = 0; i < IPCS_MAX_NUM; ++i) {
      if (cameras_[i].online_state == IPC_REG_STATUS::IPC_ONLINE &&
          cameras_[i].push_state != IPC_STREAM_STATUS::IPC_PUSHING_STREAM) {
        NET_EHOME_XML_CFG struXmlCfg = {0};
        char sCmd[32] = "GETDEVICECONFIG";
        char sInBuf[512] =
            "<Params>\r\n<ConfigCmd>GetDevAbility</ConfigCmd>\r\n<ConfigParam1>1 </ConfigParam1>\r\n\
                                                      <ConfigParam2></ConfigParam2>\r\n<ConfigParam3></ConfigParam3>\r\n<ConfigParam4></ConfigParam4>\r\n</Params>";
        char sOutBuf[1024] = {0};
        char sStatusBuf[1024] = {0};
        struXmlCfg.dwCmdLen = strlen(sCmd);
        struXmlCfg.pCmdBuf = sCmd;
        struXmlCfg.pInBuf = sInBuf;
        struXmlCfg.pOutBuf = sOutBuf;
        struXmlCfg.pStatusBuf = sStatusBuf;
        struXmlCfg.dwInSize = 512;
        struXmlCfg.dwOutSize = 1024;
        struXmlCfg.dwStatusSize = 1024;
        if (!NET_ECMS_XMLConfig(cameras_[i].login_id, &struXmlCfg,
                                sizeof(NET_EHOME_XML_CFG))) {
          printf("GetDevAbility failed, error code: %d\n",
                 NET_ECMS_GetLastError());
        } else {
          printf("GetDevAbility succeed!\n");
          //            printf("%s\n", sOutBuf);
        }

        //预览请求
        NET_EHOME_PREVIEWINFO_IN struPreviewIn = {0};
        NET_EHOME_PREVIEWINFO_OUT struPreviewOut = {0};
        struPreviewIn.iChannel = SMS_PREVIEW_CHANNEL;
        struPreviewIn.dwLinkMode = SMS_PREVIEW_LINK_MODE;
        struPreviewIn.dwStreamType = SMS_PREVIEW_STREAM_TYPE;
        memcpy(struPreviewIn.struStreamSever.szIP, sms_public_ip_.c_str(),
               sizeof(NET_EHOME_IPADDRESS));
        struPreviewIn.struStreamSever.wPort = sms_port_;
        printf("Preview settings:\n");
        printf("\tChannel: %d\n", struPreviewIn.iChannel);
        printf("\tLink mode: %d\n", struPreviewIn.dwLinkMode);
        printf("\tStream type: %d\n", struPreviewIn.dwStreamType);
        printf("\tServer IP: %s\n", struPreviewIn.struStreamSever.szIP);
        printf("\tServer port: %d\n", struPreviewIn.struStreamSever.wPort);
        if (!NET_ECMS_StartGetRealStream(cameras_[i].login_id, &struPreviewIn,
                                         &struPreviewOut)) {
          printf("NET_ECMS_StartGetRealStream failed, error code: %d\n",
                 NET_ECMS_GetLastError());
          return;
        }
        printf("NET_ECMS_StartGetRealStream succeed with ID %d!\n",
               struPreviewOut.lSessionID);

        // 发送请求给设备并开始传输实时码流，该操作只对支持 4.0 版本及以上
        // ISUP 的设备有效
        sleep(1);
        NET_EHOME_PUSHSTREAM_IN struPushStreamIn = {0};
        struPushStreamIn.dwSize = sizeof(struPushStreamIn);
        struPushStreamIn.lSessionID = struPreviewOut.lSessionID;
        NET_EHOME_PUSHSTREAM_OUT struPushStreamOut = {0};
        struPushStreamOut.dwSize = sizeof(struPushStreamOut);
        if (!NET_ECMS_StartPushRealStream(
                cameras_[i].login_id, &struPushStreamIn, &struPushStreamOut)) {
          printf("NET_ECMS_StartPushRealStream failed, error code: %d\n",
                 NET_ECMS_GetLastError());
          NET_ECMS_Fini();
          return;
        }
        printf("NET_ECMS_StartPushRealStream succeed!\n");
        cameras_[i].preview_session_id = struPreviewOut.lSessionID;
        cameras_[i].push_state = IPC_STREAM_STATUS::IPC_PUSHING_STREAM;
      }
    }
  }
}

void EHomeServer::ValidateIndex(const int &index) const {
  assert(index < IPCS_MAX_NUM);
}

BOOL EHomeServer::InitCMS() {
  //初始化 CMS 库
  NET_ECMS_Init();

#if (SDK_TYPE == SDK_ISUP)
  //使用负载均衡方式必须设置
  BOOL bSessionKeyReqMod = TRUE;
  NET_ECMS_SetSDKLocalCfg(SESSIONKEY_REQ_MOD, &bSessionKeyReqMod);

  BYTE m_byCmsSecureAccessType = 0;
  NET_EHOME_LOCAL_ACCESS_SECURITY struAccessSecure = {0};
  struAccessSecure.dwSize = sizeof(struAccessSecure);
  struAccessSecure.byAccessSecurity = (BYTE)m_byCmsSecureAccessType;
  if (!NET_ECMS_SetSDKLocalCfg(ACTIVE_ACCESS_SECURITY, &struAccessSecure)) {
    printf(
        "NET_ECMS_SetSDKLocalCfg ACTIVE_ACCESS_SECURITY failed, error code: "
        "%d\n",
        NET_ECMS_GetLastError());
  } else {
    printf("NET_ECMS_SetSDKLocalCfg ACTIVE_ACCESS_SECURITY succeed!\n");
  }

  NET_EHOME_SET_REREGISTER_MODE struSetReRegisterMode = {0};
  struSetReRegisterMode.dwSize = sizeof(struSetReRegisterMode);
  struSetReRegisterMode.dwReRegisterMode = 0;

  if (!NET_ECMS_SetSDKLocalCfg(SET_REREGISTER_MODE, &struSetReRegisterMode)) {
    printf("Set Reregister failed, error code: %d\n", NET_ECMS_GetLastError());
  } else {
    printf("Set Reregister succeed!\n");
  }
#endif

  NET_EHOME_SEND_PARAM struSendParam = {0};
  struSendParam.dwSize = sizeof(struSendParam);
  struSendParam.bySendTimes = 3;
  // CMS开始接收
  if (!NET_ECMS_SetSDKLocalCfg(SEND_PARAM, &struSendParam)) {
    printf("NET_ECMS_SetSDKLocalCfg SEND_PARAM failed, error code: %d\n",
           NET_ECMS_GetLastError());
  } else {
    printf("NET_ECMS_SetSDKLocalCfg SEND_PARAM succeed!\n");
  }

#if (SDK_TYPE == SDK_ISUP)
  NET_EHOME_REGISTER_LISTEN_MODE struParam = {0};
  struParam.dwSize = sizeof(struParam);
  struParam.dwRegisterListenMode = REGISTER_LISTEN_MODE_ALL;
  // CMS开始接收
  if (!NET_ECMS_SetSDKLocalCfg(REGISTER_LISTEN_MODE, &struParam)) {
    printf(
        "NET_ECMS_SetSDKLocalCfg register listen mode failed, error code: %d\n",
        NET_ECMS_GetLastError());
  } else {
    printf("NET_ECMS_SetSDKLocalCfg register listen mode succeed!\n");
  }
#endif

  DWORD dwVersion = NET_ECMS_GetBuildVersion();
  DWORD dwV1 = (dwVersion >> 24);
  DWORD dwV2 = ((dwVersion & 0x00FF0000) >> 16);
  DWORD dwV3 = ((dwVersion & 0x0000FF00) >> 8);
  DWORD dwV4 = (dwVersion & 0x000000FF);
  printf("NET_ECMS_GetBuildVersion, %d.%d.%d.%d\n", dwV1, dwV2, dwV3, dwV4);
  return TRUE;
}

BOOL EHomeServer::InitSMS() {
  // 初始化 SMS 库
  NET_ESTREAM_Init();
//  NET_ESTREAM_SetLogToFile((DWORD)3, (char *)"/tmp/", FALSE);
#if (SDK_TYPE == SDK_ISUP)
  NET_EHOME_LOCAL_ACCESS_SECURITY struAccessSecure = {0};
  BYTE m_byStreamSecureAccessType = 0;
  struAccessSecure.byAccessSecurity = (BYTE)m_byStreamSecureAccessType;
  if (!NET_ESTREAM_SetSDKLocalCfg(ACTIVE_ACCESS_SECURITY, &struAccessSecure)) {
    printf(
        "NET_ESTREAM_SetSDKLocalCfg ACTIVE_ACCESS_SECURITY failed, error code: "
        "%d\n",
        NET_ESTREAM_GetLastError());
  } else {
    printf("NET_ESTREAM_SetSDKLocalCfg ACTIVE_ACCESS_SECURITY succeed!\n");
  }
#endif

  auto dwVersion = NET_ESTREAM_GetBuildVersion();
  auto dwV1 = (dwVersion >> 24);
  auto dwV2 = ((dwVersion & 0x00FF0000) >> 16);
  auto dwV3 = ((dwVersion & 0x0000FF00) >> 8);
  auto dwV4 = (dwVersion & 0x000000FF);
  printf("NET_ESTREAM_GetBuildVersion, %d.%d.%d.%d\n", dwV1, dwV2, dwV3, dwV4);
  return TRUE;
}

BOOL EHomeServer::StartRegistrationListen() {
  //注册的监听参数
  NET_EHOME_CMS_LISTEN_PARAM struCMSListenPara = {0};
  memcpy(struCMSListenPara.struAddress.szIP, CMS_LISTEN_IP,
         sizeof(NET_EHOME_IPADDRESS));
  struCMSListenPara.struAddress.wPort = CMS_LISTEN_PORT;
  struCMSListenPara.fnCB = RegistrationCallBack;
#if (SDK_TYPE == SDK_ISUP)
  struCMSListenPara.dwKeepAliveSec = 5;
  struCMSListenPara.dwTimeOutCount = 6;
#endif

  //开启监听并接收设备注册信息
  reg_listen_handle_ = NET_ECMS_StartListen(&struCMSListenPara);
  if (reg_listen_handle_ <= -1) {
    printf("NET_ECMS_StartListen failed, error code: %d\n",
           NET_ECMS_GetLastError());
    NET_ECMS_Fini();
    return FALSE;
  }
  printf("NET_ECMS_StartListen succeed!\n");
  printf("\tIP: %s\n", CMS_LISTEN_IP);
  printf("\tPort: %d\n", CMS_LISTEN_PORT);
  return TRUE;
}

BOOL EHomeServer::StopRegistrationListen() {
  // CMS 停止监听
  if (reg_listen_handle_ >= 0) {
    if (!NET_ECMS_StopListen(reg_listen_handle_)) {
      printf("NET_ECMS_StopListen failed, error code: %d\n",
             NET_ECMS_GetLastError());
      return FALSE;
    }
  }

  //释放被 CMS 占用的资源
  NET_ECMS_Fini();
  return TRUE;
}

BOOL EHomeServer::StartPreviewListen() {
  // 预览的监听参数
  NET_EHOME_LISTEN_PREVIEW_CFG struListen = {0};
  memcpy(struListen.struIPAdress.szIP, SMS_LISTEN_IP,
         sizeof(NET_EHOME_IPADDRESS));
  // SMS 的监听端口号
  struListen.struIPAdress.wPort = sms_port_;
  struListen.fnNewLinkCB = PreviewNewLinkCallback;
  // 预览请求回调函数
  struListen.pUser = nullptr;
  struListen.byLinkMode = SMS_PREVIEW_LINK_MODE;  // 0-TCP, 1-UDP

  // 开启监听
  stream_listen_handle_ = NET_ESTREAM_StartListenPreview(&struListen);
  if (stream_listen_handle_ <= -1) {
    printf("NET_ESTREAM_StartListenPreview failed, error code: %d\n",
           NET_ESTREAM_GetLastError());
    NET_ESTREAM_Fini();
    return FALSE;
  }
  printf("NET_ESTREAM_StartListenPreview succeed!\n");
  printf("\tIP: %s\n", SMS_LISTEN_IP);
  printf("\tPUBLIC IP: %s\n", sms_public_ip_.c_str());
  printf("\tPort: %d\n", sms_port_);
  return TRUE;
}

BOOL EHomeServer::StopPreviewListen() {
  for (auto i = 0; i < IPCS_MAX_NUM; ++i) {
    StopCamera(i);
  }
  return TRUE;
}

void EHomeServer::StopHandler(int signum) {
  if (server_ != nullptr) {
    server_->Stop();
  }
}

void EHomeServer::DecodeCallback(int nPort, char *pBuf, int nSize,
                                 FRAME_INFO *pFrameInfo, void *nUser,
                                 int nReserved2) {
  //  std::cout << "DecodeCallback, pixfmt: " << pFrameInfo->nType << std::endl;
  if (pFrameInfo->nType == T_YV12) {
    //    std::cout << "the frame infomation is T_YV12" << std::endl;
    auto frame = (cv::Mat *)nUser;
    if (frame->empty()) {
      printf("Width: %d, Height: %d, Framerate: %d\n", pFrameInfo->nWidth,
             pFrameInfo->nHeight, pFrameInfo->dwFrameNum);
      frame->create(pFrameInfo->nHeight, pFrameInfo->nWidth, CV_8UC3);
    }
    cv::Mat YUVImage(pFrameInfo->nHeight + pFrameInfo->nHeight / 2,
                     pFrameInfo->nWidth, CV_8UC1, (unsigned char *)pBuf);
    cv::cvtColor(YUVImage, *frame, cv::COLOR_YUV2BGR_YV12);
    //    std::cout << YUVImage.cols << " " << YUVImage.rows << std::endl;
    //    cv::imshow("RGBImage", *frame);
    //    cv::waitKey(15);
    YUVImage.~Mat();
  }
}

//注册回调函数
BOOL EHomeServer::RegistrationCallBack(LONG lUserID, DWORD dwDataType,
                                       void *pOutBuffer, DWORD dwOutLen,
                                       void *pInBuffer, DWORD dwInLen,
                                       void *pUser) {
  if (ENUM_DEV_ON == dwDataType) {
    auto *pDevInfo = (NET_EHOME_DEV_REG_INFO *)pOutBuffer;
    if (pDevInfo != nullptr) {
      server_->cameras_[lUserID].login_id = lUserID;
      std::string device_id(
          reinterpret_cast<char const *>(pDevInfo->byDeviceID));
      server_->cameras_[lUserID].device_id = device_id;
      server_->cameras_[lUserID].device_ip = pDevInfo->struDevAdd.szIP;
      server_->cameras_[lUserID].online_state = IPC_REG_STATUS::IPC_ONLINE;
      printf("On-line, lUserID: %d, Device ID: %s\n", lUserID,
             pDevInfo->byDeviceID);
      printf("\tIP: %s\n", pDevInfo->struDevAdd.szIP);
      printf("\tSN: %s\n", pDevInfo->sDeviceSerial);
#if (SDK_TYPE == SDK_ISUP)
      printf("\tProtocol Version: %s\n", pDevInfo->byDevProtocolVersion);
#endif
    }

    //输入参数
    auto *pServerInfo = (NET_EHOME_SERVER_INFO *)pInBuffer;
    pServerInfo->dwTimeOutCount = 6;  //心跳超时次数
    pServerInfo->dwKeepAliveSec = 5;  //心跳间隔

    //获取设备信息
    NET_EHOME_DEVICE_INFO struDevInfo = {0};
    struDevInfo.dwSize = sizeof(NET_EHOME_DEVICE_INFO);
    NET_EHOME_CONFIG struCfg = {0};
    struCfg.pOutBuf = &struDevInfo;
    struCfg.dwOutSize = sizeof(NET_EHOME_DEVICE_INFO);
    if (!NET_ECMS_GetDevConfig(lUserID, NET_EHOME_GET_DEVICE_INFO, &struCfg,
                               sizeof(NET_EHOME_CONFIG))) {
      printf("NET_ECMS_GetDevConfig failed, error code: %d\n",
             NET_ECMS_GetLastError());
    } else {
      printf("\tSN: %s", struDevInfo.sSerialNumber);
      printf("\tStartChannel : %d", struDevInfo.dwStartChannel);
    }

  } else if (ENUM_DEV_OFF == dwDataType) {
    printf("Off-line, lUserID: %d\n", lUserID);
    NET_ECMS_ForceLogout(lUserID);
#if (SDK_TYPE == SDK_ISUP)
  } else if (ENUM_DEV_AUTH == dwDataType) {  //对于支持 5.0 版本 ISUP
    //设备的认证，必须设置认证密钥
    auto *pDevInfo = (NET_EHOME_DEV_REG_INFO_V12 *)pOutBuffer;
    ////输出设备信息
    printf("[Device Auth] DeviceID[%s] IP[%s]\n ",
           pDevInfo->struRegInfo.byDeviceID,
           pDevInfo->struRegInfo.struDevAdd.szIP);
    //假设密钥为i#Rhg&8D5lP
    char szEHomeKey[32] = "i#Rhg&8D5lP";
    // Set device EhomeKey for authentication
    strcpy((char *)pInBuffer, szEHomeKey);
  } else if (ENUM_DEV_SESSIONKEY == dwDataType) {
    ////对于支持 5.0 版本 ISUP 设备的认证，必须配置EHomeKey
    auto *pDevInfo = (NET_EHOME_DEV_REG_INFO_V12 *)pOutBuffer;
    printf("[Device SessionKey] DeviceID[%s]\n",
           pDevInfo->struRegInfo.byDeviceID);
    NET_EHOME_DEV_SESSIONKEY struSessionkey = {0};
    memcpy(struSessionkey.sDeviceID, pDevInfo->struRegInfo.byDeviceID,
           MAX_DEVICE_ID_LEN);
    memcpy(struSessionkey.sSessionKey, pDevInfo->struRegInfo.bySessionKey,
           MAX_MASTER_KEY_LEN);
    NET_ECMS_SetDeviceSessionKey(&struSessionkey);
#endif
  } else {  //...
  }
  return TRUE;
}

//处理实时码流数据
BOOL EHomeServer::ProcessInputStreamData(LONG user_id, BYTE byDataType,
                                         char *pBuffer, int iDataLen) {
#if (SMS_RECORD)
  if (video_files_[user_id] == nullptr) {
    auto filename = "Test_" + std::to_string(user_id) + ".mp4";
    video_files_[user_id] = fopen(filename.c_str(), "wb");
    printf("Save data to file: %s!\n", filename.c_str());
  }
  if (video_files_[user_id] != nullptr) {
    fwrite(pBuffer, iDataLen, 1, video_files_[user_id]);
    //回调实时码流并保存视频文件
  }
#endif

  //调用播放库解码并显示码流实现预览
  if (1 == byDataType) {
    if (!PlayM4_GetPort(&sms_port_)) {
      printf("PlayM4_GetPort failed, error code: %d!",
             PlayM4_GetLastError(sms_port_));
      return FALSE;
    }
    if (!PlayM4_SetStreamOpenMode(sms_port_, STREAME_REALTIME)) {
      printf("PlayM4_SetStreamOpenMode failed, error code: %d!",
             PlayM4_GetLastError(sms_port_));
      return FALSE;
    }
    //输入头部的前 40 字节
    if (!PlayM4_OpenStream(sms_port_, (unsigned char *)pBuffer, (DWORD)iDataLen,
                           2 * 1024 * 1024)) {
      printf("PlayM4_OpenStream failed, error code: %d!",
             PlayM4_GetLastError(sms_port_));
      return FALSE;
    }
    if (!PlayM4_SetDecCallBackMend(sms_port_, DecodeCallback,
                                   &frames_[user_id])) {
      printf("PlayM4_InputData failed, error code: %d!",
             PlayM4_GetLastError(sms_port_));
      return FALSE;
    }
    if (!PlayM4_Play(sms_port_, NULL)) {
      printf("PlayM4_Play failed, error code: %d!",
             PlayM4_GetLastError(sms_port_));
      return FALSE;
    }
  } else {
    int time = 1000;
    while (time > 0) {
      BOOL bRet = PlayM4_InputData(sms_port_, (unsigned char *)pBuffer,
                                   (DWORD)iDataLen);
      if (!bRet) {
        usleep(5000);
        time--;
        continue;
      }
      break;
    }
  }
  return TRUE;
}

//注册实时码流回调函数
void EHomeServer::PreviewDataCallback(LONG lPreviewHandle,
                                      NET_EHOME_PREVIEW_CB_MSG *pPreviewCBMsg,
                                      void *pUserData) {
  if (nullptr == pPreviewCBMsg) {
    return;
  }
  auto it = std::find_if(server_->cameras_.begin(), server_->cameras_.end(),
                         [&lPreviewHandle](const IPCamera &x) {
                           return x.stream_handle == lPreviewHandle;
                         });

  if (it != server_->cameras_.end()) {
    auto user_id = it->login_id;
    server_->ProcessInputStreamData(user_id, pPreviewCBMsg->byDataType,
                                    (char *)pPreviewCBMsg->pRecvdata,
                                    pPreviewCBMsg->dwDataLen);
  }
}

//注册预览请求的响应回调函数
BOOL EHomeServer::PreviewNewLinkCallback(
    LONG lPreviewHandle, NET_EHOME_NEWLINK_CB_MSG *pNewLinkCBMsg,
    void *pUserData) {
  //需要将字符串字段转换成GB2312
  //  DWORD dwConvertLen = 0;
  //  UTF82A((char *)pNewLinkCBMsg->szDeviceID, (char
  //  *)pNewLinkCBMsg->szDeviceID,
  //         MAX_DEVICE_ID_LEN, &dwConvertLen);
  printf("Newlink from device id %s", pNewLinkCBMsg->szDeviceID);

  std::string device_id(
      reinterpret_cast<char const *>(pNewLinkCBMsg->szDeviceID));
  auto it = std::find_if(
      server_->cameras_.begin(), server_->cameras_.end(),
      [&device_id](const IPCamera &x) { return x.device_id == device_id; });
  if (it != server_->cameras_.end()) {
    it->stream_handle = lPreviewHandle;
    printf("Callback of preview listening, Device ID: %s, Channel: %d\n",
           pNewLinkCBMsg->szDeviceID, pNewLinkCBMsg->dwChannelNo);

    //预览数据的回调参数
    NET_EHOME_PREVIEW_DATA_CB_PARAM struDataCB = {0};
    struDataCB.fnPreviewDataCB = PreviewDataCallback;
#if (SDK_TYPE == SDK_ISUP)
    struDataCB.byStreamFormat = 0;  //封装格式：0-PS 格式
#endif

    if (!NET_ESTREAM_SetPreviewDataCB(lPreviewHandle, &struDataCB)) {
      printf("NET_ESTREAM_SetPreviewDataCB failed, error code: %d\n",
             NET_ESTREAM_GetLastError());
      return FALSE;
    }
    printf("NET_ESTREAM_SetPreviewDataCB succeed!\n");

    return TRUE;
  } else {
    return FALSE;
  }
}
