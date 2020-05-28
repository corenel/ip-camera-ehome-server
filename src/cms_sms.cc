#include "cms_sms.h"

#include "opencv2/opencv.hpp"

void CALLBACK DecCBFun(int nPort, char *pBuf, int nSize, FRAME_INFO *pFrameInfo,
                       void *nUser, int nReserved2) {
  if (pFrameInfo->nType == T_YV12) {
    std::cout << "the frame infomation is T_YV12" << std::endl;
    cv::Mat g_BGRImage;
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
BOOL CALLBACK RegisterCallBack(LONG lUserID, DWORD dwDataType, void *pOutBuffer,
                               DWORD dwOutLen, void *pInBuffer, DWORD dwInLen,
                               void *pUser) {
  if (ENUM_DEV_ON == dwDataType) {
    auto *pDevInfo = (NET_EHOME_DEV_REG_INFO *)pOutBuffer;
    if (pDevInfo != nullptr) {
      lLoginID = lUserID;

      printf("On-line, lUserID: %d, Device ID: %s, DevProtocolVersion: %s\n",
             lLoginID, pDevInfo->byDeviceID, pDevInfo->byDevProtocolVersion);
    }

    //输入参数
    auto *pServerInfo = (NET_EHOME_SERVER_INFO *)pInBuffer;
    pServerInfo->dwTimeOutCount = 6;   //心跳超时次数
    pServerInfo->dwKeepAliveSec = 5;  //心跳间隔

  } else if (ENUM_DEV_OFF == dwDataType) {
    printf("Off-line, lUserID: %d\n", lUserID);
    NET_ECMS_ForceLogout(lUserID);
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
  } else {  //...
  }
  return TRUE;
}

//处理实时码流数据
BOOL InputStreamData(BYTE byDataType, char *pBuffer, int iDataLen) {
  if (Videofile == nullptr) {
    Videofile = fopen("Test.mp4", "wb");

    printf("Save data to file: Test.mp4!\n");
  }

  if (Videofile != nullptr) {
    fwrite(pBuffer, iDataLen, 1, Videofile);
    //回调实时码流并保存视频文件
  }
  //调用播放库解码并显示码流实现预览
  if (1 == byDataType) {
    if (!PlayM4_GetPort(&m_lPort)) {
      return FALSE;
    }
    if (!PlayM4_SetStreamOpenMode(m_lPort, STREAME_REALTIME)) {
      return FALSE;
    }
    //输入头部的前 40 字节
    if (!PlayM4_OpenStream(m_lPort, (unsigned char *)pBuffer, (DWORD)iDataLen,
                           2 * 1024 * 1024)) {
      return FALSE;
    }
    if (!PlayM4_SetDecCallBack(m_lPort, DecCBFun)) {
      printf("PlayM4_InputData failed, error code: %d!",
             PlayM4_GetLastError(m_lPort));
      return FALSE;
    }
    if (!PlayM4_Play(m_lPort, NULL)) {
      return FALSE;
    }

  } else {
    for (int i = 0; i < 1000; i++)

    {
      BOOL bRet =
          PlayM4_InputData(m_lPort, (unsigned char *)pBuffer, (DWORD)iDataLen);
      if (!bRet) {
        if (i >= 999)

        {
          printf("PlayM4_InputData failed, error code: %d!",
                 PlayM4_GetLastError(m_lPort));
        }
        sleep(2);
      }
    }
  }
  return TRUE;
}

//注册实时码流回调函数
void CALLBACK fnPREVIEW_DATA_CB(LONG lPreviewHandle,
                                NET_EHOME_PREVIEW_CB_MSG *pPreviewCBMsg,
                                void *pUserData) {
  if (nullptr == pPreviewCBMsg) {
    return;
  }

  lRealHandle = lPreviewHandle;

  InputStreamData(pPreviewCBMsg->byDataType, (char *)pPreviewCBMsg->pRecvdata,
                  pPreviewCBMsg->dwDataLen);
}

//注册预览请求的响应回调函数
BOOL CALLBACK fnPREVIEW_NEWLINK_CB(LONG lPreviewHandle,
                                   NET_EHOME_NEWLINK_CB_MSG *pNewLinkCBMsg,
                                   void *pUserData) {
  lLinkHandle = lPreviewHandle;

  printf("Callback of preview listening, Device ID: %s, Channel: %d\n",
         pNewLinkCBMsg->szDeviceID, pNewLinkCBMsg->dwChannelNo);

  //预览数据的回调参数
  NET_EHOME_PREVIEW_DATA_CB_PARAM struDataCB = {0};
  struDataCB.fnPreviewDataCB = fnPREVIEW_DATA_CB;
  struDataCB.byStreamFormat = 0;  //封装格式：0-PS 格式

  if (!NET_ESTREAM_SetPreviewDataCB(lPreviewHandle, &struDataCB)) {
    printf("NET_ESTREAM_SetPreviewDataCB failed, error code: %d\n",
           NET_ESTREAM_GetLastError());
    return FALSE;
  }
  printf("NET_ESTREAM_SetPreviewDataCB!\n");

  return TRUE;
}
