#include <csignal>
#include <cstring>

#include "config.h"
#include "ip_camera_manager.h"

LONG lListen = -1, lHandle = -1;
BOOL stop_flag = false;

void Stop(int sig) {
  //登出
  stop_flag = true;

#if (SMS_RECORD)
  if (video_file_ != nullptr) {
    fclose(video_file_);
    video_file_ = nullptr;
  }
#endif

  //释放被 CMS 预览请求占用的资源
  for (auto i = 0; i < IPCS_MAX_NUM; ++i) {
    if (cameras_[i].preview_session_id >= 0) {
      if (!NET_ECMS_StopGetRealStream(cameras_[i].login_id,
                                      cameras_[i].preview_session_id)) {
        printf("NET_ECMS_StopGetRealStream failed, error code: %d\n",
               NET_ECMS_GetLastError());
      }
    }
  }

  // CMS 停止监听
  if (lListen >= 0) {
    if (!NET_ECMS_StopListen(lListen)) {
      printf("NET_ECMS_StopListen failed, error code: %d\n",
             NET_ECMS_GetLastError());
    }
  }

  //释放被 CMS 占用的资源
  NET_ECMS_Fini();

  // SMS 停止转发码流
  for (auto i = 0; i < IPCS_MAX_NUM; ++i) {
    if (cameras_[i].stream_handle >= 0) {
      if (!NET_ESTREAM_StopPreview(cameras_[i].stream_handle)) {
        printf("NET_ESTREAM_StopPreview failed, error code: %d\n",
               NET_ESTREAM_GetLastError());
      }
    }

#if (SDK_TYPE == SDK_ISUP)
    if (cameras_[i].preview_session_id >= 0) {
      NET_EHOME_STOPSTREAM_PARAM struStopParam = {0};
      struStopParam.lSessionID = cameras_[i].preview_session_id;
      if (!NET_ECMS_StopGetRealStreamEx(cameras_[i].login_id, &struStopParam)) {
        printf("NET_ECMS_StopGetRealStreamEx failed, error code: %d\n",
               NET_ESTREAM_GetLastError());
      }
    }
#endif
  }

  // SMS 停止监听
  if (lHandle >= 0) {
    if (!NET_ESTREAM_StopListenPreview(lHandle)) {
      printf("NET_ESTREAM_StopListenPreview failed, error code: %d\n",
             NET_ESTREAM_GetLastError());
    }
  }
  //释放被 SMS 占用的资源
  NET_ESTREAM_Fini();

  printf("Exit!\n");
}

int main() {
  //注册和预览请求
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
  lListen = NET_ECMS_StartListen(&struCMSListenPara);
  if (lListen <= -1) {
    printf("NET_ECMS_StartListen failed, error code: %d\n",
           NET_ECMS_GetLastError());
    NET_ECMS_Fini();
    return 1;
  }
  printf("NET_ECMS_StartListen succeed!\n");
  DWORD dwVersion = NET_ECMS_GetBuildVersion();
  DWORD dwV1 = (dwVersion >> 24);
  DWORD dwV2 = ((dwVersion & 0x00FF0000) >> 16);
  DWORD dwV3 = ((dwVersion & 0x0000FF00) >> 8);
  DWORD dwV4 = (dwVersion & 0x000000FF);
  printf("NET_ECMS_GetBuildVersion, %d.%d.%d.%d\n", dwV1, dwV2, dwV3, dwV4);

  // SMS 在监听开启后获取码流
  // 初始化 SMS 库
  NET_ESTREAM_Init();
  NET_ESTREAM_SetLogToFile((DWORD)3, (char *)"/tmp/", FALSE);
#if (SDK_TYPE == SDK_ISUP)
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

  // 预览的监听参数
  NET_EHOME_LISTEN_PREVIEW_CFG struListen = {0};
  memcpy(struListen.struIPAdress.szIP, SMS_LISTEN_IP,
         sizeof(NET_EHOME_IPADDRESS));
  // SMS 的监听端口号
  struListen.struIPAdress.wPort = SMS_LISTEN_PORT;
  struListen.fnNewLinkCB = PreviewNewLinkCallback;
  // 预览请求回调函数
  struListen.pUser = nullptr;
  struListen.byLinkMode = SMS_PREVIEW_LINK_MODE;  // 0-TCP, 1-UDP

  // 开启监听
  lHandle = NET_ESTREAM_StartListenPreview(&struListen);
  if (lHandle <= -1) {
    printf("NET_ESTREAM_StartListenPreview failed, error code: %d\n",
           NET_ESTREAM_GetLastError());
    NET_ESTREAM_Fini();
    return 1;
  }
  printf("NET_ESTREAM_StartListenPreview succeed!\n");

  dwVersion = NET_ESTREAM_GetBuildVersion();
  dwV1 = (dwVersion >> 24);
  dwV2 = ((dwVersion & 0x00FF0000) >> 16);
  dwV3 = ((dwVersion & 0x0000FF00) >> 8);
  dwV4 = (dwVersion & 0x000000FF);
  printf("NET_ESTREAM_GetBuildVersion, %d.%d.%d.%d\n", dwV1, dwV2, dwV3, dwV4);

  // Register signals
  signal(SIGINT, Stop);

  while (!stop_flag) {
    // The following operations should be done when the
    // registration is completed

    sleep(1);
    for (auto i = 0; i < IPCS_MAX_NUM; ++i) {
      if (cameras_[i].online_state == IPCS_ONLINE &&
          cameras_[i].push_state != IPCS_PUSHING_STREAM) {
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
          printf("%s\n", sOutBuf);
        }

        //预览请求
        NET_EHOME_PREVIEWINFO_IN struPreviewIn = {0};
        NET_EHOME_PREVIEWINFO_OUT struPreviewOut = {0};
        struPreviewIn.iChannel = SMS_PREVIEW_CHANNEL;
        struPreviewIn.dwLinkMode = SMS_PREVIEW_LINK_MODE;
        struPreviewIn.dwStreamType = SMS_PREVIEW_STREAM_TYPE;
        memcpy(struPreviewIn.struStreamSever.szIP, SMS_PUBLIC_IP,
               sizeof(NET_EHOME_IPADDRESS));
        struPreviewIn.struStreamSever.wPort = SMS_LISTEN_PORT;
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
          return 1;
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
          return 1;
        }
        printf("NET_ECMS_StartPushRealStream succeed!\n");
        cameras_[i].preview_session_id = struPreviewOut.lSessionID;
        cameras_[i].push_state = IPCS_PUSHING_STREAM;
      }
    }
  }

  return 0;
}