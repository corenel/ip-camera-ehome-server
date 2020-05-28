#include <csignal>
#include <cstring>

#include "ip_camera_manager.h"

LONG lListen, lHandle;

void Stop(int sig) {
  //登出

  //释放被 CMS 预览请求占用的资源
  for (auto i = 0; i < IPCS_MAX_NUM; ++i) {
    if (!NET_ECMS_StopGetRealStream(cameras_[i].login_id,
                                    cameras_[i].preview_session_id)) {
      printf("NET_ECMS_StopGetRealStream failed, error code: %d\n",
             NET_ECMS_GetLastError());
    }
  }

  // CMS 停止监听
  if (!NET_ECMS_StopListen(lListen)) {
    printf("NET_ECMS_StopListen failed, error code: %d\n",
           NET_ECMS_GetLastError());
  }

  //释放被 CMS 占用的资源
  NET_ECMS_Fini();

  // SMS 停止转发码流
  for (auto i = 0; i < IPCS_MAX_NUM; ++i) {
    if (cameras_[i].stream_handle >= 0) {
      if (!NET_ESTREAM_StopPreview(cameras_[i].stream_handle))

      {
        printf("NET_ESTREAM_StopPreview failed, error code: %d\n",
               NET_ECMS_GetLastError());
      }
    }
  }

  // SMS 停止监听
  if (lHandle >= 0) {
    if (!NET_ESTREAM_StopListenPreview(lHandle)) {
      printf("NET_ESTREAM_StopListenPreview failed, error code: %d\n",
             NET_ECMS_GetLastError());
    }
  }  //释放被 SMS 占用的资源 NET_ESTREAM_Fini();

  printf("Exit!\n");
}

int main() {
  // SMS 在监听开启后获取码流

  //初始化 SMS 库
  NET_ESTREAM_Init();
  //预览的监听参数
  NET_EHOME_LISTEN_PREVIEW_CFG struListen = {0};
  memcpy(struListen.struIPAdress.szIP, SMS_PREVIEW_STREAM_SERVER_IP,
         sizeof(SMS_PREVIEW_STREAM_SERVER_IP));
  // SMS 的监听端口号
  struListen.struIPAdress.wPort = SMS_PREVIEW_STREAM_SERVER_PORT;
  struListen.fnNewLinkCB = PreviewNewLinkCallback;
  //预览请求回调函数
  struListen.pUser = nullptr;
  struListen.byLinkMode = SMS_PREVIEW_LINK_MODE;  // 0-TCP, 1-UDP

  //开启监听
  lHandle = NET_ESTREAM_StartListenPreview(&struListen);
  if (lHandle < -1) {
    printf("NET_ESTREAM_StartListenPreview failed, error code: %d\n",
           NET_ESTREAM_GetLastError());
    NET_ESTREAM_Fini();
    return 1;
  }
  printf("NET_ESTREAM_StartListenPreview!\n");

  //注册和预览请求
  //初始化 CMS 库
  NET_ECMS_Init();

  //注册的监听参数
  NET_EHOME_CMS_LISTEN_PARAM struCMSListenPara = {0};
  memcpy(struCMSListenPara.struAddress.szIP, CMS_LISTEN_IP,
         sizeof(CMS_LISTEN_IP));
  struCMSListenPara.struAddress.wPort = CMS_LISTEN_PORT;
  struCMSListenPara.fnCB = RegistrationCallBack;

  //开启监听并接收设备注册信息
  lListen = NET_ECMS_StartListen(&struCMSListenPara);
  if (lListen < -1) {
    printf("NET_ECMS_StartListen failed, error code: %d\n",
           NET_ECMS_GetLastError());

    NET_ECMS_Fini();

    return 1;
  }
  printf("NET_ECMS_StartListen!\n");

  // Register signals
  signal(SIGINT, Stop);

  while (true) {
    // The following operations should be done when the
    // registration is completed
    sleep(1);
    for (auto i = 0; i < IPCS_MAX_NUM; ++i) {
      if (cameras_[i].online_state == IPCS_ONLINE &&
          cameras_[i].push_state != IPCS_PUSHING_STREAM) {
        //预览请求的输入参数
        NET_EHOME_PREVIEWINFO_IN_V11 struPreviewIn = {0};
        //通道号
        struPreviewIn.iChannel = SMS_PREVIEW_CHANNEL;
        // 0-TCP, 1-UDP
        struPreviewIn.dwLinkMode = SMS_PREVIEW_LINK_MODE;
        //码流类型：0-主码流，1-子码流 2-第三码流
        struPreviewIn.dwStreamType = SMS_PREVIEW_STREAM_TYPE;
        // SMS 的 IP 地址
        memcpy(struPreviewIn.struStreamSever.szIP, SMS_PREVIEW_STREAM_SERVER_IP,
               sizeof(SMS_PREVIEW_STREAM_SERVER_IP));
        // SMS 的端口号，需和监听端口号一致
        struPreviewIn.struStreamSever.wPort = SMS_PREVIEW_STREAM_SERVER_PORT;

        //预览请求的输出参数
        NET_EHOME_PREVIEWINFO_OUT struPreviewOut = {0};

        //预览请求
        if (!NET_ECMS_StartGetRealStreamV11(cameras_[i].login_id,
                                            &struPreviewIn, &struPreviewOut)) {
          printf("NET_ECMS_StartGetRealStreamV11 failed, error code: %d\n",
                 NET_ECMS_GetLastError());
          NET_ECMS_Fini();
          return 1;
        }
        cameras_[i].preview_session_id = struPreviewOut.lSessionID;
        printf("NET_ECMS_StartGetRealStreamV11!\n");

        //码流传输请求的输入参数
        NET_EHOME_PUSHSTREAM_IN struPushStreamIn = {0};
        struPushStreamIn.dwSize = sizeof(struPushStreamIn);
        struPushStreamIn.lSessionID =
            struPreviewOut.lSessionID;  //预览请求的会话 ID

        //码流传输请求的输出参数
        NET_EHOME_PUSHSTREAM_OUT struPushStreamOut = {0};

        //发送请求给设备并开始传输实时码流，该操作只对支持 4.0 版本及以上 ISUP
        //的设备有效
        if (!NET_ECMS_StartPushRealStream(
                cameras_[i].login_id, &struPushStreamIn, &struPushStreamOut)) {
          printf("NET_ECMS_StartPushRealStream failed, error code: %d\n",
                 NET_ECMS_GetLastError());
          NET_ECMS_Fini();
          return 1;
        }
        printf("NET_ECMS_StartPushRealStream!\n");
        cameras_[i].push_state = IPCS_PUSHING_STREAM;
      }
    }
  }

  return 0;
}