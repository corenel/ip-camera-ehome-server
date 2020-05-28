#include "ip_camera_manager.h"

void CALLBACK DecodeCallback(int nPort, char *pBuf, int nSize,
                             FRAME_INFO *pFrameInfo, void *nUser,
                             int nReserved2) {
  if (pFrameInfo->nType == T_YV12) {
    std::cout << "the frame infomation is T_YV12" << std::endl;
    int *user_id = (int *)nUser;
    cv::Mat &g_BGRImage = frames_[*user_id];
    if (g_BGRImage.empty()) {
      g_BGRImage.create(pFrameInfo->nHeight, pFrameInfo->nWidth, CV_8UC3);
    }
    cv::Mat YUVImage(pFrameInfo->nHeight + pFrameInfo->nHeight / 2,
                     pFrameInfo->nWidth, CV_8UC1, (unsigned char *)pBuf);

    cv::cvtColor(YUVImage, g_BGRImage, cv::COLOR_YUV2BGR_YV12);
    std::cout << YUVImage.cols << " " << YUVImage.rows << std::endl;
    //    cv::imshow("RGBImage", g_BGRImage);
    //    cv::waitKey(15);
    YUVImage.~Mat();
  }
}

//注册回调函数
BOOL CALLBACK RegistrationCallBack(LONG lUserID, DWORD dwDataType,
                                   void *pOutBuffer, DWORD dwOutLen,
                                   void *pInBuffer, DWORD dwInLen,
                                   void *pUser) {
  if (ENUM_DEV_ON == dwDataType) {
    auto *pDevInfo = (NET_EHOME_DEV_REG_INFO *)pOutBuffer;
    if (pDevInfo != nullptr) {
      cameras_[lUserID].login_id = lUserID;
      std::string device_id(
          reinterpret_cast<char const *>(pDevInfo->byDeviceID));
      cameras_[lUserID].device_id = device_id;
      cameras_[lUserID].device_ip = pDevInfo->struDevAdd.szIP;
      cameras_[lUserID].online_state = IPCS_ONLINE;
      printf("On-line, lUserID: %d, Device ID: %s\n", lUserID,
             pDevInfo->byDeviceID);
    }

    //输入参数
    auto *pServerInfo = (NET_EHOME_SERVER_INFO *)pInBuffer;
    pServerInfo->dwTimeOutCount = 6;  //心跳超时次数
    pServerInfo->dwKeepAliveSec = 5;  //心跳间隔

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
BOOL ProcessInputStreamData(LONG user_id, BYTE byDataType, char *pBuffer,
                            int iDataLen) {
  //  if (Videofile == nullptr) {
  //    Videofile = fopen("Test.mp4", "wb");
  //
  //    printf("Save data to file: Test.mp4!\n");
  //  }
  //
  //  if (Videofile != nullptr) {
  //    fwrite(pBuffer, iDataLen, 1, Videofile);
  //    //回调实时码流并保存视频文件
  //  }
  //调用播放库解码并显示码流实现预览
  if (1 == byDataType) {
    if (!PlayM4_GetPort(&port_)) {
      return FALSE;
    }
    if (!PlayM4_SetStreamOpenMode(port_, STREAME_REALTIME)) {
      return FALSE;
    }
    //输入头部的前 40 字节
    if (!PlayM4_OpenStream(port_, (unsigned char *)pBuffer, (DWORD)iDataLen,
                           2 * 1024 * 1024)) {
      return FALSE;
    }
    if (!PlayM4_SetDecCallBackMend(port_, DecodeCallback, &user_id)) {
      printf("PlayM4_InputData failed, error code: %d!",
             PlayM4_GetLastError(port_));
      return FALSE;
    }
    if (!PlayM4_Play(port_, NULL)) {
      return FALSE;
    }

  } else {
    for (int i = 0; i < 1000; i++)

    {
      BOOL bRet =
          PlayM4_InputData(port_, (unsigned char *)pBuffer, (DWORD)iDataLen);
      if (!bRet) {
        if (i >= 999)

        {
          printf("PlayM4_InputData failed, error code: %d!",
                 PlayM4_GetLastError(port_));
        }
        sleep(2);
      }
    }
  }
  return TRUE;
}

//注册实时码流回调函数
void CALLBACK PreviewDataCallback(LONG lPreviewHandle,
                                  NET_EHOME_PREVIEW_CB_MSG *pPreviewCBMsg,
                                  void *pUserData) {
  if (nullptr == pPreviewCBMsg) {
    return;
  }
  auto it = std::find_if(cameras_.begin(), cameras_.end(),
                         [&lPreviewHandle](const IPCamera &x) {
                           return x.stream_handle == lPreviewHandle;
                         });

  if (it != cameras_.end()) {
    auto user_id = it->login_id;
    ProcessInputStreamData(user_id, pPreviewCBMsg->byDataType,
                           (char *)pPreviewCBMsg->pRecvdata,
                           pPreviewCBMsg->dwDataLen);
  }
}

//注册预览请求的响应回调函数
BOOL CALLBACK PreviewNewLinkCallback(LONG lPreviewHandle,
                                     NET_EHOME_NEWLINK_CB_MSG *pNewLinkCBMsg,
                                     void *pUserData) {
  std::string device_id(
      reinterpret_cast<char const *>(pNewLinkCBMsg->szDeviceID));
  auto it = std::find_if(
      cameras_.begin(), cameras_.end(),
      [&device_id](const IPCamera &x) { return x.device_id == device_id; });
  if (it != cameras_.end()) {
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
    printf("NET_ESTREAM_SetPreviewDataCB!\n");

    return TRUE;
  } else {
    return FALSE;
  }
}
