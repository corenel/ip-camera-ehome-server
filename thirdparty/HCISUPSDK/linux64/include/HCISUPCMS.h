#ifndef _HC_EHOME_CMS_H_
#define _HC_EHOME_CMS_H_

#include "HCISUPPublic.h"

typedef enum tagNET_EHOME_CMS_INIT_CFG_TYPE
{
    NET_EHOME_CMS_INIT_CFG_LIBEAY_PATH = 0, //设置OpenSSL的libeay32.dll/libcrypto.so所在路径
    NET_EHOME_CMS_INIT_CFG_SSLEAY_PATH = 1  //设置OpenSSL的ssleay32.dll/libssl.so所在路径
}NET_EHOME_CMS_INIT_CFG_TYPE;

#define EHOME_PROTOCOL_LEN  8*1024

//初始化，反初始化
NET_DVR_API BOOL  CALLBACK NET_ECMS_Init();
NET_DVR_API BOOL  CALLBACK NET_ECMS_Fini();

//初始化之前的配置，初始化之后不可调用
NET_DVR_API BOOL  CALLBACK NET_ECMS_SetSDKInitCfg(NET_EHOME_CMS_INIT_CFG_TYPE enumType, void* const lpInBuff);

//获取错误码
NET_DVR_API DWORD CALLBACK NET_ECMS_GetLastError();

//获取版本号
NET_DVR_API DWORD CALLBACK NET_ECMS_GetBuildVersion();

//开启关闭监听
typedef enum tagNET_EHOME_REGISTER_TYPE
{
    ENUM_UNKNOWN                = -1,
    ENUM_DEV_ON                 = 0,    //设备上线回调
    ENUM_DEV_OFF,                       //设备下线回调
    ENUM_DEV_ADDRESS_CHANGED,           //设备地址发生变化
    ENUM_DEV_AUTH,                      //Ehome5.0设备认证回调
    ENUM_DEV_SESSIONKEY,                //Ehome5.0设备Sessionkey回调
    ENUM_DEV_DAS_REQ,                   //Ehome5.0设备重定向请求回调
    ENUM_DEV_SESSIONKEY_REQ,            //EHome5.0设备sessionkey请求回调
    ENUM_DEV_DAS_REREGISTER,            //设备重注册回调
    ENUM_DEV_DAS_PINGREO,               //设备注册心跳
    ENUM_DEV_DAS_EHOMEKEY_ERROR,        //校验密码失败
    ENUM_DEV_SESSIONKEY_ERROR,           //Sessionkey交互异常
    ENUM_DEV_SLEEP = 11           //设备进入休眠状态（注：休眠状态下，设备无法做预览、回放、语音对讲、配置等CMS中的信令作响应；设备可通过NET_ECMS_WakeUp接口进行唤醒）
}NET_EHOME_REGISTER_TYPE;

//语音对讲编码类型
typedef enum tagNET_EHOME_TALK_ENCODING_TYPE
{
    ENUM_ENCODING_START     = 0,        //编码类型开始
    ENUM_ENCODING_G722_1,               //G722_1
    ENUM_ENCODING_G711_MU,              //G711_MU
    ENUM_ENCODING_G711_A,               //G711_A
    ENUM_ENCODING_G723,                 //G723
    ENUM_ENCODING_MP1L2,                //MP1L2
    ENUM_ENCODING_MP2L2,                //MP2L2
    ENUM_ENCODING_G726,                 //G726
    ENUM_ENCODING_AAC,                  //AAC
    ENUM_ENCODING_RAW       = 100       //RAW
}NET_EHOME_TALK_ENCODING_TYPE;

typedef BOOL (CALLBACK * DEVICE_REGISTER_CB)(LONG lUserID, DWORD dwDataType, void* pOutBuffer, DWORD dwOutLen,
                                             void* pInBuffer, DWORD dwInLen, void* pUser);

typedef struct tagNET_EHOME_CMS_LISTEN_PARAM
{
    NET_EHOME_IPADDRESS struAddress;    //本地监听信息，IP为0.0.0.0的情况下，默认为本地地址，多个网卡的情况下，默认为从操作系统获取到的第一个
    DEVICE_REGISTER_CB  fnCB;           //报警信息回调函数
    void*               pUserData;      //用户数据
    DWORD               dwKeepAliveSec; //心跳间隔（单位：秒,0:默认为30S）
    DWORD               dwTimeOutCount; //心跳超时次数（0：默认为3）
    BYTE                byRes[24];
}NET_EHOME_CMS_LISTEN_PARAM, *LPNET_EHOME_CMS_LISTEN_PARAM;

//开启关闭监听
NET_DVR_API LONG CALLBACK NET_ECMS_StartListen(LPNET_EHOME_CMS_LISTEN_PARAM lpCMSListenPara);
NET_DVR_API BOOL CALLBACK NET_ECMS_StopListen(LONG iHandle);
//注销设备
NET_DVR_API BOOL CALLBACK NET_ECMS_ForceLogout(LONG lUserID);
NET_DVR_API BOOL CALLBACK NET_ECMS_SetLogToFile(DWORD iLogLevel, char* strLogDir, BOOL bAutoDel);

typedef enum tagNET_EHOME_CALLBACK_TYPE
{
    NET_EHOME_REALSTREAM_CB = 0, //取流异步回调
    NET_EHOME_PLAYBACK_CB,       //回放异步回调
    NET_EHOME_VOICETALK_CB,       //语音对讲回调
    NET_EHOME_PASSTHROUGH_CB,    //ISAPI透传异步回调
    NET_EHOME_V2_VOICETALK_CB     //2.0语音对讲异步回调
} NET_EHOME_CALLBACK_TYPE;

typedef struct tagNET_EHOME_CMSCB_DATA
{
    LONG   lUserID;      //用户ID
    DWORD  dwType;       //回调类型
    DWORD  dwHandle;     //消息句柄，唯一标识
    BOOL   bSucc;        //对应同步下的接口返回值
    void*  pOutBuffer;   //设备响应数据
    DWORD  dwOutLen;     //设备响应数据长度
    DWORD  dwErrorNo;    //SDK错误码
    BYTE   byRes[32];    //保留字节，用于扩展
} NET_EHOME_CMSCB_DATA;

#define ECMS_CB_TYPE_UNDEFINE                  0
#define ECMS_CB_TYPE_START_GET_REALSTREAM      1
#define ECMS_CB_TYPE_START_PUSH_REALSTREAM     3
#define ECMS_CB_TYPE_STOP_GET_REALSTREAM_EX    4
#define ECMS_CB_TYPE_START_PLAYBACK            5
#define ECMS_CB_TYPE_START_PUSH_PLAYBACK       6
#define ECMS_CB_TYPE_STOP_PLAYBACK_EX          7
#define ECMS_CB_STOP_VOICETALK_EX              9
#define ECMS_CB_TYPE_START_VOICETALK_STM       10
#define ECMS_CB_TYPE_START_PUSH_VOICE_STREAM   11
#define ECMS_CB_TYPE_STOP_VOICE_TALK_STM_EX    12
#define ECMS_CB_TYPE_ISAPI_PASSTHROUGH         13
#define ECMS_CB_TYPE_START_V2_VOICETALK        14
#define ECMS_CB_TYPE_PAUSE_PLAYBACK            15
#define ECMS_CB_TYPE_RESTART_PLAYBACK          16

typedef void(CALLBACK *ECMSCallback)(NET_EHOME_CMSCB_DATA *pData, void* pUser);

NET_DVR_API BOOL CALLBACK NET_ECMS_SetCallback(NET_EHOME_CALLBACK_TYPE enumCallbackType, ECMSCallback fnCallback, void* pUser);

#define FIRMWARE_VER_LEN    24
#define CODE_LEN            8
#define MAX_DEVNAME_LEN     32
#define MAX_DEVNAME_LEN_EX  64      //设备名称长度扩展

typedef struct tagNET_EHOME_DEV_REG_INFO
{
    DWORD  dwSize;
    DWORD  dwNetUnitType;                   //根据EHomeSDK协议预留，目前没有意义
    BYTE   byDeviceID[MAX_DEVICE_ID_LEN];   //设备ID
    BYTE   byFirmwareVersion[24];           //固件版本
    NET_EHOME_IPADDRESS struDevAdd;         //设备注册上来是，设备的本地地址
    DWORD  dwDevType;                       //设备类型
    DWORD  dwManufacture;                   //设备厂家代码
    BYTE   byPassWord[32];                  //设备登陆CMS的密码，由用户自行根据需求进行验证
    BYTE   sDeviceSerial[NET_EHOME_SERIAL_LEN/*12*/];   //设备序列号，数字序列号
    BYTE   byReliableTransmission;
    BYTE   byWebSocketTransmission;
    BYTE   bySupportRedirect;               //设备支持重定向注册 0-不支持 1-支持
    BYTE   byDevProtocolVersion[6];         //设备协议版本
    BYTE   bySessionKey[MAX_MASTER_KEY_LEN];//Ehome5.0设备SessionKey
    BYTE   byMarketType; //0-无效（未知类型）,1-经销型，2-行业型
    BYTE   byRes[26];
}NET_EHOME_DEV_REG_INFO, *LPNET_EHOME_DEV_REG_INFO;

typedef struct tagNET_EHOME_DEV_REG_INFO_V12
{
    NET_EHOME_DEV_REG_INFO struRegInfo;
    NET_EHOME_IPADDRESS struRegAddr;         //设备注册的服务器地址
    BYTE   sDevName[MAX_DEVNAME_LEN_EX];     //设备名称
    BYTE   byDeviceFullSerial[MAX_FULL_SERIAL_NUM_LEN];   //设备完整序列号，设备型号+日期+短序号+扩展
    BYTE   byRes[128];
}NET_EHOME_DEV_REG_INFO_V12, *LPNET_EHOME_DEV_REG_INFO_V12;

typedef struct tagNET_EHOME_BLACKLIST_SEVER
{
    NET_EHOME_IPADDRESS struAdd;            //服务器地址
    BYTE    byServerName[NAME_LEN/*32*/];   //服务器名称
    BYTE    byUserName[NAME_LEN/*32*/];     //用户名
    BYTE    byPassWord[NAME_LEN/*32*/];     //密码
    BYTE    byRes[64];
}NET_EHOME_BLACKLIST_SEVER, *LPNET_EHOME_BLACKLIST_SEVER;

typedef struct tagNET_EHOME_SERVER_INFO
{
    DWORD               dwSize;
    DWORD               dwKeepAliveSec;         //心跳间隔（单位：秒,0:默认为15S）
    DWORD               dwTimeOutCount;         //心跳超时次数（0：默认为6）
    NET_EHOME_IPADDRESS struTCPAlarmSever;      //报警服务器地址（TCP协议）
    NET_EHOME_IPADDRESS struUDPAlarmSever;      //报警服务器地址（UDP协议）
    DWORD               dwAlarmServerType;      //报警服务器类型0-只支持UDP协议上报，1-支持UDP、TCP两种协议上报，2 - 采用MQTT协议方式上报（此时服务器地址为TCP协议服务地址）
    NET_EHOME_IPADDRESS struNTPSever;           //NTP服务器地址
    DWORD               dwNTPInterval;          //NTP校时间隔（单位：秒）
    NET_EHOME_IPADDRESS struPictureSever;       //图片服务器地址
    DWORD               dwPicServerType;        //图片服务器类型图片服务器类型，1-VRB图片服务器，0-Tomcat图片服务,2-云存储3,3-KMS,4-EHome5.0存储协议
    NET_EHOME_BLACKLIST_SEVER   struBlackListServer;    //黑名单服务器
    BYTE                        byRes[128];
}NET_EHOME_SERVER_INFO, *LPNET_EHOME_SERVER_INFO;

typedef struct tagNET_EHOME_SERVER_INFO_V50
{
    DWORD                   dwSize;
    DWORD                   dwKeepAliveSec;         //心跳间隔（单位：秒,0:默认为15S）
    DWORD                   dwTimeOutCount;         //心跳超时次数（0：默认为6）
    NET_EHOME_IPADDRESS     struTCPAlarmSever;      //报警服务器地址（TCP协议）
    NET_EHOME_IPADDRESS     struUDPAlarmSever;      //报警服务器地址（UDP协议）
    DWORD                   dwAlarmServerType;      //报警服务器类型0-只支持UDP协议上报，1-支持UDP、TCP两种协议上报
    NET_EHOME_IPADDRESS     struNTPSever;           //NTP服务器地址
    DWORD                   dwNTPInterval;          //NTP校时间隔（单位：秒）
    NET_EHOME_IPADDRESS     struPictureSever;       //图片服务器地址
    DWORD                   dwPicServerType;        //图片服务器类型图片服务器类型，1-VRB图片服务器，0-Tomcat图片服务,2-云存储3,3-KMS
    NET_EHOME_BLACKLIST_SEVER   struBlackListServer;//黑名单服务器
    NET_EHOME_IPADDRESS     struRedirectSever;      //Redirect Server
    BYTE                    byClouldAccessKey[64];  //云存储AK
    BYTE                    byClouldSecretKey[64];  //云存储SK
    BYTE                    byClouldHttps;          //云存储HTTPS使能 1-HTTPS 0-HTTP
    BYTE                    byRes1[3];
    DWORD                   dwAlarmKeepAliveSec;    //报警心跳间隔（单位：秒,0:默认为30s）
    DWORD                   dwAlarmTimeOutCount;    //报警心跳超时次数（0：默认为3）
    BYTE                    byRes[372];
}NET_EHOME_SERVER_INFO_V50, *LPNET_EHOME_SERVER_INFO_V50;

//-----------------------------------------------------------------------------------------------------------
//语音对讲

typedef void(CALLBACK *fVoiceDataCallBack)(LONG iVoiceHandle, char* pRecvDataBuffer, DWORD dwBufSize, DWORD dwEncodeType, BYTE byAudioFlag, void* pUser);

typedef struct tagNET_EHOME_VOICETALK_PARA
{
    BOOL           bNeedCBNoEncData; //需要回调的语音类型：0-编码后语音，1-编码前语音（语音转发时不支持）
    fVoiceDataCallBack  cbVoiceDataCallBack; //用于回调音频数据的回调函数
    DWORD          dwEncodeType;    //SDK赋值,SDK的语音编码类型,0- OggVorbis，1-G711U，2-G711A，3-G726，4-AAC，5-MP2L2，6-PCM,7-MP3
    void*          pUser;           //用户参数
    BYTE           byVoiceTalk;     //0-语音对讲,1-语音转发,2-语音广播
    BYTE           byDevAudioEnc;   //输出参数，设备的音频编码方式 0- OggVorbis，1-G711U，2-G711A，3-G726，4-AAC，5-MP2L2，6-PCM,7-MP3
    BYTE           byRes1[2];
    LONG           lHandle;         //（输出参数）设置了回放异步回调之后，该值为消息句柄，回调中用于标识
    BYTE           byRes[56];       //Reserved, set as 0. 0
} NET_EHOME_VOICETALK_PARA,*LPNET_EHOME_VOICETALK_PARA;

typedef struct tagNET_EHOME_VOICE_TALK_IN
{
    DWORD dwVoiceChan;                                  //通道号
    NET_EHOME_IPADDRESS struStreamSever;                //流媒体地址
    NET_EHOME_TALK_ENCODING_TYPE  byEncodingType[9];    //语音对讲编码类型
    BYTE  byRes[119];
}NET_EHOME_VOICE_TALK_IN, *LPNET_EHOME_VOICE_TALK_IN;

typedef struct tagNET_EHOME_VOICE_TALK_OUT
{
    LONG  lSessionID;
    LONG  lHandle;
    BYTE  byRes[124];
}NET_EHOME_VOICE_TALK_OUT, *LPNET_EHOME_VOICE_TALK_OUT;

typedef struct tagNET_EHOME_PUSHVOICE_IN
{
    DWORD dwSize;
    LONG  lSessionID; 
    BYTE  byToken[64];
    BYTE  byRes[64];
}NET_EHOME_PUSHVOICE_IN, *LPNET_EHOME_PUSHVOICE_IN;

typedef struct tagNET_EHOME_PUSHVOICE_OUT
{
    DWORD dwSize;
    LONG  lHandle;
    BYTE  byRes[124];
}NET_EHOME_PUSHVOICE_OUT, *LPNET_EHOME_PUSHVOICE_OUT;

typedef struct tagNET_EHOME_STOPVOICETALK_STM_PARAM
{
    LONG lSessionID;
    LONG lHandle;
    BYTE  byRes[120];
}NET_EHOME_STOPVOICETALK_STM_PARAM, *LPNET_EHOME_STOPVOICETALK_STM_PARAM;

NET_DVR_API LONG CALLBACK  NET_ECMS_StartVoiceTalk(LONG lUserID, DWORD dwVoiceChan,
                                                   const NET_EHOME_VOICETALK_PARA* pVoiceTalkPara);

NET_DVR_API BOOL CALLBACK  NET_ECMS_StartVoiceWithStmServer(LONG lUserID, NET_EHOME_VOICE_TALK_IN* lpVoiceTalkIn, NET_EHOME_VOICE_TALK_OUT* lpVoiceTalkOut);
NET_DVR_API BOOL CALLBACK  NET_ECMS_StartPushVoiceStream(LONG lUserID, LPNET_EHOME_PUSHVOICE_IN lpPushParamIn, LPNET_EHOME_PUSHVOICE_OUT lpPushParamOut);
NET_DVR_API BOOL CALLBACK  NET_ECMS_StopVoiceTalk(LONG iVoiceHandle);
NET_DVR_API BOOL CALLBACK  NET_ECMS_StopVoiceTalkWithStmServer(LONG lUserID, LONG lSessionID);
NET_DVR_API BOOL CALLBACK  NET_ECMS_StopVoiceTalkWithStmServerEx(LONG lUserID, LPNET_EHOME_STOPVOICETALK_STM_PARAM pStopParam);
NET_DVR_API BOOL CALLBACK  NET_ECMS_SendVoiceTransData(LONG iVoiceHandle, const char *pSendBuf, DWORD dwBufSize);

//---------------------------------------------------------------------------------------------------------

typedef    struct    tagNET_EHOME_CONFIG
{
    void*        pCondBuf;      //[in]，条件数据指针，如表示通道号等
    DWORD        dwCondSize;    //[in]，pCondBuf指向的数据大小
    void*        pInBuf;        //[in]，设置时需要用到，指向结构体的指针
    DWORD        dwInSize;      //[in], pInBuf指向的数据大小
    void*        pOutBuf;       //[out]，获取时需要用到，指向结构体的指针，内存由上层分配
    DWORD        dwOutSize;     //[in]，获取时需要用到，表示pOutBuf指向的内存大小， 
    BYTE         byRes[40];     //保留
}NET_EHOME_CONFIG, *LPNET_EHOME_CONFIG;

typedef struct tagNET_EHOME_XML_CFG 
{
    void*      pCmdBuf;    //字符串格式命令
    DWORD      dwCmdLen;   //pCmdBuf长度
    void*      pInBuf;     //输入数据
    DWORD      dwInSize;   //输入数据长度
    void*      pOutBuf;    //输出缓冲
    DWORD      dwOutSize;  //输出缓冲区长度
    DWORD      dwSendTimeOut;  //数据发送超时时间,单位ms，默认5s
    DWORD      dwRecvTimeOut;  //数据接收超时时间,单位ms，默认5s
    void*      pStatusBuf;     //返回的状态参数(XML格式),如果不需要,可以置NULL
    DWORD      dwStatusSize;   //状态缓冲区大小(内存大小)
    BYTE       byRes[24];
}NET_EHOME_XML_CFG, *LPNET_EHOME_XML_CFG;

#define    MAX_SERIALNO_LEN        128    //序列号最大长度
#define    MAX_PHOMENUM_LEN        32    //手机号码最大长度
#define    MAX_DEVICE_NAME_LEN     32//设备名称长度

typedef struct tagNET_DVR_DVR_TYPE_NAME
{
    DWORD dwDVRType;
    char byDevName[24];  
}NET_DVR_TYPE_NAME;

static const NET_DVR_TYPE_NAME DVRTypeName[] = 
{
    {0,"UNKNOWN TYPE"},         /*对尚未定义的dvr类型返回NETRET_DVR*/
    {1,"DVR"},                    /*DVR*/    
    {2,"ATMDVR"},                /*atm dvr*/
    {3,"DVS"},                /*DVS*/
    {4,"DEC"},                /* 6001D */
    {5,"ENC_DEC"},                /* 6001F */
    {6,"DVR_HC"},                /*8000HC*/
    {7,"DVR_HT"},                /*8000HT*/
    {8,"DVR_HF"},                /*8000HF*/
    {9,"DVR_HS"},                /* 8000HS DVR(no audio) */
    {10,"DVR_HTS"},              /* 8016HTS DVR(no audio) */
    {11,"DVR_HB"},              /* HB DVR(SATA HD) */
    {12,"DVR_HCS"},              /* 8000HCS DVR */
    {13,"DVS_A"},              /* 带ATA硬盘的DVS */
    {14,"DVR_HC_S"},              /* 8000HC-S */
    {15,"DVR_HT_S"},              /* 8000HT-S */
    {16,"DVR_HF_S"},              /* 8000HF-S */
    {17,"DVR_HS_S"},              /* 8000HS-S */
    {18,"ATMDVR_S"},              /* ATM-S */
    {19,"DVR_7000H"},                /*7000H系列*/
    {20,"DEC_MAT"},              /*多路解码器*/
    {21,"DVR_MOBILE"},                /* mobile DVR */                 
    {22,"DVR_HD_S"},              /* 8000HD-S */
    {23,"DVR_HD_SL"},                /* 8000HD-SL */
    {24,"DVR_HC_SL"},                /* 8000HC-SL */
    {25,"DVR_HS_ST"},                /* 8000HS_ST */
    {26,"DVS_HW"},              /* 6000HW */
    {27,"DS630X_D"},             /* 多路解码器 */
    {28,"DS640X_HD"},                /*640X高清解码器*/
    {29,"DS610X_D"},              /*610X解码器*/
    {30,"IPCAM"},                /*IP 摄像机*/
    {31,"MEGA_IPCAM"},                /*高清IP摄像机 852F&852MF*/
    {32,"IPCAM_862MF"},                /*862MF可以接入9000设备*/
    {35,"ITCCAM"},                /*智能高清网络摄像机*/
    {36,"IVS_IPCAM"},          /*智能分析高清网络摄像机*/
    {38,"ZOOMCAM"},            /*一体机*/
    {40,"IPDOME"},              /*IP 标清球机*/
    {41,"IPDOME_MEGA200"},              /*IP 200万高清球机*/
    {42,"IPDOME_MEGA130"},              /*IP 130万高清球机*/
    {43,"IPDOME_AI"},              /*IP 高清智能快球*/ 
    {44,"TII_IPCAM"},          /*红外热成像摄像机*/
    {50,"IPMOD"},                /*IP 模块*/
    {59,"DS64XXHD_T"},                //64-T高清解码器
    {60,"IDS6501_HF_P"},    // 6501 车牌
    {61,"IDS6101_HF_A"},              //智能ATM
    {62,"IDS6002_HF_B"},          //双机跟踪：DS6002-HF/B
    {63,"IDS6101_HF_B"},          //行为分析：DS6101-HF/B
    {64,"IDS52XX"},          //智能分析仪IVMS
    {65,"IDS90XX"},                // 9000智能
    {67,"IDS8104_AHL_S_H"},              // 海康人脸识别 ATM
    {68,"IDS91XX"},                // 9100智能
    {69,"IIP_CAM_B"},  //智能行为IP摄像机
    {70,"IIP_CAM_F"},  //智能人脸IP摄像机
    {71,"DS71XX_H"},                /* DS71XXH_S */
    {72,"DS72XX_H_S"},                /* DS72XXH_S */
    {73,"DS73XX_H_S"},                /* DS73XXH_S */
    {74,"DS72XX_HF_S"},              //DS72XX_HF_S
    {75,"DS73XX_HFI_S"},              //DS73XX_HFI_S
    {75,"DS73XX_HF_S"},              //DS73XX_HF_S
    {76,"DS76XX_H_S"},                /* DVR,e.g. DS7604_HI_S */
    {77,"DS76XX_N_S"},                /* NVR,e.g. DS7604_NI_S */
    {81,"DS81XX_HS_S"},                /* DS81XX_HS_S */
    {82,"DS81XX_HL_S"},                /* DS81XX_HL_S */
    {83,"DS81XX_HC_S"},                /* DS81XX_HC_S */
    {84,"DS81XX_HD_S"},                /* DS81XX_HD_S */
    {85,"DS81XX_HE_S"},                /* DS81XX_HE_S */
    {86,"DS81XX_HF_S"},                /* DS81XX_HF_S */
    {87,"DS81XX_AH_S"},                /* DS81XX_AH_S */
    {88,"DS81XX_AHF_S"},                /* DS81XX_AHF_S */
    {90,"DS90XX_HF_S"},              /*DS90XX_HF_S*/
    {91,"DS91XX_HF_S"},              /*DS91XX_HF_S*/
    {92,"DS91XX_HD_S"},              /*91XXHD-S(MD)*/
    {93,"IDS90XX_A"},                // 9000智能 ATM
    {94,"IDS91XX_A"},                // 9100智能 ATM
    {95,"DS95XX_N_S"},              /*DS95XX_N_S NVR 不带任何输出*/
    {96,"DS96XX_N_SH"},              /*DS96XX_N_SH NVR*/
    {97,"DS90XX_HF_SH"},              /*DS90XX_HF_SH */   
    {98,"DS91XX_HF_SH"},              /*DS91XX_HF_SH */
    {100,"DS_B10_XY"},             /*视频综合平台设备型号(X:编码板片数，Y:解码板片数)*/
    {101,"DS_6504HF_B10"},             /*视频综合平台内部编码器*/
    {102,"DS_6504D_B10"},             /*视频综合平台内部解码器*/
    {103,"DS_1832_B10"},             /*视频综合平台内部码分器*/
    {104,"DS_6401HFH_B10"},             /*视频综合平台内部光纤板*/
    {105,"DS_65XXHC"},                //65XXHC DVS
    {106,"DS_65XXHC_S"},                //65XXHC-SATA DVS
    {107,"DS_65XXHF"},                //65XXHF DVS
    {108,"DS_65XXHF_S"},                //65XXHF-SATA DVS
    {109,"DS_6500HF_B"},             //65 rack DVS
    {110,"IVMS_6200_C"},            // iVMS-6200(/C)  
    {111,"IVMS_6200_B"},             // iVMS-6200(/B)
    {112,"DS_72XXHV_ST"},                //72XXHV_ST15 DVR
    {113,"DS_72XXHV_ST"},                //72XXHV_ST20 DVR,这两个设备显示一个名字
    {114,"IVMS_6200_T"},             // IVMS-6200(/T)
    {115,"IVMS_6200_BP"},             // IVMS-6200(/BP)
    {116,"DS_81XXHC_ST"},                //DS_81XXHC_ST
    {117,"DS_81XXHS_ST"},                //DS_81XXHS_ST
    {118,"DS_81XXAH_ST"},                //DS_81XXAH_ST
    {119,"DS_81XXAHF_ST"},                //DS_81XXAHF_ST
    {120,"DS_66XXDVS"},                //66XX DVS
    {121,"DS_1964_B10"},             /*视频综合平台内部报警器*/
    {122,"DS_B10N04_IN"},             /*视频综合平台内部级联输入*/
    {123,"DS_B10N04_OUT"},             /*视频综合平台内部级联输出*/
    {124,"DS_B10N04_INTEL"},             /*视频综合平台内部智能*/
    {125,"DS_6408HFH_B10E_RM"},             //V6高清
    {126,"DS_B10N64F1_RTM"},             //V6级联不带DSP
    {127,"DS_B10N64F1D_RTM"},             //V6级联带DSP
    {128,"DS_B10_SDS"},             //视频综合平台子域控制器
    {129,"DS_B10_DS"},             //视频综合平台域控制器
    {130,"DS_6401HFH_B10V"},             //VGA高清编码器
    {131,"DS_6504D_B10B"},             /*视频综合平台内部标清解码器*/
    {132,"DS_6504D_B10H"},             /*视频综合平台内部高清解码器*/
    {133,"DS_6504D_B10V"},             /*视频综合平台内部VGA解码器*/
    {134,"DS_6408HFH_B10S"},             //视频综合平台SDI子板 
    {135,"DS_18XX_N"},             /* 矩阵接入网关*/ 
    //注：121～140预留给视频综合平台系列产品使用,141~160留给码分设备和报警设备类用
    {141,"DS_18XX_PTZ"},                /*网络码分类产品*/
    {142,"DS_19AXX"},                /*通用报警主机类产品*/
    {143,"DS_19BXX"},                /*家用报警主机*/
    {144,"DS_19CXX"},                /*自助银行报警主机*/
    {145,"DS_19DXX"},                /*动环监控报警主机*/    
    {146,"DS_19XX"},             /*1900系列报警主机*/ 
    {147,"DS_19SXX"},                /*视频报警主机*/
    {148, "DS_1HXX"},            /*CS类产品*/ //防护舱
    /**********************设备类型 end***********************/
    //2011-11-03 161~170分配给大屏设备使用
    {161,"DS_C10H"},           /*多屏控制器*/
    {162,"DS_C10N_BI"},            //BNC处理器
    {163,"DS_C10N_DI"},            //rbg处理器
    {164,"DS_C10N_SI"},            //码流处理器
    {165,"DS_C10N_DO"},            //显示处理器
    {166,"DS_C10N_SERVER"},        //分布式服务器
    // 171- 180  划给智能使用
    {171,"IDS_8104_AHFL_S_H"},             // 8104ATM 
    {172,"IDS_65XX_HF_A"},             // 65 ATM
    {173,"IDS90XX_HF_RH"},             // 9000 智能RH
    {174,"IDS91XX_HF_RH"},             // 9100 智能RH设备
    {175,"IDS_65XX_HF_B"},             // 65 行为分析
    {176,"IDS_65XX_HF_P"},             // 65 车牌识别
    {177,"IVMS_6200_F"},     // IVMS-6200(/F)
    {178,"IVMS_6200_A"},             //iVMS-6200(/A)
    {179,"IVMS_6200_F_S"},   //iVMS-6200(/F_S)
    {181,"DS90XX_HF_RH"},             // 9000 RH
    {182,"DS91XX_HF_RH"},             // 9100 RH设备
    {183,"DS78XX_S"},             // 78系列设备
    {185,"DS81XXHW_S"},                // 81 Resolution 960 
    {186,"DS81XXHW_ST"},             // DS81XXHW_ST
    {187,"DS91XXHW_ST"},             // DS91XXHW_ST
    {188,"DS91XX_ST"},             // DS91XX_ST
    {189,"DS81XX_ST"},             // DS81XX_ST
    {190,"DS81XXH_ST"},             // DS81XXHDI_ST,DS81XXHE_ST ky2012
    {191,"DS73XXH_ST"},             // DS73XXHI_ST ky2012
    {192,"DS81XX_SH"},             // 审讯81SH,81SHF
    {193,"DS81XX_SN"},             // 审讯81SNL
    {194,"DS96XXN_ST"},             //NVR:DS96xxN_ST
    {195,"DS86XXN_ST"},             //NVR:DS86xxN_ST
    {196,"DS80XXHF_ST"},             //DVR:DS80xxHF_ST
    {197,"DS90XXHF_ST"},             //DVR:DS90xxHF_ST
    {198,"DS76XXN_ST"},             //NVR:DS76xxN_ST
    {199,"DS_9664N_RX"},         //嵌入式NVR(64路IP通道，无模拟通道)，最后一个X：T/H    
    {200,"ENCODER_SERVER"},            // 编码卡服务器
    {201,"DECODER_SERVER"},         // 解码卡服务器
    {202,"PCNVR_SERVER"},         // PCNVR存储服务器
    {203,"CVR_SERVER"},         // 邦诺CVR，他给自己定的类型为DVR_S-1
    {204,"DS_91XXHFH_ST"},         // 91系列HD-SDI高清DVR
    {205,"DS_66XXHFH"},         // 66高清编码器
    {210,"TRAFFIC_TS_SERVER"},         //终端服务器
    {211,"TRAFFIC_VAR"},         //视频分析记录仪
    {212,"IPCALL"},              //IP可视对讲分机

    //2012-03-21 类型值255以后的设备不要添加至此数组中，该数组的作用是为了使扩展协议能获取类型值255以下设备的名称
    //{2001, "DS90XXHW_ST"}        // DS90XXHW_ST混合DVR
};


/**********************设备大类 begin**********************/

/* dvr相关 1-50 */
#define DEV_CLASS_DVR               1   //普通dvr类型
#define DEV_CLASS_INTERROGATION     2   //审讯机
#define DEV_CLASS_SIMPLE_TRAIL      3   //简易庭审主机
#define DEV_CLASS_TRAIL             4   //标准庭审主机
#define DEV_CLASS_RECORD_PLAY       5   //录播主机
#define DEV_CLASS_ATM               6   //ATM机

/* dvs相关 51-100 */
#define DEV_CLASS_DVS               51  //普通dvs

/* nvr相关 101-150 */
#define DEV_CLASS_NVR               101 //普通nvr

/* ipc相关 151-200 */
#define DEV_CLASS_GUN               151 //ipc枪机
#define DEV_CLASS_BALL              152 //ipc球机
#define DEV_CLASS_SNAP              153 //抓拍机
#define DEV_CLASS_INTELLI_TILT      154 //智能云台
#define DEV_CLASS_FISH_EYE          155 //鱼眼
#define DEV_CLASS_2DP_Z             156 //大鹰眼
#define DEV_CLASS_2DP               157 //小鹰眼
#define DEV_CLASS_PT                158 //全景细节相机
#define DEV_CLASS_TRI               159 //超高频固定式读卡器

/* CVR相关 201 - 250*/
#define DEV_CLASS_CVR               201 //CVR

/* 传显相关 251 - 300*/
#define DEV_CLASS_B20                   251 //传显B20系列
#define DEV_CLASS_B10                   252 //传显B10系列
#define DEV_CLASS_DECODER               253 //解码器
#define DEV_CLASS_MATRIXMANAGEDEVICE    254 //矩阵接入网关
#define DEV_CLASS_OTICAL                255 //光端机
#define DEV_CLASS_CODESPITTER           256 //码分器
#define DEV_CLASS_ALARMHOST             257 //行业报警主机
#define DEV_CLASS_MOVING_RING           258 //动环设备
#define DEV_CLASS_CVCS                  259 //集中式多屏控制器
#define DEV_CLASS_DVCS                  260 //分布式多屏控制器
#define DEV_CLASS_TRANSCODER            261 //转码器
#define DEV_CLASS_LCD_SCREEN            262 //LCD屏幕
#define DEV_CLASS_LED_SCREEN            263 //LED屏幕
#define DEV_CLASS_MATRIX                264 //矩阵
#define DEV_CLASS_CONFERENCE_SYSTEM     265 //视频会议设备
#define DEV_CLASS_INFORMATION_RELEASE_EQUIPMENT 266 //信息发布设备
#define DEV_CLASS_NET_GAP               267 //网闸
#define DEV_CLASS_MERGE                 268 //合码器
#define DEV_CLASS_REAR_PROJECTION       269 //背投显示设备
#define DEV_CLASS_SWITCH                270 //交换机
#define DEV_CLASS_FIBER_CONVERTER       271 //光纤收发器
#define DEV_CLASS_SCREEN_SERVER         272 //屏幕服务器
#define DEV_CLASS_SCE_SERVER            273 //抓屏服务器
#define DEV_CLASS_WIRELESS_TRANS        274 //无线传输设备
#define DEV_CLASS_Y10_SERIES            275 //Y10系列
#define DEV_CLASS_SAFETY_MAVHINE        276 //安监一体机
#define DEV_CLASS_IOTGATEWAY            277 //物联网关类
/* 报警主机相关 301 - 350*/
#define DEV_CLASS_VIDEO_ALARM_HOST      301 //视频报警主机
#define DEV_CLASS_NET_ALARM_HOST        302 //网络报警主机
#define DEV_CLASS_ONE_KEY_ALARM         303 //一键式报警产品
#define DEV_CLASS_WIRELESS_ALARM_HOST   304 //无线报警主机
#define DEV_CLASS_ALARM_MODULE          305 //报警模块
#define DEV_CLASS_HOME_ALARM_HOST       306 //家用报警主机

/* 门禁相关 351 - 400*/
#define DEV_CLASS_ACCESS_CONTROL        351 //门禁产品

/* 可视对讲 401 - 450*/
#define DEV_CLASS_VIDEO_INTERCOM        401 //可视对讲

/* 无人机 451 - 500*/
#define DEV_CLASS_UMMANNED_AERIAL_VEHICLE   451 //无人机产品

/* 移动产品: 501-550*/
#define DEV_CLASS_MOBILE                501 //移动产品

/* 移动车载设备: 551-600*/
#define DEV_CLASS_MOBILE_VEHICLE        551 //移动车载设备

//智能分析仪：601-650
#define DEV_CLASS_INTELLIGENT_ANALYZER  601 //智能分析仪

//智能交通服务器：651-700
#define DEV_CLASS_INTELLIGENT_TRAFFIC_SERVER    651 //智能交通服务器
#define DS_TP2200_EC                            652 //经济型机柜监测仪

/* nvs相关 701-750 */
#define DEV_CLASS_NVS           701 //普通nvs

/*有源RFID系列 751-800*/
#define DS_TRI21A_1_P           751 //有源RFID读取器

/* 数据中心设备801-850 */
#define DS_CLASS_FA             801 //脸谱单机
#define DS_CLASS_PURE           802 //脸谱纯分析
#define DS_CLASS_FS             803 //人脸静态数据服务器
#define DS_CLASS_FD             804 //抓拍检测服务器
#define DS_CLASS_HAWK           805 //猎鹰服务器
#define DS_CLASS_BLADE          806 //刀锋
#define DS_CLASS_HMCP           807 //模型对比服务器

/* 智能锁相关 851 - 900*/
#define DEV_CLASS_SMART_LOCK    851 //智能锁盒子

/* 雷达相关 901 - 950*/
#define DEV_CLASS_RADAR         901 //雷达产品

/* 智慧消防相关 951 - 1000*/
#define DEV_CLASS_FIRE_CONTROL   951 //消防产品

/*全景细节相机： 8451-8470*/
#define iDS_PT                  8451    //全景细节相机

/* 其他设备类型 65534 */
#define DEV_CLASS_DEFAULT       65534   //默认设备类型
/**********************设备大类 end**********************/

typedef struct tagNET_EHOME_DEVICE_INFO
{
    DWORD   dwSize;             //结构体大小
    DWORD   dwChannelNumber;    //模拟视频通道个数
    DWORD   dwChannelAmount;    //总视频通道数（模拟+IP）
    DWORD   dwDevType;          //设备类型，1-DVR，3-DVS，30-IPC，40-IPDOME，其他值参考海康NetSdk设备类型号定义值
    DWORD   dwDiskNumber;       //设备当前硬盘数
    BYTE    sSerialNumber[MAX_SERIALNO_LEN];    //设备序列号
    DWORD   dwAlarmInPortNum;   //模拟通道报警输入个数
    DWORD   dwAlarmInAmount;    //设备总报警输入个数
    DWORD   dwAlarmOutPortNum;  //模拟通道报警输出个数
    DWORD   dwAlarmOutAmount;   //设备总报警输出个数
    DWORD   dwStartChannel;     //视频起始通道号
    DWORD   dwAudioChanNum;     //语音对讲通道个数（设备通道+IP通道（byStartDTalkChan））
    DWORD   dwMaxDigitChannelNum;   //设备支持的最大数字通道路数
    DWORD   dwAudioEncType;     //语音对讲音频格式，0-G722，1-G711U，2-G711A，3-G726，4-AAC，5-MP2L2,6-PCM,7-MP3
    BYTE    sSIMCardSN[MAX_SERIALNO_LEN];       //车载设备扩展：SIM卡序列号
    BYTE    sSIMCardPhoneNum[MAX_PHOMENUM_LEN]; //车载扩展：SIM卡手机号码
    DWORD   dwSupportZeroChan;  // SupportZeroChan:支持零通道的个数：0-不支持，1-支持1路，2-支持2路，以此类推
    DWORD   dwStartZeroChan;    //零通道起始编号，默认10000
    DWORD   dwSmartType;        //智能类型，0-smart，1-专业智能；默认0-smart
    WORD    wDevClass;          //设备大类
    BYTE     byStartDTalkChan;    //起始数字对讲通道号，区别于模拟对讲通道号，0表示无数字对讲通道
    BYTE    byRes[157];         //保留
}NET_EHOME_DEVICE_INFO,*LPNET_EHOME_DEVICE_INFO;

#define MAX_VERSION_LEN     32   //版本描述长度

typedef struct tagNET_EHOME_VERSION_INFO
{
    DWORD   dwSize; //结构体大小
    BYTE    sSoftwareVersion[MAX_VERSION_LEN];      //主控版本
    BYTE    sDSPSoftwareVersion[MAX_VERSION_LEN];   //编码版本
    BYTE    sPanelVersion[MAX_VERSION_LEN];         //面板版本
    BYTE    sHardwareVersion[MAX_VERSION_LEN];      //硬件版本
    BYTE    byRes[124];
}NET_EHOME_VERSION_INFO,*LPNET_EHOME_VERSION_INFO;

typedef struct tagNET_EHOME_DEVICE_CFG
{
    DWORD        dwSize;            //结构体大小
    BYTE         sServerName[MAX_DEVICE_NAME_LEN];  //设备名称
    DWORD        dwServerID;        //设备号（遥控器编号:0~255）
    DWORD        dwRecycleRecord;   //是否循环录像，0代表不循环，1代表循环
    DWORD        dwServerType;      //设备类型，同DevType，由于公司设备型号有几百个，本协议中只使用通用型号：1-DVR，3-DVS，30-IPC，40-IPDOME
    DWORD        dwChannelNum;      //通道个数，包括模拟+IP通道（只读）
    DWORD        dwHardDiskNum;     //硬盘个数（只读）
    DWORD        dwAlarmInNum;      //报警输入个数（模拟通道）（只读）
    DWORD        dwAlarmOutNum;     //报警输出个数（模拟通道）（只读）
    DWORD        dwRS232Num;        //232串口个数（只读）
    DWORD        dwRS485Num;        //485串口个数（只读）
    DWORD        dwNetworkPortNum;  //网络口个数（只读）
    DWORD        dwAuxoutNum;       //辅口个数（只读）
    DWORD        dwAudioNum;        //语音口个数（只读）
    BYTE         sSerialNumber[MAX_SERIALNO_LEN];   //设备序列号（只读）
    DWORD        dwMajorScale;      //主口缩放：0代表不缩放；1代表缩放
    DWORD        dwMinorScale;      //辅口缩放：0代表不缩放；1代表缩放
    BYTE         byRes[292];        //保留
}NET_EHOME_DEVICE_CFG,*LPNET_EHOME_DEVICE_CFG;

#define NET_EHOME_GET_DEVICE_INFO       1   //获取设备信息
#define NET_EHOME_GET_VERSION_INFO      2   //获取版本信息
#define NET_EHOME_GET_DEVICE_CFG        3   //获取设备基本参数
#define NET_EHOME_SET_DEVICE_CFG        4   //设置设备基本参数

#define NET_EHOME_GET_GPS_CFG           20  //获取GPS参数
#define NET_EHOME_SET_GPS_CFG           21  //设置GPS参数
#define NET_EHOME_GET_PIC_CFG           22  //获取OSD显示参数
#define NET_EHOME_SET_PIC_CFG           23  //设置OSD显示参数
#define NET_EHOME_GET_WIRELESSINFO_CFG  24  //获取无线网络信息参数
#define NET_EHOME_SET_WIRELESSINFO_CFG  25  //设置无线网络信息参数

#define MAX_EHOME_PROTOCOL_LEN          1500

typedef struct tagNET_EHOME_REMOTE_CTRL_PARAM
{
    DWORD   dwSize;
    void*   lpCondBuffer;        //条件参数缓冲区
    DWORD   dwCondBufferSize;    //条件参数缓冲区长度
    void*   lpInbuffer;          //控制参数缓冲区
    DWORD   dwInBufferSize;      //控制参数缓冲区长度
    BYTE    byRes[32];
}NET_EHOME_REMOTE_CTRL_PARAM, *LPNET_EHOME_REMOTE_CTRL_PARAM;

//GPS参数
typedef struct tagNET_EHOME_GPS_CFG
{
    DWORD        dwSize;
    DWORD        dwTransInterval;   //GPS上传时间，单位：秒
    DWORD        dwMaxSpeed;        //超速限速值，单位：厘米/小时
    DWORD        dwMinSpeed;        //低速限速值，单位：厘米/小时
    BYTE         bEnable;           //使能,1-不启用，2-启用
    BYTE         byRes[63];         //保留
}NET_EHOME_GPS_CFG, *LPNET_EHOME_GPS_CFG;

//OSD显示参数
typedef struct tagNET_EHOME_PIC_CFG
{
    DWORD           dwSize;
    BYTE            byChannelName[NAME_LEN];    //通道名称（GB2312编码， V30以后版本使用UTF-8编码），长度不超过31字节
    BOOL            bIsShowChanName;            //是否显示通道名称：0：不显示；1：显示
    WORD            wChanNameXPos;              //通道名称显示X坐标，按照704x576来配置
    WORD            wChanNameYPos;              //通道名称显示Y坐标，按照704x576来配置，坐标值为16的倍数
    BOOL            bIsShowOSD;                 //是否显示OSD：0：不显示；1：显示
    WORD            wOSDXPos;                   //OSD显示X坐标
    WORD            wOSDYPos;                   //OSD显示Y坐标
    BYTE            byOSDType;                  /**OSD类型 0:XXXX-XX-XX(年月日) 1:XX-XX-XXXX(月日年) 2:XXXX年XX月XX日
                                                    3:XX月XX日XXXX年 4:XX-XX-XXXX(日月年) 5:XX日XX月XXXX年
                                                */
    BYTE            byOSDAtrib;                 //OSD属性 0：不显示OSD 1：透明，闪烁 2：透明，不闪烁 3：闪烁，不透明 4：不透明，不闪烁
    BYTE            byRes1[2];                  //保留1
    BOOL            bIsShowWeek;                //是否显示星期：0：不显示；1：显示
    BYTE            byRes2[64];                 //保留2
}NET_EHOME_PIC_CFG, *LPNET_EHOME_PIC_CFG;

typedef struct tagNET_EHOME_WIRELESS_INFO_CFG
{
    DWORD        dwSize;
    DWORD        dwInfoTransInterval;   //上传间隔,单位：秒
    BYTE         byEnable;              //使能，0-不启用 1-启用
    BYTE         byRes[47];
}NET_EHOME_WIRELESS_INFO_CFG, *LPNET_EHOME_WIRELESS_INFO_CFG;

#define STREAM_KEY_LEN  32

typedef struct tagNET_EHOME_STREAM_PASSWORD
{
    BYTE byEnable;                  // 加密操作，0- 不启用，1- 启用, 2- 修改密码
    BYTE byRes1[3];
    BYTE byNewKey[STREAM_KEY_LEN];  // 密码
    BYTE byOldKey[STREAM_KEY_LEN];  //原密码
    BYTE byRes[12];
}NET_EHOME_STREAM_PASSWORD, *LPNET_EHOME_STREAM_PASSWORD;

NET_DVR_API BOOL CALLBACK NET_ECMS_GetDevConfig(LONG lUserID, DWORD dwCommand, LPNET_EHOME_CONFIG lpConfig, DWORD dwConfigSize);

NET_DVR_API BOOL CALLBACK NET_ECMS_SetDevConfig(LONG lUserID, DWORD dwCommand, LPNET_EHOME_CONFIG lpConfig, DWORD dwConfigSize);

NET_DVR_API BOOL CALLBACK NET_ECMS_XMLConfig(LONG lUserID, LPNET_EHOME_XML_CFG pXmlCfg, DWORD dwConfigSize);

NET_DVR_API BOOL CALLBACK NET_ECMS_RemoteControl(LONG lUserID, DWORD dwCommand, LPNET_EHOME_REMOTE_CTRL_PARAM lpCtrlParam);

NET_DVR_API BOOL CALLBACK NET_ECMS_CheckStreamEncrypt(LONG lUserID, const char* pStreamEncrypt);

//设置设备SessionKey信息
NET_DVR_API BOOL CALLBACK NET_ECMS_SetDeviceSessionKey(NET_EHOME_DEV_SESSIONKEY* pDeviceKey);
//获取设备EhomeKey信息
NET_DVR_API BOOL CALLBACK NET_ECMS_GetDeviceSessionKey(NET_EHOME_DEV_SESSIONKEY* pDeviceKey);
//重置5.0设备注册状态
NET_DVR_API BOOL CALLBACK NET_ECMS_ResetDevRegisterState(const char* sDeviceID);

//预览请求
typedef struct tagNET_EHOME_PREVIEWINFO_IN
{
    int     iChannel;     //通道号 
    DWORD   dwStreamType; // 码流类型，0-主码流，1-子码流, 2-第三码流,3- 语音监听
    DWORD   dwLinkMode;   // 0：TCP方式,1：UDP方式,2: HRUDP方式  
    NET_EHOME_IPADDRESS struStreamSever;    //流媒体地址
}NET_EHOME_PREVIEWINFO_IN, *LPNET_EHOME_PREVIEWINFO_IN;

typedef struct tagNET_EHOME_PREVIEWINFO_IN_V11
{
    int     iChannel;
    DWORD   dwStreamType; // 码流类型，0-主码流，1-子码流, 2-第三码流,3- 语音监听
    DWORD   dwLinkMode;
    NET_EHOME_IPADDRESS struStreamSever;
    BYTE    byDelayPreview;
    BYTE    byEncrypt;    //0-不加密，1-加密
    BYTE    byRes[30];
}NET_EHOME_PREVIEWINFO_IN_V11, *LPNET_EHOME_PREVIEWINFO_IN_V11;

typedef struct tagNET_EHOME_MAKE_I_FRAME
{
    int     iChannel;       // 通道号
    DWORD   dwStreamType;   // 码流类型，0- 主码流，1- 子码流, 2- 预留 
    BYTE    byRes[40];
}NET_EHOME_MAKE_I_FRAME, *LPNET_EHOME_MAKE_I_FRAME;


typedef struct tagNET_EHOME_PREVIEWINFO_OUT
{
    LONG  lSessionID;
    LONG  lHandle;      //设置了取流异步回调之后，该值为消息句柄，回调中用于标识
    BYTE  byRes[124];
}NET_EHOME_PREVIEWINFO_OUT, *LPNET_EHOME_PREVIEWINFO_OUT;

typedef struct tagNET_EHOME_PUSHSTREAM_IN
{
    DWORD dwSize;
    LONG  lSessionID; 
    BYTE  byRes[128];
}NET_EHOME_PUSHSTREAM_IN, *LPNET_EHOME_PUSHSTREAM_IN;

typedef struct tagNET_EHOME_PUSHSTREAM_INFO_OUT
{
    DWORD dwSize;
    LONG  lHandle;  //设置了取流异步回调之后，该值为消息句柄，回调中用于标识
    BYTE  byRes[124];
}NET_EHOME_PUSHSTREAM_OUT, *LPNET_EHOME_PUSHSTREAM_OUT;

typedef struct tagNET_EHOME_STOPSTREAM_PARAM
{
    LONG  lSessionID;
    LONG  lHandle;
    BYTE  byRes[120];
}NET_EHOME_STOPSTREAM_PARAM, *LPNET_EHOME_STOPSTREAM_PARAM;

NET_DVR_API BOOL CALLBACK NET_ECMS_StartGetRealStream(LONG lUserID, LPNET_EHOME_PREVIEWINFO_IN pPreviewInfoIn, LPNET_EHOME_PREVIEWINFO_OUT pPreviewInfoOut ); //lUserID由SDK分配的用户ID，由设备注册回调时fDeviceRegisterCallBack返回
NET_DVR_API BOOL CALLBACK NET_ECMS_StartGetRealStreamV11(LONG lUserID, LPNET_EHOME_PREVIEWINFO_IN_V11 pPreviewInfoIn, LPNET_EHOME_PREVIEWINFO_OUT pPreviewInfoOut);
NET_DVR_API BOOL CALLBACK NET_ECMS_StopGetRealStream(LONG lUserID, LONG lSessionID);
NET_DVR_API BOOL CALLBACK NET_ECMS_StopGetRealStreamEx(LONG lUserID, LPNET_EHOME_STOPSTREAM_PARAM pStopParam);

NET_DVR_API BOOL CALLBACK NET_ECMS_StartPushRealStream(LONG lUserID, LPNET_EHOME_PUSHSTREAM_IN pPushInfoIn, LPNET_EHOME_PUSHSTREAM_OUT pPushInfoOut);
NET_DVR_API BOOL CALLBACK NET_ECMS_MakeIFrame(LONG lUserID, LPNET_EHOME_MAKE_I_FRAME pIFrameParma);
NET_DVR_API BOOL CALLBACK NET_ECMS_SetStreamEncrypt(LONG lUserID, LPNET_EHOME_STREAM_PASSWORD pStreamPassword);
NET_DVR_API BOOL CALLBACK NET_ECMS_WakeUp(LONG lUserID);

//查询接口
#define MAX_FILE_NAME_LEN  100
#define LEN_32             32

typedef enum tagSEARCH_TYPE
{
    ENUM_SEARCH_TYPE_ERR        = -1,
    ENUM_SEARCH_RECORD_FILE     = 0,    //查找录像文件
    ENUM_SEARCH_PICTURE_FILE    = 1,    //查找图片文件
    ENUM_SEARCH_FLOW_INFO       = 2,    //流量查询
    ENUM_SEARCH_DEV_LOG         = 3,    //设备日志查询
    ENUM_SEARCH_ALARM_HOST_LOG  = 4,    //报警主机日志查询
}SEARCH_TYPE_ENUM;

typedef enum
{
    ENUM_GET_NEXT_STATUS_SUCCESS    = 1000, //成功读取到一条数据，处理完本次数据后需要再次调用FindNext获取下一条数据
    ENUM_GET_NETX_STATUS_NO_FILE,           //没有找到一条数据
    ENUM_GET_NETX_STATUS_NEED_WAIT,         //数据还未就绪，需等待，继续调用FindNext函数
    ENUM_GET_NEXT_STATUS_FINISH,            //数据全部取完
    ENUM_GET_NEXT_STATUS_FAILED,            //出现异常
    ENUM_GET_NEXT_STATUS_NOT_SUPPORT        //设备不支持该操作，不支持的查询类型
}SEARCH_GET_NEXT_STATUS_ENUM;

//时间参数
typedef struct tagNET_EHOME_TIME
{
    WORD    wYear;      //年
    BYTE    byMonth;    //月
    BYTE    byDay;      //日
    BYTE    byHour;     //时
    BYTE    byMinute;   //分
    BYTE    bySecond;   //秒
    BYTE    byRes1;
    WORD    wMSecond;   //毫秒
    BYTE    byRes2[2];
}NET_EHOME_TIME, *LPNET_EHOME_TIME;

typedef struct tagNET_EHOME_FINDCOND
{
    DWORD               dwSize;
    LONG                iChannel;           //通道号，从1开始
    DWORD               dwMaxFileCountPer;  //单次搜索最大文件个数，最大文件个数，需要确定实际网络环境，建议最大个数为8
    NET_EHOME_TIME      struStartTime;      //开始时间
    NET_EHOME_TIME      struStopTime;       //结束时间
    SEARCH_TYPE_ENUM    enumSearchType;     //查找类型 0-录像文件查找，对应struRecordFileParam  1-图片文件查找，对应struPicFileParam 2-流量查询，对应struFlowParam
    union
    {
        BYTE           byLen[64];
        struct
        {
            DWORD       dwFileType;         /**文件类型（字节整形数）：
                                            0xff-全部类型录像（不包含图片类型） 
                                            0-定时录像
                                            1-移动报警
                                            2-报警触发
                                            3-报警|动测
                                            4-报警&动测
                                            5-命令触发
                                            6-手动录像
                                            7-震动报警
                                            8-环境报警
                                            9-智能报警（或者取证录像）
                                            10（0x0a）-PIR报警
                                            11（0x0b）-无线报警
                                            12（0x0c）-呼救报警
                                            13（0x0d）全部报警
                                            100-全部类型图片
                                            101-车牌识别图片
                                            102-稽查报警图片
                                            103-手动抓拍图片
                                            104-回放抓拍图片
                                            **/

       }struRecordFileParam;
        struct
        {
            DWORD       dwFileType;         /*255（0xff）-全部类型：
                                            0（0x00）-定时抓图
                                            1（0x01）-移动侦测抓图
                                            2（0x02）-报警抓图
                                            3（0x03）-报警|移动侦测抓图
                                            4（0x04）-报警&移动侦测抓图
                                            5（0x05）-命令触发抓图
                                            6（0x06）-手动抓图
                                            7（0x07）-震动报警抓图
                                            8（0x08）-环境报警触发抓图
                                            9（0x09）-智能报警图片
                                            10（0x0a）-PIR报警图片
                                            11（0x0b）-无线报警图片
                                            12（0x0c）-呼救报警图片
                                            13（0x0d）-人脸侦测图片
                                            14（0x0e）-越界侦测图片
                                            15（0x0f）-入侵区域侦测图片
                                            16（0x10）-场景变更侦测图片
                                            17（0x11）-设备本地回放时截图
                                            18（0x12）-智能侦测图片
                                            19（0x13）-进入区域侦测图片
                                            20（0x14）-离开区域侦测图片
                                            21（0x15）-徘徊侦测图片
                                            22（0x16）-人员聚集侦测图片
                                            23（0x17）-快速运动侦测图片
                                            24（0x18）-停车侦测图片
                                            25（0x19）-物品遗留侦测图片
                                            26（0x1a）-物品拿取侦测图片
                                            27（0x1b）-车牌侦测图片
                                            28（0x1c）-客户端上传图片
                                            */
        }struPicFileParam;
        struct
        {
            BYTE        bySearchMode;       //查询模式，1-按year查询，2-按month查询，3-按day查询
        }struFlowParam;
    }unionSearchParam;
	BYTE                byStartIndex;       //查询起始位置，兼容老版本，新版本中不建议使用
	BYTE                byRes1[3];
	DWORD               dwStartIndexEx;     //扩展后的查询起始位置， 增加对记录条数过大的支持
	BYTE                byRes[124];
}NET_EHOME_FINDCOND, *LPNET_EHOME_FINDCOND;

//录像文件查找条件
typedef struct tagNET_EHOME_REC_FILE_COND
{
    DWORD           dwChannel;          //通道号，从1开始
    DWORD           dwRecType;          /*0xff-全部类型录像（不包含图片类型）
                                        0-定时录像
                                        1-移动报警
                                        2-报警触发
                                        3-报警|动测
                                        4-报警&动测
                                        5-命令触发
                                        6-手动录像
                                        7-震动报警
                                        8-环境报警
                                        9-智能报警（或者取证录像）
                                        10（0x0a）-PIR报警
                                        11（0x0b）-无线报警
                                        12（0x0c）-呼救报警
                                        13（0x0d）全部报警
                                        100-全部类型图片
                                        101-车牌识别图片
                                        102-稽查报警图片
                                        103-手动抓拍图片
                                        104-回放抓拍图片
                                        */
    NET_EHOME_TIME  struStartTime;      //开始时间
    NET_EHOME_TIME  struStopTime;       //结束时间
    DWORD           dwStartIndex;       //查询起始位置
    DWORD           dwMaxFileCountPer;  //单次搜索最大文件个数，最大文件个数，需要确定实际网络环境，建议最大个数为8
    BYTE            byLocalOrUTC;       //0-struStartTime和struStopTime中，表示的是设备本地时间，即设备OSD时间  1-struStartTime和struStopTime中，表示的是UTC时间
    BYTE            byRes[63];
}NET_EHOME_REC_FILE_COND, *LPNET_EHOME_REC_FILE_COND;

//图片文件查找条件
typedef struct tagNET_EHOME_PIC_FILE_COND
{
    DWORD           dwChannel;          //通道号，从1开始
    DWORD           dwPicType;          /*255（0xff）-全部类型
                                        0（0x00）-定时抓图
                                        1（0x01）-移动侦测抓图
                                        2（0x02）-报警抓图
                                        3（0x03）-报警|移动侦测抓图
                                        4（0x04）-报警&移动侦测抓图
                                        5（0x05）-命令触发抓图
                                        6（0x06）-手动抓图
                                        7（0x07）-震动报警抓图
                                        8（0x08）-环境报警触发抓图
                                        9（0x09）-智能报警图片
                                        10（0x0a）-PIR报警图片
                                        11（0x0b）-无线报警图片
                                        12（0x0c）-呼救报警图片
                                        13（0x0d）-人脸侦测图片
                                        14（0x0e）-越界侦测图片
                                        15（0x0f）-入侵区域侦测图片
                                        16（0x10）-场景变更侦测图片
                                        17（0x11）-设备本地回放时截图
                                        18（0x12）-智能侦测图片
                                        19（0x13）-进入区域侦测图片
                                        20（0x14）-离开区域侦测图片
                                        21（0x15）-徘徊侦测图片
                                        22（0x16）-人员聚集侦测图片
                                        23（0x17）-快速运动侦测图片
                                        24（0x18）-停车侦测图片
                                        25（0x19）-物品遗留侦测图片
                                        26（0x1a）-物品拿取侦测图片
                                        27（0x1b）-车牌侦测图片
                                        28（0x1c）-客户端上传图片
                                        */
    NET_EHOME_TIME  struStartTime;      //开始时间
    NET_EHOME_TIME  struStopTime;       //结束时间
    DWORD           dwStartIndex;       //查询起始位置
    DWORD           dwMaxFileCountPer;  //单次搜索最大文件个数，最大文件个数，需要确定实际网络环境，建议最大个数为8
    BYTE            byLocalOrUTC;       //0-struStartTime和struStopTime中，表示的是设备本地时间，即设备OSD时间  1-struStartTime和struStopTime中，表示的是UTC时间
    BYTE            byRes[63];
}NET_EHOME_PIC_FILE_COND, *LPNET_EHOME_PIC_FILE_COND;

//流量查询条件
typedef struct tagNET_EHOME_FLOW_COND
{
    BYTE            bySearchMode;       //查询模式，1-按year查询，2-按month查询，3-按day查询
    BYTE            byRes[3];
    NET_EHOME_TIME  struStartTime;      //开始时间
    NET_EHOME_TIME  struStopTime;       //结束时间
    DWORD           dwStartIndex;       //查询起始位置
    DWORD           dwMaxFileCountPer;  //单次搜索最大文件个数，最大文件个数，需要确定实际网络环境，建议最大个数为8
    BYTE            byLocalOrUTC;       //0-struStartTime和struStopTime中，表示的是设备本地时间，即设备OSD时间  1-struStartTime和struStopTime中，表示的是UTC时间
    BYTE            byRes1[63];
}NET_EHOME_FLOW_COND, *LPNET_EHOME_FLOW_COND;

//设备日志查询条件
typedef struct tagNET_EHOME_DEV_LOG_COND
{
    DWORD           dwMajorType;        //日志主类型，1-报警，2-异常，3-操作，0xffff-全部
    DWORD           dwMinorType;        //日志次类型，与设备基线代码定义相同
    NET_EHOME_TIME  struStartTime;      //开始时间
    NET_EHOME_TIME  struStopTime;       //结束时间
    DWORD           dwStartIndex;       //查询起始位置
    DWORD           dwMaxFileCountPer;  //单次搜索最大文件个数，最大文件个数，需要确定实际网络环境，建议最大个数为8
    BYTE            byLocalOrUTC;       //0-struStartTime和struStopTime中，表示的是设备本地时间，即设备OSD时间  1-struStartTime和struStopTime中，表示的是UTC时间
    BYTE            byRes[63];
}NET_EHOME_DEV_LOG_COND, *LPNET_EHOME_DEV_LOG_COND;

//报警主机日志查询条件
typedef struct tagNET_EHOME_ALARM_HOST_LOG_COND
{
    DWORD           dwMajorType;        //日志主类型，1-报警，2-异常，3-操作，4-事件，0xffff-全部
    DWORD           dwMinorType;        //日志次类型，与设备基线代码定义相同
    NET_EHOME_TIME  struStartTime;      //开始时间
    NET_EHOME_TIME  struStopTime;       //结束时间
    DWORD           dwStartIndex;       //查询起始位置
    DWORD           dwMaxFileCountPer;  //单次搜索最大文件个数，最大文件个数，需要确定实际网络环境，建议最大个数为8
    BYTE            byLocalOrUTC;       //0-struStartTime和struStopTime中，表示的是设备本地时间，即设备OSD时间  1-struStartTime和struStopTime中，表示的是UTC时间
    BYTE            byRes1[63];
}NET_EHOME_ALARM_HOST_LOG_COND, *LPNET_EHOME_ALARM_HOST_LOG_COND;

//老版的录像文件信息
typedef struct tagNET_EHOME_FINDDATA
{
    DWORD           dwSize;
    char            szFileName[MAX_FILE_NAME_LEN];  //文件名
    NET_EHOME_TIME  struStartTime;                  //文件的开始时间
    NET_EHOME_TIME  struStopTime;                   //文件的结束时间
    DWORD           dwFileSize;                     //文件的大小
    DWORD           dwFileMainType;                 //录像文件主类型
    DWORD           dwFileSubType;                  //录像文件子类型
    DWORD           dwFileIndex;                    //录像文件索引
    BYTE            byRes[128];
}NET_EHOME_FINDDATA, *LPNET_EHOME_FINDDATA;

//录像文件信息
typedef struct tagNET_EHOME_REC_FILE
{
    DWORD           dwSize;
    char            sFileName[MAX_FILE_NAME_LEN];   //文件名
    NET_EHOME_TIME  struStartTime;                  //文件的开始时间
    NET_EHOME_TIME  struStopTime;                   //文件的结束时间
    DWORD           dwFileSize;                     //文件的大小
    DWORD           dwFileMainType;                 //录像文件主类型
    DWORD           dwFileSubType;                  //录像文件子类型
    DWORD           dwFileIndex;                    //录像文件索引
    BYTE            byTimeDiffH;                    //struStartTime、struStopTime与国际标准时间（UTC）的时差（小时），-12 ... +14,0xff表示无效
    BYTE            byTimeDiffM;                    //struStartTime、struStopTime与国际标准时间（UTC）的时差（分钟），-30,0, 30, 45, 0xff表示无效
    BYTE            byRes[126];
}NET_EHOME_REC_FILE, *LPNET_EHOME_REC_FILE;

//图片文件信息
typedef struct tagNET_EHOME_PIC_FILE
{
    DWORD           dwSize;
    char            sFileName[MAX_FILE_NAME_LEN];   //文件名
    NET_EHOME_TIME  struPicTime;                    //图片生成时间
    DWORD           dwFileSize;                     //文件的大小
    DWORD           dwFileMainType;                 //图片文件主类型
    DWORD           dwFileIndex;                    //图片文件索引
    BYTE            byTimeDiffH;                    //struPicTime与国际标准时间（UTC）的时差（小时），-12 ... +14,0xff表示无效
    BYTE            byTimeDiffM;                    //struPicTime与国际标准时间（UTC）的时差（分钟），-30,0, 30, 45, 0xff表示无效
    BYTE            byRes[126];
}NET_EHOME_PIC_FILE, *LPNET_EHOME_PIC_FILE;

//流量信息
typedef struct tagNET_EHOME_FLOW_INFO
{
    DWORD   dwSize;
    DWORD   dwFlowValue;    //流量值
    DWORD   dwFlowIndex;    //流量索引
    BYTE    byRes[128];
}NET_EHOME_FLOW_INFO, *LPNET_EHOME_FLOW_INFO;

//设备日志信息
#define MAX_LOG_INFO_LEN    8*1024   //日志附加信息长度

typedef struct tagNET_EHOME_DEV_LOG
{
    NET_EHOME_TIME  struLogTime;                    //日志时间
    DWORD           dwMajorType;                    //日志主类型，1-报警，2-异常，3-操作
    DWORD           dwMinorType;                    //日志次类型，与设备基线代码定义相同
    DWORD           dwParamType;                    //次类型参数类型，大部分表示防区号，如果没有则值为0
    char            sLocalUser[NAME_LEN];           //本地用户
    char            sRemoteUser[NAME_LEN];          //远程用户
    char            sIPAddress[128];                //远端主机IP地址
    DWORD           dwChannelNo;                    //通道号
    DWORD           dwHardDiskNo;                   //硬盘号
    DWORD           dwAlarmInputChanNo;             //报警输入通道号
    DWORD           dwAlarmOutputChanNo;            //报警输出通道号
    char            sLogContext[MAX_LOG_INFO_LEN];  //日志内容
    BYTE            byTimeDiffH;                    //struLogTime与国际标准时间（UTC）的时差（小时），-12 ... +14,0xff表示无效
    BYTE            byTimeDiffM;                    //struLogTime与国际标准时间（UTC）的时差（分钟），-30,0, 30, 45, 0xff表示无效
    BYTE            byRes[62];
}NET_EHOME_DEV_LOG, *LPNET_EHOME_DEV_LOG;

//报警主机日志信息
typedef struct tagNET_EHOME_ALARM_HOST_LOG
{
    NET_EHOME_TIME  struLogTime;                    //日志时间
    DWORD           dwMajorType;                    //日志主类型，1-报警，2-异常，3-操作，4-事件
    DWORD           dwMinorType;                    //日志次类型，与设备基线代码定义相同
    DWORD           dwParamType;                    //次类型参数类型，大部分表示防区号，如果没有则值为0
    char            sUserName[NAME_LEN];            //用户名
    char            sIPAddress[128];                //远端主机IP地址
    char            sLogContext[MAX_LOG_INFO_LEN];  //日志内容
    BYTE            byTimeDiffH;                    //struLogTime与国际标准时间（UTC）的时差（小时），-12 ... +14,0xff表示无效
    BYTE            byTimeDiffM;                    //struLogTime与国际标准时间（UTC）的时差（分钟），-30,0, 30, 45, 0xff表示无效
    BYTE            byRes[62];
}NET_EHOME_ALARM_HOST_LOG, *LPNET_EHOME_ALARM_HOST_LOG;

NET_DVR_API LONG CALLBACK NET_ECMS_StartFindFile(LONG lUserID, LPNET_EHOME_FINDCOND pFindCond); //仅支持录像文件、图片文件和流量查询
NET_DVR_API LONG CALLBACK NET_ECMS_FindNextFile(LONG lHandle, LPNET_EHOME_FINDDATA pFindData); //仅支持录像文件查询

NET_DVR_API LONG CALLBACK NET_ECMS_StartFindFile_V11(LONG lUserID, LONG lSearchType, LPVOID pFindCond, DWORD dwCondSize);
NET_DVR_API LONG CALLBACK NET_ECMS_FindNextFile_V11(LONG lHandle, LPVOID pFindData, DWORD dwDataSize);
NET_DVR_API BOOL CALLBACK NET_ECMS_StopFindFile(LONG lHandle);

typedef struct tagNET_EHOME_PLAYBACK_INFO_IN
{
    DWORD       dwSize;
    DWORD       dwChannel;                    //回放的通道号
    BYTE        byPlayBackMode;               //回放下载模式 0－按名字 1－按时间
    BYTE        byStreamPackage;              //回放码流类型，设备端发出的码流格式 0－PS（默认） 1－RTP
    BYTE        byRes[2];
    union
    {
        BYTE    byLen[512];
        struct
        {
            char   szFileName[MAX_FILE_NAME_LEN];   //回放的文件名
            DWORD  dwSeekType;                      //0-按字节长度计算偏移量  1-按时间（秒数）计算偏移量
            DWORD  dwFileOffset;                    //文件偏移量，从哪个位置开始下载，如果dwSeekType为0，偏移则以字节计算，为1则以秒数计算
            DWORD  dwFileSpan;                      //下载的文件大小，为0时，表示下载直到该文件结束为止，如果dwSeekType为0，大小则以字节计算，为1则以秒数计算
        }struPlayBackbyName;
        struct 
        {
            NET_EHOME_TIME  struStartTime;  // 按时间回放的开始时间
            NET_EHOME_TIME  struStopTime;   // 按时间回放的结束时间
            BYTE    byLocalOrUTC;           //0-设备本地时间，即设备OSD时间  1-UTC时间
            BYTE    byDuplicateSegment;     //byLocalOrUTC为1时无效 0-重复时间段的前段 1-重复时间段后端
        }struPlayBackbyTime;
    }unionPlayBackMode;
    NET_EHOME_IPADDRESS struStreamSever;    //流媒体地址
}NET_EHOME_PLAYBACK_INFO_IN, *LPNET_EHOME_PLAYBACK_INFO_IN;

typedef struct tagNET_EHOME_PLAYBACK_INFO_OUT
{
    LONG   lSessionID;     //目前协议不支持，返回-1
    LONG   lHandle;  //设置了回放异步回调之后，该值为消息句柄，回调中用于标识
    BYTE   byRes[124];
}NET_EHOME_PLAYBACK_INFO_OUT, *LPNET_EHOME_PLAYBACK_INFO_OUT;

typedef struct tagNET_EHOME_PUSHPLAYBACK_IN
{
    DWORD dwSize;
    LONG lSessionID; 
    BYTE byKeyMD5[32];//码流加密秘钥,两次MD5
    BYTE  byRes[96];
} NET_EHOME_PUSHPLAYBACK_IN, *LPNET_EHOME_PUSHPLAYBACK_IN;

typedef struct tagNET_EHOME_PUSHPLAYBACK_OUT
{
    DWORD dwSize;
    LONG  lHandle;
    BYTE  byRes[124];
} NET_EHOME_PUSHPLAYBACK_OUT, *LPNET_EHOME_PUSHPLAYBACK_OUT;

typedef struct tagNET_EHOME_STOPPLAYBACK_PARAM
{
    LONG  lSessionID;
    LONG  lHandle;
    BYTE  byRes[120];
}NET_EHOME_STOPPLAYBACK_PARAM, *LPNET_EHOME_STOPPLAYBACK_PARAM;

typedef struct tagNET_EHOME_PLAYBACK_PAUSE_RESTART_PARAM
{
    LONG lSessionID;
    LONG lHandle;  //（输出参数）设置了回放异步回调之后，该值为消息句柄，回调中用于标识（新增）
    BYTE byRes[120];
}NET_EHOME_PLAYBACK_PAUSE_RESTART_PARAM, *LPNET_EHOME_PLAYBACK_PAUSE_RESTART_PARAM;

typedef enum tagENUM_NET_EHOME_PLAYBACK_OPERATE_MODE
{
    PLAYBACK_OPERATE_UNKNOW     = -1,   //无效操作
    PLAYBACK_OPERATE_PAUSE      = 0,    //暂停回放,对应pOperateParam结构体NET_EHOME_PLAYBACK_PAUSE_RESTART_PARAM
    PLAYBACK_OPERATE_RESTART    = 1     //恢复回放，对应pOperateParam结构体NET_EHOME_PLAYBACK_PAUSE_RESTART_PARAM
}ENUM_NET_EHOME_PLAYBACK_OPERATE_MODE;

NET_DVR_API BOOL CALLBACK NET_ECMS_StartPlayBack(LONG lUserID, LPNET_EHOME_PLAYBACK_INFO_IN pPlayBackInfoIn, LPNET_EHOME_PLAYBACK_INFO_OUT pPlayBackInfoOut);
NET_DVR_API BOOL CALLBACK NET_ECMS_StopPlayBack(LONG lUserID, LONG lSessionID);
NET_DVR_API BOOL CALLBACK NET_ECMS_StopPlayBackEx(LONG iUserID, LPNET_EHOME_STOPPLAYBACK_PARAM pStopParam);
NET_DVR_API BOOL CALLBACK NET_ECMS_StartPushPlayBack(LONG lUserID, LPNET_EHOME_PUSHPLAYBACK_IN pPushInfoIn, LPNET_EHOME_PUSHPLAYBACK_OUT pPushInfoOut);
NET_DVR_API BOOL CALLBACK NET_ECMS_PlayBackOperate(LONG iUserID, ENUM_NET_EHOME_PLAYBACK_OPERATE_MODE enumMode, void* pOperateParam);

//云台相关结构体
#define NET_EHOME_PTZ_CTRL      1000    //云镜控制
#define NET_EHOME_PRESET_CTRL   1001    //预置点操作
#define NET_EHOME_PZIN          1002    //3D框选放大缩小
#define NET_EHOME_PTRACK        1003    //3D点击居中

typedef enum
{
    PTZ_UP              = 0,    //向上
    PTZ_DOWN,                   //向下
    PTZ_LEFT,                   //向左
    PTZ_RIGHT,                  //向右
    PTZ_UPLEFT,                 //向左上
    PTZ_DOWNLEFT,               //向左下
    PTZ_UPRIGHT,                //向右上
    PTZ_DOWNRIGHT,              //向右下
    PTZ_ZOOMIN,                 //变焦－
    PTZ_ZOOMOUT,                //变焦＋
    PTZ_FOCUSNEAR,              //聚焦－
    PTZ_FOCUSFAR,               //聚焦＋
    PTZ_IRISSTARTUP,            //光圈变大
    PTZ_IRISSTOPDOWN,           //光圈变小
    PTZ_LIGHT,                  //补光灯
    PTZ_WIPER,                  //雨刷
    PTZ_AUTO                    //自动
}EN_PTZ_CMD;

typedef struct tagNET_EHOME_PTZ_PARAM
{
    DWORD dwSize;
    BYTE  byPTZCmd; //云台命令，参见EN_PTZ_CMD
    BYTE  byAction; //云台动作，0-开始云台动作，1-停止云台动作
    BYTE  bySpeed;  //云台速度，0-7，数值越大速度越快
    BYTE  byRes[29];
}NET_EHOME_PTZ_PARAM, *LPNET_EHOME_PTZ_PARAM;

typedef struct tagNET_EHOME_PRESET_PARAM
{
    DWORD dwSize;
    BYTE  byPresetCmd;      //预置点控制命令，1-设置预置点，2-清除预置点，3-转到预置点
    BYTE  byRes1[3];
    DWORD dwPresetIndex;    //预置点编号
    BYTE  byRes2[32];
}NET_EHOME_PRESET_PARAM, *LPNET_EHOME_PRESET_PARAM;

typedef struct tagNET_EHOME_PZIN_PARAM
{
    DWORD           dwSize;
    BYTE            byAction;   //动作，0缩小（右上->左下，右下->左上） 1放大（左上->右下，左下->右上）
    BYTE            byRes1[3];
    NET_EHOME_ZONE  struArea;   //框选范围
    BYTE            byRes2[32];
}NET_EHOME_PZIN_PARAM, *LPNET_EHOME_PZIN_PARAM;

typedef struct tagNET_EHOME_POINT
{
    DWORD dwX;
    DWORD dwY;
    BYTE  byRes[4];
}NET_EHOME_POINT, *LPNET_EHOME_POINT;

typedef struct tagNET_EHOME_IPADDR
{
    char    sIpV4[16];
    char    sIpV6[128];
}NET_EHOME_IPADDR, *LPNET_EHOME_IPADDR;

#define MACADDR_LEN     6   //mac地址长度

typedef struct tagNET_EHOME_ETHERNET
{
    NET_EHOME_IPADDR    struDevIP;      //设备IP地址
    NET_EHOME_IPADDR    struDevIPMask;  //掩码地址
    DWORD               dwNetInterface; // 网卡类型1:10M半双工 2:10M全双工 3:100M半双工 4:100M全双工 6:1000M全双工 5:10M/100M/1000M自适应
    WORD                wDevPort;       //设备SDK端口号：8000
    WORD                wMTU;           // MTU参数
    BYTE                byMACAddr[MACADDR_LEN]; //mac地址
    BYTE                byRes[2];
}NET_EHOME_ETHERNET,*LPNET_EHOME_ETHERNET;

#define PASSWD_LEN      16  //密码长度

typedef struct tagNET_EHOME_PPPOECFG
{
    DWORD   dwPPPoE;                    //是否启用PPPOE：1代表使用；0代表不使用
    char    sPPPoEUser[NAME_LEN];       //PPPoE用户名
    char    sPPPoEPassword[PASSWD_LEN]; //PPPoE密码
    NET_EHOME_IPADDR    struPPPoEIP;    //PPPoE IP地址
}NET_EHOME_PPPOECFG,*LPNET_EHOME_PPPOECFG;

typedef struct tagNET_EHOME_NETWORK_CFG
{
    DWORD               dwSize;             //结构体大小
    NET_EHOME_ETHERNET  struEtherNet;       //以太网口
    NET_EHOME_IPADDR    struGateWayIP;      //网关地址
    NET_EHOME_IPADDR    struMultiCastIP;    //多播地址
    NET_EHOME_IPADDR    struDDNSServer1IP;  //DDNS1服务器IP
    NET_EHOME_IPADDR    struDDNSServer2IP;  //DDNS2服务器IP
    NET_EHOME_IPADDR    struAlarmHostIP;    //报警主机IP
    WORD                wAlarmHostPort;     //报警主机端口号
    WORD                wIPResolverPort;    //解析服务器端口
    NET_EHOME_IPADDR    struIPResolver;     //解析服务器地址
    NET_EHOME_PPPOECFG  struPPPoE;          //PPPoE参数
    WORD                wHTTPPort;;         //Http端口
    BYTE                byRes[674];         //保留
}NET_EHOME_NETWORK_CFG,*LPNET_EHOME_NETWORK_CFG;

typedef struct tagNET_EHOME_COMPRESSION_COND
{
    DWORD   dwSize;             //结构体大小
    DWORD   dwChannelNum;       //通道号，从1开始
    BYTE    byCompressionType;  //码流类型，1-主码流，2-子码流，3-三码流
    BYTE    byRes[23];
}NET_EHOME_COMPRESSION_COND,*LPNET_EHOME_COMPRESSION_COND;

typedef struct tagNET_EHOME_COMPRESSION_CFG
{
    DWORD       dwSize;         //结构体大小
    BYTE        byStreamType;   //码流类型
    BYTE        byPicQuality;   //图像质量0:最高 1:较高 2:中等 3:低 4:较低 5:最低
    BYTE        byBitRateType;  //码率类型0:变码率 1:定码率
    BYTE        byRes1;         //保留
    DWORD       dwResolution;   //分辨率，0:DCIF 1:CIF 2:QCIF 3:4CIF 4:2CIF 6:QVGA（320x240） 16:VGA 17:UXGA 18:SVGA 19:HD720p 20:hd900 21:XVGA    22:SXGAp(1360*1024)
    //27:1080P(1920*1080)    28:2560x1920 /*500W*/    29:1600x304    30:2048x1536 /*300W*/
    //31:2448x2048/*500W*/        32:2448x1200        33:2448x800    34:XGA/*(1024*768)*/
    //35:SXGA/*(1280*1024)*/    36:WD1/*(960*576/960*480)*/    37:HD1080I    38-WXGA(1440*900)，
    //39-HD_F(1920*1080/1280*720)，40-HD_H(1920*540/1280*360)，  41-HD_Q(960*540/630*360)，
    //42-2336*1744，    43-1920*1456，44-2592*2048，    45-3296*2472，46-1376*768，47-1366*768,
    //48-1360*768,  49-WSXGA+，50-720*720，51-1280*1280，52-2048*768，53-2048*2048
    //54-2560x2048,  55-3072x2048 ,  56-2304*1296  57-WXGA(1280*800),  58-1600*600
    //59-2592*1944  60-2752*2208,    61-384*288,    62-4000*3000,  63-4096*2160,  64-3840*2160,
    //65-4000*2250, 66-3072*1728,
    DWORD       dwVideoBitRate;     //0-32K 1-48k 2-64K 3-80K 4-96K 5-128K 6-160k 7-192K 8-224K 9-256K 10-320K 11-384K 12-448K 13-512K 14-640K 15-768K 16-896K 17-1024K 18-1280K 19-1536K 20-1792K 21-2048K 22-自定义
    DWORD       dwMaxBitRate;       //自定义码率
    DWORD       dwVideoFrameRate;   //视频帧率（4字节整数）0:全帧率 1:1/16 2:1/8 3:1/4 4:1/2 5:1 6:2 7:4 8:6 9:8 10:10 11:12 12:16 13:20 14:15 15:18 16:22
    WORD        wIntervalFrameI;    // I帧间隔，范围1~400
    BYTE        byIntervalBPFrame;  //帧类型 0:BBP 1：BP 2:P
    BYTE        byRes[41];          //保留
}NET_EHOME_COMPRESSION_CFG,*LPNET_EHOME_COMPRESSION_CFG;

#define MAX_TIME_SEGMENT        8   //时间段,ehome协议只支持4个
#define MAX_ANALOG_ALARMOUT     32  //最大32路模拟报警输出
#define MAX_ANALOG_CHANNUM      32  //最大32个模拟通道
#define MAX_DIGIT_CHANNUM       480 //最大480个数字通道,与网络库保持

typedef struct tagNET_EHOME_ALARM_TIME_COND
{
    DWORD       dwSize;         //结构体大小
    BYTE        byAlarmType;    //报警类型，0-移动侦测，1-视频丢失，2-遮挡报警，3-报警输入，4-报警输出，9-客流量
    BYTE        byWeekday;      //0-Mon，1-Tues，2-Wed，3-Thur，4-Fri，5-Sat，6-Sun
    BYTE        byRes1[2];
    DWORD       dwChannel;      //通道号，从1开始
    BYTE        byRes2[20];
}NET_EHOME_ALARM_TIME_COND,*LPNET_EHOME_ALARM_TIME_COND;

typedef struct tagNET_EHOME_SCHEDTIME
{
    BYTE        byStartHour;    //开始时间：时
    BYTE        byStartMin;     //开始时间：分
    BYTE        byStopHour;     //结束时间：时
    BYTE        byStopMin;      //结束时间：分
}NET_EHOME_SCHEDTIME,*LPNET_EHOME_SCHEDTIME;

typedef struct tagNET_EHOME_ALARM_TIME_CFG
{
    DWORD       dwSize;             //结构体大小
    NET_EHOME_SCHEDTIME struSchedTime[MAX_TIME_SEGMENT];//布防时间段
    BYTE        bySchedTimeCount;   //只读，布防时间段数
    BYTE        byRes[43];
}NET_EHOME_ALARM_TIME_CFG,*LPNET_EHOME_ALARM_TIME_CFG;

typedef struct tagNET_EHOME_ALARMOUT_CFG
{
    DWORD       dwSize;                 //结构体大小
    BYTE        sAlarmOutName[NAME_LEN];//报警输出名称
    WORD        wAlarmOutDelay;         //输出延时：0：5秒；1：10秒； 2：30秒； 3：1分； 4：2分； 5：5分； 6：10分； 7：最大；
    BYTE        byRes[26];              //保留
}NET_EHOME_ALARMOUT_CFG,*LPNET_EHOME_ALARMOUT_CFG;

typedef struct tagNET_EHOME_ALARMOUT_STATUS_CFG
{
    DWORD       dwSize;             //结构体大小
    BYTE        byAlarmOutStatus;   //1-开启报警输出，0-关闭报警输出
    BYTE        byRes[11];
}NET_EHOME_ALARMOUT_STATUS_CFG,*LPNET_EHOME_ALARMOUT_STATUS_CFG;

typedef struct tagNET_EHOME_ALARMIN_COND
{
    DWORD       dwSize;         //结构体大小
    DWORD       dwAlarmInNum;   //报警编号，从1开始
    DWORD       dwPTZChan;      // PTZ联动视频通道编号,从1开始
    BYTE        byRes[20];
}NET_EHOME_ALARMIN_COND,*LPNET_EHOME_ALARMIN_COND;

typedef struct tagNET_EHOME_LINKAGE_ALARMOUT
{
    DWORD       dwAnalogAlarmOutNum;                    //只读，模拟报警数量
    BYTE        byAnalogAlarmOut[MAX_ANALOG_ALARMOUT];  //模拟报警输出，0：不使用；1：使用
    BYTE        byRes[5000];                            //保留，协议里面没有IP报警输出联动，接口上位置留出来
}NET_EHOME_LINKAGE_ALARMOUT,*LPNET_EHOME_LINKAGE_ALARMOUT;

typedef struct tagNET_EHOME_LINKAGE_PTZ
{
    BYTE    byUsePreset;    //是否调用预置点，0：不使用；1：使用
    BYTE    byUseCurise;    //是否调用巡航，0：不使用；1：使用
    BYTE    byUseTrack;     //是否调用轨迹，0：不使用；1：使用
    BYTE    byRes1;         //保留
    WORD    wPresetNo;      //预置点号，范围：1~256，协议中规定是1～256，实际已有设备支持300
    WORD    wCuriseNo;      //巡航路径号，范围：1~16
    WORD    wTrackNo;       //轨迹号，范围：1~16
    BYTE    byRes2[6];      //保留
}NET_EHOME_LINKAGE_PTZ,*LPNET_EHOME_LINKAGE_PTZ;

typedef struct tagNET_EHOME_ALARMIN_LINKAGE_TYPE
{
    BYTE    byMonitorAlarm;     //监视器上警告，0：不使用；1：使用
    BYTE    bySoundAlarm;       //声音报警，0：不使用；1：使用
    BYTE    byUpload;           //上传中心，0：不使用；1：使用
    BYTE    byAlarmout;         //触发报警输出，0：不使用；1：使用-
    BYTE    byEmail;            //邮件联动，0：不使用；1：使用
    BYTE    byRes1[3];          //保留
    NET_EHOME_LINKAGE_PTZ    struPTZLinkage;    //PTZ联动
    NET_EHOME_LINKAGE_ALARMOUT    struAlarmOut;    //报警输出联动
    BYTE    byRes[128];
}NET_EHOME_ALARMIN_LINKAGE_TYPE,*LPNET_EHOME_ALARMIN_LINKAGE_TYPE;

typedef struct tagNET_EHOME_RECORD_CHAN
{
    BYTE    byAnalogChanNum;                    //只读，模拟通道数
    BYTE    byAnalogChan[MAX_ANALOG_CHANNUM];   //模拟通道，0：不使用；1：使用
    BYTE    byRes1[3];                          //保留
    WORD    wDigitChanNum;                      //只读，数字通道数
    BYTE    byDigitChan[MAX_DIGIT_CHANNUM];     //数字通道，0：不使用；1：使用
    BYTE    byRes2[62];                         //保留
}NET_EHOME_RECORD_CHAN,*LPNET_EHOME_RECORD_CHAN;

typedef struct tagNET_EHOME_ALARMIN_CFG
{
    DWORD       dwSize;                     //结构体大小
    BYTE        sAlarmInName[NAME_LEN];     //报警输入名称
    BYTE        byAlarmInType;              //报警器类型：0：常开；1：常闭
    BYTE        byUseAlarmIn;               //是否处理，0：不使用；1：使用
    BYTE        byRes1[2];                  //保留
    NET_EHOME_ALARMIN_LINKAGE_TYPE    struLinkageType;    //联动模式
    NET_EHOME_RECORD_CHAN    struRecordChan;//关联录像通道
    BYTE        byRes2[128];                //保留
}NET_EHOME_ALARMIN_CFG,*LPNET_EHOME_ALARMIN_CFG;

typedef struct tagNET_EHOME_MANUAL_IOOUT_CTRL
{
    DWORD       dwSize;        //结构体大小
    DWORD       dwChan;        //IO输出编号，从1开始
    DWORD       dwDelayTime;   //报警输出持续时间（秒），值为0表示一直输出，否则按指定时间长度进行输出。
    BYTE        byAction;      //0-关闭报警输出,1-开启报警输出
    BYTE        byRes[19];
}NET_EHOME_MANUAL_IOOUT_CTRL,*LPNET_EHOME_MANUAL_IOOUT_CTRL;

typedef struct tagNET_EHOME_IMAGE_CFG
{
    DWORD       dwSize;         //结构体大小
    BYTE        byHue;          //色调，0～255
    BYTE        byContrast;     //对比度,0~255
    BYTE        byBright;       //亮度，0～255
    BYTE        bySaturation;   //饱和度
    BYTE        byRes[24];
}NET_EHOME_IMAGE_CFG,*LPNET_EHOME_IMAGE_CFG;

typedef struct tagBINARY_DATA_INFO
{
    DWORD dwMagic;
    DWORD dwCommand;  //命令码
    DWORD dwSequence; //序号
    DWORD dwStatus;   //状态值
    DWORD dwBufLen;
    BYTE  byMsgType;  //报文类型，0-无效，1-请求报文，2-应答报文
    char  sSerialNumber[12];//设备序列号，尚未使用
    //char  sDeviceID[16];  //设备ID
    BYTE  byVersion;
    BYTE  byRes[2];
} BINARY_DATA_INFO, *LPBINARY_DATA_INFO;

#define NET_EHOME_GET_NETWORK_CFG           5   //获取网络参数
#define NET_EHOME_SET_NETWORK_CFG           6   //设置网络参数
#define NET_EHOME_GET_COMPRESSION_CFG       7   //获取压缩参数
#define NET_EHOME_SET_COMPRESSION_CFG       8   //设置压缩参数
#define NET_EHOME_GET_IMAGE_CFG             9   //获取图像参数
#define NET_EHOME_SET_IMAGE_CFG             10  //设置图像参数
#define NET_EHOME_GET_ALARMIN_CFG           11  //获取报警输入参数
#define NET_EHOME_SET_ALARMIN_CFG           12  //设置报警输入参数
#define NET_EHOME_GET_ALARM_TIME_CFG        13  //获取报警布防时间参数
#define NET_EHOME_SET_ALARM_TIME_CFG        14  //设置报警布防时间参数
#define NET_EHOME_GET_ALARMOUT_CFG          15  //获取报警输出参数
#define NET_EHOME_SET_ALARMOUT_CFG          16  //设置报警输出参数
#define NET_EHOME_GET_ALARMOUT_STATUS_CFG   17  //获取报警输出状态参数
#define NET_EHOME_SET_ALARMOUT_STATUS_CFG   18  //设置报警输出状态参数
#define NET_EHOME_MANUAL_IOOUT              19  //手动控制IO输出

typedef enum tagNET_CMS_ENUM_PROXY_TYPE
{
    ENUM_PROXY_TYPE_NETSDK = 0, //NetSDK代理
    ENUM_PROXY_TYPE_HTTP        //HTTP代理
}NET_CMS_ENUM_PROXY_TYPE;

typedef struct tagNET_EHOME_PT_PARAM
{
    NET_EHOME_IPADDRESS struIP; //监听的IP和端口
    BYTE    byProtocolType;     //协议类型，0-TCP
    BYTE    byProxyType;        //代理类型，0-NetSDK代理，1-HTTP代理
    BYTE    byRes[2];
}NET_EHOME_PT_PARAM, *LPNET_EHOME_Proxy_PARAM;

typedef LPNET_EHOME_Proxy_PARAM LPNET_EHOME_PT_PARAM;

typedef struct tagNET_EHOME_PASSTHROUGH_PARAM
{
    DWORD   dwSequence; //报文序号
    DWORD   dwUUID;     //会话ID
    BYTE    byRes[64];
}NET_EHOME_PASSTHROUGH_PARAM, *LPNET_EHOME_PASSTHROUGH_PARAM;

typedef struct tagNET_EHOME_PTXML_PARAM
{
    void*   pRequestUrl;        //请求URL
    DWORD   dwRequestUrlLen;    //请求URL长度
    void*   pCondBuffer;        //条件缓冲区（XML格式数据）
    DWORD   dwCondSize;         //条件缓冲区大小
    void*   pInBuffer;          //输入缓冲区（XML格式数据）
    DWORD   dwInSize;           //输入缓冲区大小
    void*   pOutBuffer;         //输出缓冲区（XML格式数据）
    DWORD   dwOutSize;          //输出缓冲区大小
    DWORD   dwReturnedXMLLen;   //实际从设备接收到的XML数据的长度
    DWORD   dwRecvTimeOut;      //默认5000ms
    DWORD   dwHandle;           //（输出参数）设置了回放异步回调之后，该值为消息句柄，回调中用于标识（新增）
    BYTE    byRes[24];          //保留
}NET_EHOME_PTXML_PARAM, *LPNET_EHOME_PTXML_PARAM;

typedef struct tagNET_EHOME_HTTP_PARAM
{
    void*   pInBuffer;          //输入缓冲区（XML格式数据）
    DWORD   dwInSize;           //输入缓冲区大小
    void*   pOutBuffer;         //输出缓冲区（XML格式数据）
    DWORD   dwOutSize;          //输出缓冲区大小
    DWORD   dwReturnedXMLLen;   //实际从设备接收到的XML数据的长度
    DWORD   dwRecvTimeOut;      //接收超时，单位ms
    BYTE    byRes[28];          //保留
}NET_EHOME_HTTP_PARAM, *LPNET_EHOME_HTTP_PARAM;

NET_DVR_API LONG CALLBACK NET_ECMS_StartListenProxy(LPNET_EHOME_PT_PARAM lpStru);
NET_DVR_API BOOL CALLBACK NET_ECMS_StopListenProxy(LONG lListenHandle, DWORD dwProxyType = 0);
NET_DVR_API LONG CALLBACK NET_ECMS_ConvertProtocolHttpToPassthrough(void* pSrcBuffer, DWORD dwSrcBufLen, void* pDestBuffer, DWORD dwDestBufLen, LPNET_EHOME_PASSTHROUGH_PARAM lpParam, BOOL bToPassthrough = TRUE);
typedef void (CALLBACK* PASSTHROUGHDATACALLBACK)(DWORD dwProxyType, LONG lListenHandle, void* pDeviceID, DWORD dwDevIDLen, void* pDataBuffer, DWORD dwDataLen, void* pUser);
NET_DVR_API BOOL CALLBACK NET_ECMS_SetPassthroughDataCallback(PASSTHROUGHDATACALLBACK fnPassthroughDataCb, void* pUser, DWORD dwProxyType = 0);
NET_DVR_API BOOL CALLBACK NET_ECMS_SendPassthroughData(void* pDataBuffer, DWORD dDataLen, DWORD dwProxyType = 0);
NET_DVR_API BOOL CALLBACK NET_ECMS_GetPTXMLConfig(LONG iUserID, LPNET_EHOME_PTXML_PARAM lpPTXMLParam);
NET_DVR_API BOOL CALLBACK NET_ECMS_PutPTXMLConfig(LONG iUserID, LPNET_EHOME_PTXML_PARAM lpPTXMLParam);
NET_DVR_API BOOL CALLBACK NET_ECMS_PostPTXMLConfig(LONG iUserID, LPNET_EHOME_PTXML_PARAM lpPTXMLParam);
NET_DVR_API BOOL CALLBACK NET_ECMS_DeletePTXMLConfig(LONG lUserID, LPNET_EHOME_PTXML_PARAM lpPTXMLParam);
NET_DVR_API BOOL CALLBACK NET_ECMS_ISAPIPassThrough(LONG lUserID, LPNET_EHOME_PTXML_PARAM lpParam);
NET_DVR_API BOOL CALLBACK NET_ECMS_HTTPConfig(LONG iUserID, LPNET_EHOME_HTTP_PARAM lpParam);
NET_DVR_API BOOL CALLBACK NET_ECMS_SetSDKLocalCfg(NET_EHOME_LOCAL_CFG_TYPE enumType, void* const lpInBuff);
NET_DVR_API BOOL CALLBACK NET_ECMS_GetSDKLocalCfg(NET_EHOME_LOCAL_CFG_TYPE enumType,void* lpOutBuff);

typedef struct tagNET_EHOME_XML_REMOTE_CTRL_PARAM
{
    DWORD dwSize;
    void* lpInbuffer;           //控制参数缓冲区
    DWORD dwInBufferSize;       //控制参数缓冲区长度
    DWORD dwSendTimeOut;        //数据发送超时时间,单位ms，默认5s
    DWORD dwRecvTimeOut;        //数据接收超时时间,单位ms，默认5s
    void* lpOutBuffer;          //输出缓冲区
    DWORD dwOutBufferSize;      //输出缓冲区大小
    void* lpStatusBuffer;       //状态缓冲区,若不需要可置为NULL
    DWORD dwStatusBufferSize;   //状态缓冲区大小
    BYTE  byRes[16];
}NET_EHOME_XML_REMOTE_CTRL_PARAM, *LPNET_EHOME_XML_REMOTE_CTRL_PARAM;
NET_DVR_API BOOL CALLBACK NET_ECMS_XMLRemoteControl(LONG lUserID, LPNET_EHOME_XML_REMOTE_CTRL_PARAM lpCtrlParam, DWORD dwCtrlSize);


#define EHOME_CMSALARM_EXCEPTION      0x105  //CMS接收报警异常

NET_DVR_API BOOL CALLBACK NET_ECMS_SetExceptionCallBack(DWORD dwMessage, HANDLE hWnd, void (CALLBACK* fExceptionCallBack)(DWORD dwType, LONG iUserID, LONG iHandle, void* pUser), void* pUser);

NET_DVR_API BOOL CALLBACK NET_ECMS_TranBuf(LONG lUserID, DWORD dwLength, void* pBuf);

typedef enum
{
    LONG_CFG_CREATED,       //长连接建立成功
    LONG_CFG_CREATE_FAIL,   //长连接建立失败
    LONG_CFG_DATA,          //长连接普通数据
    LONG_CFG_TERMINATE      //长连接销毁消息
}LONG_LINK_MSG;

typedef BOOL(CALLBACK *LongConfigCallBack)(LONG iHandle, LONG_LINK_MSG enMsg, void* pOutBuffer, DWORD dwOutLen, void* pUser);

typedef struct tagNET_EHOME_LONG_CFG_INPUT
{
    LongConfigCallBack fnDataCallBack;
    void*   pUser;
    BYTE    byRes[32];
}NET_EHOME_LONG_CFG_INPUT, *LPNET_EHOME_LONG_CFG_INPUT;

typedef struct tagNET_EHOME_LONG_CFG_SEND
{
    void*   pDataBuffer;
    DWORD   dwDataLen;
    BYTE    byRes[32];
}NET_EHOME_LONG_CFG_SEND, *LPNET_EHOME_LONG_CFG_SEND;

//兼容信息发布私有协议 begin

#define NET_EHOME_S2C_TIME_ADJUST                   0x9             //终端校时
#define NET_EHOME_S2C_SEND_ERRCODE                  0xD             //发送错误码

#define NET_EHOME_S2C_SET_SERVER_INFO               0x18            //设置服务器信息给终端
#define NET_EHOME_S2C_GET_TERMINAL_INFO             0x19            //获取终端信息

#define NET_EHOME_S2C_LOGOUT                        0x20            //通知终端登出服务器

#define NET_EHOME_S2C_POST_SCHEDULE                 0x50            //日程下发
#define NET_EHOME_S2C_TERMINALCONTROL_V20           0x51            //终端设备控制V20
#define NET_EHOME_S2C_GET_CAP_SUPPORTEDCMD          0x52            //获取终端支持的信令的能力集
#define NET_EHOME_S2C_TERMINALCONTROL               0x53            //终端设备控制
#define NET_EHOME_S2C_PLAYCONTROL                   0x54            //终端播放控制
#define NET_EHOME_S2C_SET_IPCINFO                   0x55            //设置IPC信息
#define NET_EHOME_S2C_GET_WORKSTATUS                0x56            //获取设备工作状态
#define NET_EHOME_S2C_VERSIONUPGRADE                0x57            //设备升级
#define NET_EHOME_S2C_SETPLAYERPARAM                0x58            //设置终端播放参数
#define NET_EHOME_S2C_SETSERVERADDR                 0x5B            //设置终端注册的服务器信息
#define NET_EHOME_S2C_VERSIONUPGRADE_V20            0x5C            //设备升级V20

#define NET_EHOME_S2C_SET_WEATHERINFO               0x61            //城市天气信息设置     
#define NET_EHOME_S2C_GET_SCREENSHOT                0x62            //获取终端的屏幕截图
#define NET_EHOME_S2C_GET_DEFAULTPARAM              0x63            //获取恢复默认参数后的值
#define NET_EHOME_S2C_REPLACE_MATERIAL              0x64            //素材替换
#define NET_EHOME_S2C_CANCEL_SCHEDULE               0x65            //取消日程发布
#define NET_EHOME_S2C_SET_QUEUE_INFO                0x66            //叫号管理信息
#define NET_EHOME_S2C_PUBLISH_SHOT_PIC              0x67            //下发弹图图片
#define NET_EHOME_S2C_GET_IPCINFO                   0x68            //获取IPC信息
#define NET_EHOME_S2C_SET_TERMINAL_IP               0x69            //设置终端的ip
#define NET_EHOME_S2C_GET_TERMINAL_IP               0x70            //获取终端的ip
#define NET_EHOME_S2C_SET_TERMINAL_NAME             0x71            //设置终端的名称
#define NET_EHOME_S2C_PUBLISH_BIG_DATA              0x72            //下发大数据，超过8K数据

#define NET_EHOME_S2C_SET_SWITCHPLAN                0x100           //定时开关机计划下发
#define NET_EHOME_S2C_GET_SWITCHPLAN                0x101           //获取定时开关机计划
#define NET_EHOME_S2C_SET_VOLUMEPLAN                0x102           //设置定时音量计划
#define NET_EHOME_S2C_GET_VOLUMEPLAN                0x103           //获取音量控制计划
#define NET_EHOME_S2C_SET_INPUTPLAN                 0x104           //按时间段控制HDMI和信息发布切换
#define NET_EHOME_S2C_GET_INPUTPLAN                 0x105           //获取HDMI和信息发布切换切换计划
#define NET_EHOME_S2C_SET_CHARACTER                 0x106           //往终端插播文字消息
#define NET_EHOME_S2C_GET_CHARACTER                 0x107           //获取终端正在插播的文字消息
#define NET_EHOME_S2C_RETRANSMIT_TO_TERM            0x108           //服务器数据透传到终端
#define NET_EHOME_S2C_TRANS_WITH_RET                0x10A           //服务器数据透传到终端并接收终端透传到服务器的数据
#define NET_EHOME_S2C_SET_ADB_CFG                   0x110           //设置ADB参数    
#define NET_EHOME_S2C_GET_ADB_CFG                   0x111           //获取ADB参数
#define NET_EHOME_S2C_SET_TIEM_ZONE                 0x112           //设置终端的时区信息
#define NET_EHOME_S2C_GET_TIME_ZONE                 0x113           //获取终端的时区信息
#define NET_EHOME_S2C_SET_SADP                      0x114           //设置终端的SADP信息
#define NET_EHOME_S2C_GET_SADP                      0x115           //获取终端的SADP信息
#define NET_EHOME_S2C_GET_COMPONENT		            0x116 	        //获取终端组件信息

#define NET_EHOME_S2C_PUBLISH_XML                   0x200           //日程、插播、升级XML透传（公安平台专用）
#define NET_EHOME_S2C_SCREEN_SHOT                   0x201           //终端截屏（公安平台专用，如果为双面屏终端，表示截主屏）
#define NET_EHOME_S2C_SCREEN_SHOT_SECOND                   0x202           //终端截屏（双面屏终端截屏，截辅屏）

#define NET_EHOME_S2C_GET_VCA_VERSION               0x100034        //获取智能库版本号

#define NET_EHOME_S2C_GET_VCA_MASK_REGION           0x100054        //获取人脸屏蔽区域配置
#define NET_EHOME_S2C_SET_VCA_MASK_REGION           0x100055        //设置人脸屏蔽区域配置

#define NET_EHOME_S2C_SET_CALIBRATION               0x100080        //设置标定场景
#define NET_EHOME_S2C_GET_CALIBRATION               0x100081        //获取标定场景

#define NET_EHOME_S2C_GET_PDC_RULECFG_V42           0x113427        //获取客流量统计规则参数
#define NET_EHOME_S2C_SET_PDC_RULECFG_V42           0x113428        //设置客流量统计规则参数

#define NET_EHOME_S2C_GET_CAMERA_SETUPCFG           0x11350d        //获取相机安装位置
#define NET_EHOME_S2C_SET_CAMERA_SETUPCFG           0x11350e        //配置相机安装位置

#define NET_EHOME_S2C_GET_FACESNAPCFG               0x115001        //获取人脸抓拍参数
#define NET_EHOME_S2C_SET_FACESNAPCFG               0x115002        //设置人脸抓拍参数

#define NET_EHOME_S2C_GET_VCA_CTRLINFO_CFG          0x11503e        //批量获取智能控制参数
#define NET_EHOME_S2C_SET_VCA_CTRLINFO_CFG          0x11503f        //批量设置智能控制参数

#define NET_EHOME_MAX_TYPE_LEN          32      //通用长度
#define NET_EHOME_MAX_NAME_LEN          64      //名称长度
#define NET_EHOME_MAX_TERM_NAME_LEN     100     //终端的名字长度
#define NET_EHOME_MAX_TEXT_MESSAGE_NUM  5       //终端一个窗口最大支持的文字消息条数
#define NET_EHOME_MAX_TEXT_CONTENT_NUM  1024    //插播的文字消息内容的长度
#define NET_EHOME_MAX_PLATFROM_URL_LEN  256     //KMS服务器上文件的URL地址的最大长度
#define NET_EHOME_MAX_WEATHERINFO_LEN   4*1024  //天气信息的最大长度

//二进制数据的发送结构
typedef struct tagNET_EHOME_BINARY_SEND_DATA
{
    DWORD   dwSize;
    void*   lpInBuffer;
    DWORD   dwInBufferSize;
    DWORD   dwRecvTimeOut; //接收超时时间
    BYTE    byRes[64];
}NET_EHOME_BINARY_SEND_DATA, *LPNET_EHOME_BINARY_SEND_DATA;

//二进制数据的接收结构
typedef struct tagNET_EHOME_BINARY_RECV_DATA
{
    DWORD   dwSize;
    void*   lpOutBuffer;
    DWORD   dwOutBufferSize;
    BYTE   byRes[64];
}NET_EHOME_BINARY_RECV_DATA, *LPNET_EHOME_BINARY_RECV_DATA;

//日程下发
typedef struct tagNET_EHOME_POST_SCHEDULE
{
    BYTE    byServerIP[32];         //服务器ip
    DWORD   dwServerPort;           //服务器的端口号
    DWORD   dwScheduleID;           //日程的ID，新建日程时分配的唯一索引
    DWORD   dwScheduleSeq;          //断点续传的时候使用，用来标示日程是否被修改，能否继续传输
    BOOL    byIsDefaultSchedual;    //用来标示这个日程是否是垫片日程
    BYTE    byRes[2];
    DWORD   dwTimingSchedualID;     //定时日程计划的ID，标示终端想要获取的是哪个定时计划
    BYTE    byEffectiveTime[32];    //日程开始播放的时间
}NET_EHOME_POST_SCHEDULE, *LPNET_EHOME_POST_SCHEDULE;

//用来标示发送的是叫号数据还是命令
typedef enum
{
    NET_EHOME_DATA_INFO_TYPE    = 1,    //叫号数据
    NET_EHOME_COMMAND_INFO_TYPE = 2,    //叫号控制命令
    NET_EHOME_CANCEL_SHOT_PIC,          //取消动态弹图
    NET_EHOME_START_CAPTURE_PIC,        //开始抓拍
    NET_EHOME_STOP_CAPTURE_PIC,         //取消显示抓拍图片
}NET_EHOME_THIRD_PARTY_DATA_TYPE;

typedef enum
{
    NET_EHOME_SINGLE_REFRESH_TYPE   = 1, //单个数据
    NET_EHOME_WHOLE_REFRESH_TYPE    = 2, //全部数据
    NET_EHOME_OTHER_REFRESH_TYPE,
}NET_EHOME_REFRESH_TYPE;

typedef struct
{
    char    szQueueDataValue[32];
    DWORD   dwQueueDataId;
}NET_EHOME_QUEUE_DATA;

typedef struct
{
    NET_EHOME_QUEUE_DATA queueData[20];
}NET_EHOME_QUEUE_DATALIST;

typedef struct
{
    DWORD   dwQueueDataListCnt; //行
    DWORD   dwQueueDataCnt;     //列
    NET_EHOME_QUEUE_DATALIST struQueueDatalist[4];
}NET_EHOME_QUEUE_ITEM_DATALIST;

//叫号管理信息
typedef struct tagNET_EHOME_QUEUE_INFO
{
    DWORD   dwMaterialId;       //素材ID
    DWORD   dwQueueId;          //叫号ID
    NET_EHOME_THIRD_PARTY_DATA_TYPE enumDataType;       //命令类型
    NET_EHOME_REFRESH_TYPE          enumRefreshType;    //数据更新方式
    NET_EHOME_QUEUE_ITEM_DATALIST   struItemDataList;   //叫号数据列表
}NET_EHOME_QUEUE_INFO, *LPNET_EHOME_QUEUE_INFO;

//弹图命令
typedef struct tagNET_EHOME_SHOT_PIC
{
    BYTE    byServerIP[32]; //服务器IP地址
    DWORD   dwServerPort;   //服务器端口号
    DWORD   dwShotPicID;    //弹图的图片ID
}NET_EHOME_SHOT_PIC, *LPNET_EHOME_SHOT_PIC;

//错误回应
typedef struct tagNET_EHOME_ERRCODE_RET
{
    DWORD dwCmd;        //命令对应的错误码
    DWORD dwErrCode;    //对应的NET_EHOME_ERROR_CODE的值
}NET_EHOME_ERRCODE_RET, *LPNET_EHOME_ERRCODE_RET;

//服务器和终端之间定义的错误码（2.0及以上版本可用）
enum NET_EHOME_ERROR_CODE
{
    EHOME_OK                        = 0,
    EHOME_PASSERWORD_ERROR          = 1, //密码错误
    EHOME_NOT_SUPPORT               = 2, //终端不支持
    EHOME_USERNAME_ERROR            = 3, //用户名错误
    EHOME_USERNAME_PASSWORD_ERROR   = 4, //用户名或者密码错误
    EHOME_TERM_RNAME_REPEAT         = 5, //终端名称重复
    EHOME_TERM_SERIAL_REPEAT        = 6, //终端序列号重复
    EHOME_PARAM_ERROR               = 7, //参数错误
};

//控制命令对应的命令码
enum NET_EHOME_CONTROL_TYPE
{
    NET_EHOME_CONTROL_STARTPLAY     = 1,    //开始播放
    NET_EHOME_CONTROL_STOPPLAY,             //停止播放
    NET_EHOME_CONTROL_INSERT,               //插播
    NET_EHOME_CONTROL_STOPINSERT,           //停止插播
    NET_EHOME_CONTROL_POWERON,              //开机
    NET_EHOME_CONTROL_POWEROFF,             //关机
    NET_EHOME_CONTROL_REBOOT,               //重启
    NET_EHOME_CONTROL_RESTORECONFIG,        //恢复默认参数
    NET_EHOME_CONTROL_SCREENOPEN,           //屏幕开
    NET_EHOME_CONTROL_SCREENCLOSE,          //屏幕关
    NET_EHOME_CONTROL_PLANCTRL,             //音量和定时开关机计划
    NET_EHOME_CONTROL_CANCELINSERT,         //取消插播
    NET_EHOME_CONTROL_PUBLISH_STATE_SHOW,   //终端发布进度显示
    NET_EHOME_CONTROL_PUBLISH_STATE_HIDE,   //终端发布进度隐藏
};

//插播的类型，插播素材或者节目
enum NET_EHOME_PROGRAM_INSERT_TYPE
{
    NET_EHOME_INSERT_TYPE_MATERIAL = 1,
    NET_EHOME_INSERT_TYPE_PROGRAM,
};

//位置坐标
typedef struct tagNET_EHOME_POSITION_INFO
{
    DWORD dwPositionX;
    DWORD dwPositionY;
    DWORD dwHeight;
    DWORD dwWidth;
}NET_EHOME_POSITION_INFO, *LPNET_EHOME_POSITION_INFO;

//字符效果
typedef struct
{
    DWORD   dwFontSize;             //字体大小
    DWORD   dwFontColor;            //字体颜色
    DWORD   dwBackColor;            //背景颜色
    DWORD   dwBackTransparent;      //透明度
    BOOL    bSubtitlesEnabled;      //使能字符显示模式
    BYTE    szScrollDirection[32];  //滚动方向，left,right,up,down
    DWORD   dwScrollSpeed;          //滚动速度
}NET_EHOME_CHAR_EFFECT;

//插播效果
typedef struct
{
    NET_EHOME_CHAR_EFFECT struCharEffect;   //字符效果
    DWORD   dwPageTime;                     //页面时间
    DWORD   dwScrollSpeedWeb;               //页面滚动速度
}NET_EHOME_INSERT_EFFECT;

//插播素材或者节目的信息
typedef struct tagNET_EHOME_INSERT_TEXT_INFO
{
    DWORD   dwTextNo;       //插播的文件索引，素材ID或者节目ID
    BYTE    szPlayMode[32]; //播放模式，byTime或byEndTime
    DWORD   dwCountNum;     //播放次数，暂时不支持
    DWORD   dwPlayDuration; //播放时间
    BYTE    playEndTime[32];//按照结束时间进行插播
    DWORD   dwTextSeq;      //用于判断文件ID对应的文件是否一致
}NET_EHOME_INSERT_TEXT_INFO, *LPNET_EHOME_INSERT_TEXT_INFO;

//坐标模式
enum NET_EHOME_POS_MODE
{
    NET_EHOME_POS_MODE_ILLEGAL      = 0,
    NET_EHOME_POS_MODE_STANDARD     = 1,    //基准坐标1920*1920
    NET_EHOME_POS_MODE_RESOLUTION   = 2,    //分辨率坐标
};

//插播参数
typedef struct tagNET_EHOME_INSERT_INFO
{
    DWORD   dwInsertType;       //NET_EHOME_PROGRAM_INSERT_TYPE
    NET_EHOME_INSERT_TEXT_INFO  struMaterialInfo;
    NET_EHOME_INSERT_TEXT_INFO  struProgramInfo;
    NET_EHOME_POSITION_INFO     struPosInfo;
    NET_EHOME_INSERT_EFFECT     struInsertEffect;
}NET_EHOME_INSERT_INFO, *LPNET_EHOME_INSERT_INFO;

//插播参数V20
typedef struct tagNET_EHOME_INSERT_INFO_V20
{
    DWORD   dwInsertType;       //NET_EHOME_PROGRAM_INSERT_TYPE
    NET_EHOME_INSERT_TEXT_INFO  struMaterialInfo;
    NET_EHOME_INSERT_TEXT_INFO  struProgramInfo;
    NET_EHOME_POSITION_INFO     struPosInfo;
    NET_EHOME_INSERT_EFFECT     struInsertEffect;
    DWORD   dwPosMode;          //坐标模式，参见枚举NET_EHOME_POS_MODE
    BYTE    byRes[64];
}NET_EHOME_INSERT_INFO_V20, *LPNET_EHOME_INSERT_INFO_V20;

enum NET_EHOME_PLANCTRL_TYPE
{
    NET_EHOME_CONTROL_SWITCHPLAY = 1,   //定时开关机对应的EHome命令
    NET_EHOME_CONTROL_VOLUMEPLAY,       //音量对应的EHome命令
    NET_EHOME_CONTROL_INPUTPLAY,        //定时输入计划是否启用对应的的EHome命令
};

//定时计划控制
typedef struct tagNET_EHOME_PLAY_CTRLPARAM
{
    NET_EHOME_PLANCTRL_TYPE enumPlanType;   //控制的类型
    BOOL    bEnable;                        //是否使能
}NET_EHOME_PLAN_CTRLPARAM, *LPNET_EHOME_PLAY_CTRLPARAM;

//终端控制
typedef struct tagNET_EHOME_TERMINAL_CONTROL
{
    DWORD   dwControlType;                  //NET_EHOME_CONTROL_TYPE 控制命令的类型
    NET_EHOME_INSERT_INFO struInsertInfo;   //插播相关的参数
    NET_EHOME_PLAN_CTRLPARAM struPlanCtrl;  //信息发布终端的定时计划控制
    DWORD   dwConnPort;                     //数据传输端口，暂时没使用
}NET_EHOME_TERMINAL_CONTROL, *LPNET_EHOME_TERMINAL_CONTROL;

//终端控制V20
typedef struct tagNET_EHOME_TERMINAL_CONTROL_V20
{
    DWORD   dwControlType;                      //NET_EHOME_CONTROL_TYPE 控制命令的类型
    NET_EHOME_INSERT_INFO_V20 struInsertInfo;   //插播相关的参数
    NET_EHOME_PLAN_CTRLPARAM struPlanCtrl;      //信息发布终端的定时计划控制
    DWORD   dwConnPort;                         //数据传输端口，暂时没使用
    BYTE    byRes[64];
}NET_EHOME_TERMINAL_CONTROL_V20, *LPNET_EHOME_TERMINAL_CONTROL_V20;

//文字消息的内容
typedef struct tagNET_EHOME_TEXT_MESSAGE
{
    DWORD   dwId;                                       //文字消息唯一索引
    BYTE    szMsgName[NET_EHOME_MAX_NAME_LEN];          //消息名称
    BYTE    szContent[NET_EHOME_MAX_TEXT_CONTENT_NUM];  //消息内容
    BYTE    szBeginTime[NET_EHOME_MAX_TYPE_LEN];        //消息开始插播时间
    BYTE    szEndTime[NET_EHOME_MAX_TYPE_LEN];          //消息插播结束时间
}NET_EHOME_TEXT_MESSAGE, *LPNET_EHOME_TEXT_MESSAGE;

//插播文字消息
typedef struct tagNET_EHOME_INSERT_CHARACTER
{
    DWORD   dwMessageCnt;
    NET_EHOME_TEXT_MESSAGE struTextMessage[NET_EHOME_MAX_TEXT_MESSAGE_NUM];
    BYTE    szMsgPos[NET_EHOME_MAX_TYPE_LEN];   //消息位置，顶部，中部，底部，自定义
    NET_EHOME_POSITION_INFO struPosition;       //消息自定义位置，只有msgPos为自定义时有效
    NET_EHOME_CHAR_EFFECT struCharEffect;       //字符效果
}NET_EHOME_INSERT_CHARACTER, *LPNET_EHOME_INSERT_CHARACTER;

typedef enum
{
    POWER_ON    = 1,   //开机
    POWER_OFF,          //关机
}NET_EHOME_POWER_ONOFF;

typedef enum
{
    INVALID_PLAN    = 0,
    DAYLY_PLAN,         //日计划
    WEEKLY_PLAN,        //周计划
    SELF_DEF_PLAN,      //自定义计划
    CIRCLE_PLAN,        //轮播
}NET_EHOME_PLAY_SCHEDULE_TYPE1;

typedef enum
{
    INVALID_DAY = 0,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,
    SUNDAY,
}NET_EHOME_DAY_OF_WEEK1;

//具体的配置信息
typedef struct
{
    DWORD   dwId;           //标示第几个开关机配置节点
    DWORD   dwSwitchType;   //开关机类型，开机还是关机，1开机，2关机
    DWORD   dwSwitchDate;   //开机或者关机的日期
    DWORD   dwSwitchTime;   //开机或者关机的时间，235959 等价于23:59:59
}NET_EHOME_SWITCH_PLAN, *LPNET_EHOME_SWITCHE_PLAN;

//定时开关机日计划
typedef struct
{
    DWORD   dwDayPlanCnt; //一天有几个开关机节点
    NET_EHOME_SWITCH_PLAN struDayPlan[16];
}NET_EHOME_SWITCH_DAILY_PLAN, *LPNET_EHOME_SWITCH_DAILY_PLAN;

typedef struct
{
    DWORD  dwId;                                    //标示第几天
    NET_EHOME_DAY_OF_WEEK1 enumDayOfWeek;           //标示具体一周中的哪一天
    NET_EHOME_SWITCH_DAILY_PLAN struDayOfWeekPlan;  //日计划
}NET_EHOME_SWITCHE_DAY_OF_WEEK_PLAN, *LPNET_EHOME_SWITCH_DAY_OF_WEEK_PLAN;

//周计划
typedef struct
{
    DWORD   dwWeekPlanCnt; //一周有几天的定时计划
    NET_EHOME_SWITCHE_DAY_OF_WEEK_PLAN struWeekPlan[7];
}NET_EHOME_SWITCH_WEEKLY_PLAN, *LPNET_EHOME_SWITCH_WEEKLY_PLAN;

//发送给终端的定时开关机参数结构
typedef struct
{
    NET_EHOME_PLAY_SCHEDULE_TYPE1 enumPlanType;         //计划类型，周计划还是日计划
    union
    {
        NET_EHOME_SWITCH_DAILY_PLAN struDailyPlan;      //每天的定时计划
        NET_EHOME_SWITCH_WEEKLY_PLAN struWeeklyPlan;    //每周的定时计划
    }unionSwitchPlan;
}NET_EHOME_SWITCH_PLAN_PARAM, *LPNET_EHOME_SWITCH_PLAN_PARAM;

//具体的音量参数
typedef struct
{
    DWORD   dwId;               //用来标示第几个音量配置
    DWORD   dwVolumeValue;      //音量的数值
    DWORD   dwVolumeBeginDate;  //音量的开始日期
    DWORD   dwVolumeBeginTime;  //音量的开始时间235959等价于23:59:59
    DWORD   dwVolumeEndDate;    //音量的数据日期
    DWORD   dwVolumeEndTime;    //音量结束时间
}NET_EHOME_VOLUME_PLAN, *LPNET_EHOME_VOLUME_PLAN;

//音量天计划
typedef struct
{
    DWORD   dwDayPlanCnt;
    NET_EHOME_VOLUME_PLAN struDayPlan[8]; //每一天的音量计划
}NET_EHOME_VOLUME_DAILY_PLAN, *LPNET_EHOME_VOLUME_DAILY_PLAN;

typedef struct
{
    DWORD   dwId;
    NET_EHOME_DAY_OF_WEEK1 enumDayOfWeek; //一周中的哪一天
    NET_EHOME_VOLUME_DAILY_PLAN struDayOfWeekPlan;
}NET_EHOME_VOLUME_DAY_OF_WEEK_PLAN, *LPNET_EHOME_VOLUME_DAY_OF_WEEK_PLAN;

//音量周计划
typedef struct
{
    DWORD   dwWeekPlanCnt;
    NET_EHOME_VOLUME_DAY_OF_WEEK_PLAN struWeekPlan[7];
}NET_EHOME_VOLUME_WEEKLY_PLAN, *LPNET_EHOME_VOLUME_WEEKLY_PLAN;

//发送给终端的定时调节音量参数结构
typedef struct
{
    NET_EHOME_PLAY_SCHEDULE_TYPE1 enumPlanType;
    union
    {
        NET_EHOME_VOLUME_DAILY_PLAN struDailyPlan;      //每天的定时计划
        NET_EHOME_VOLUME_WEEKLY_PLAN struWeeklyPlan;    //每周的定时计划
    }unionVolumePlan;

}NET_EHOME_VOLUME_PLAN_PARAM, *LPNET_EHOME_VOLUME_PLAN_PARAM;

//定时输入切换，某个时间段，具体的计划参数
typedef struct tagNET_EHOME_INPUT_DAY_PLAN
{
    DWORD   dwId;
    BYTE    byInputType[32];    //输入类型，HDMI,InfoPublish,VGA
    DWORD   dwBeginDate;        //年月日转换成十进制
    DWORD   dwBeginTime;        //235959 等价于23:59:59
    DWORD   dwEndDate;          //定时输入切换开始时间
    DWORD   dwEndTime;          //定时输入切换结束时间
}NET_EHOME_INPUT_DAY_PLAN, *LPNET_EHOME_INPUT_DAY_PLAN;

//信息发布平台HDMI和信息发布切换日计划
typedef struct tagNET_EHOME_INPUT_DAILY_PLAN
{
    DWORD   dwDayPlanCnt;                       //一天多少个定时输入切换
    NET_EHOME_INPUT_DAY_PLAN struDayPlan[8];    //最多支持8个
}NET_EHOME_INPUT_DAILY_PLAN, *LPNET_EHOME_INPUT_DAILY_PLAN;

//HDMI和信息发布切换一周中某一天的计划
typedef struct tagNET_EHOME_INPUT_DAY_OF_WEEK_PLAN
{
    DWORD   dwId;
    NET_EHOME_DAY_OF_WEEK1 enumDayOfWeek; //周
    NET_EHOME_INPUT_DAILY_PLAN struDayOfWeekPlan;
}NET_EHOME_INPUT_DAY_OF_WEEK_PLAN, *LPNET_EHOME_INPUT_DAY_OF_WEEK_PLAN;

//信息发布平台HDMI和信息发布切换周计划
typedef struct tagNET_EHOME_INPUT_WEEKLY_PLAN
{
    DWORD   dwWeekPlanCnt;
    NET_EHOME_INPUT_DAY_OF_WEEK_PLAN struWeekPlan[7]; //7天
}NET_EHOME_INPUT_WEEKLY_PLAN, *LPNET_EHOME_INPUT_WEEKLY_PLAN;

//信息发布平台HDMI和信息发布切换自定义计划
typedef struct tagNET_EHOME_INPUT_SELF_PLAN
{
    DWORD   dwSelfPlanCnt;
    NET_EHOME_INPUT_DAY_PLAN struSelfPlan[8];
}NET_EHOME_INPUT_SELF_PLAN, *LPNET_EHOME_INPUT_SELF_PLAN;

//HDMI和信息发布切换计划
typedef struct tagNET_EHOME_INPUT_PLAN
{
    DWORD   dwId;
    BYTE    byPlanType[32];
    union
    {
        NET_EHOME_INPUT_DAILY_PLAN struDailyPlan;
        NET_EHOME_INPUT_WEEKLY_PLAN struWeeklyPlan;
    }unionInputPlan;
}NET_EHOME_INPUT_PLAN, *LPNET_EHOME_INPUT_PLAN;

//终端的播放参数
typedef struct tagNET_EHOME_DISPLAY_PARAM
{
    DWORD   dwR;        //显示的颜色，红
    DWORD   dwG;
    DWORD   dwB;
    DWORD   dwOffsetR;  //红偏移
    DWORD   dwOffsetG;
    DWORD   dwOffsetB;
    DWORD   dwBrightnessLevel;  //亮度
    DWORD   dwContrastLevel;    //对比度
    DWORD   dwSharpnessLevel;   //锐度
    DWORD   dwSaturationLevel;  //饱和度
    DWORD   dwHueLevel;         //色调，上边的参数暂时不支持，可以不用赋值
    //自动亮度调节
    BOOL    bAutoLightEnable;   //自动亮度调节（2.0新增）
    DWORD   dwLightMode;        //自动亮度的值（2.0新增，有个范围的）
    DWORD   dwBackLightLevel;   //背光参数
    BOOL    bBootLogoEnabled;   //开机Logo
    DWORD   dwScreenVolume;     //屏幕音量，暂时不支持
    //垫片信息
    BOOL    bDefaultScheduleEnable;     //垫片日程是否使能
    DWORD   dwDefaultScheduleId;        //垫片日程的ID
    BYTE    byDefaultScheduleName[32];  //垫片日程的名字
    BOOL    bTemperaturenable;          //温度安全是否启用（2.0新增）
    DWORD   bSecurity;                  //温度的安全值
    DWORD   bProtectValue;              //温度的保护值
}NET_EHOME_DISPLAY_PARAM, *LPNET_EHOME_DISPLAY_PARAM;

//终端密码参数
typedef struct tagNET_EHOME_PASSWORD_CFG
{
    BYTE    szOldPwd[32]; //老的密码
    BYTE    szNewPwd[32]; //新密码
}NET_EHOME_PASSWORD_CFG, *LPNET_EHOME_PASSWORD_CFG;

//终端的参数
typedef struct tagNET_EHOME_PLAYER_PARAM
{
    BYTE    szCfgType[32];  //volume,light,logo,defaultSchedule,password,temperature,allParam
    DWORD   dwplayerVolume; //播放音量
    NET_EHOME_DISPLAY_PARAM struDisplayParam;   //终端的播放参数
    NET_EHOME_PASSWORD_CFG struPasswordCfg;     //终端的密码
}NET_EHOME_PLAYER_PARAM, *LPNET_EHOME_PLAYER_PARAM;

/*终端设备的基本参数*/
typedef struct tagNET_EHOME_DEV_DEFAULT_PARAM
{
    BOOL    bAutoLightEnable;       //自动亮度调节（新增）
    DWORD   dwLightMode;            //亮度值（新增）
    DWORD   dwBackLightLevel;       //背光参数
    DWORD   dwVolume;               //音量
    BOOL    bEnableStartLogo;       //开机LOGO是否使能
    BOOL    bEnableVolumeTiming;    //定时音量计划是否使能
    BOOL    bEnablePowerTiming;     //定时开关机计划是否使能
    BOOL    bDefaultScheduleEnable; //垫片是否使能
    DWORD   dwDefaultScheduleId;    //垫片日程的id
    BYTE    szDefaultScheduleName[32]; //垫片日程的名字
    BOOL    bTemperaturenable;      //温度安全是否启用（2.0新增）
    DWORD   dwSecurity;             //温度的安全值
    DWORD   dwProtectValue;         //温度的保护值
}NET_EHOME_DEV_DEFAULT_PARAM, *LPNET_EHOME_DEV_DEFAULT_PARAM;

//终端的IP对应的网关
typedef struct tagNET_EHOME_TERM_DEFAULT_GATEWAY
{
    BYTE    szIpAddress[32];    //Ipv4的网关
    BYTE    szIpv6Address[32];  //Ipv6的网关
}NET_EHOME_TERM_DEFAULT_GATEWAY, *LPNET_EHOME_TERM_DEFAULT_GATEWAY;

//终端的ip地址信息
typedef struct tagNET_EHOME_TERM_IP_ADDRESS
{
    BYTE    szIpVersion[32];    //v4,v6
    BYTE    szIpAddress[32];    //IP地址
    BYTE    szSubnetMask[32];   //子网掩码
    BYTE    szIpv6Address[32];  //Ipv6地址，暂时不支持
    BYTE    szBitMask[32];      //Ipv6网关
    NET_EHOME_TERM_DEFAULT_GATEWAY strDefaultGateway;
}NET_EHOME_TERM_IP_ADDRESS, *LPNET_EHOME_TERM_IP_ADDRESS;

enum NET_EHOME_TIME_ZONE
{
    NET_EHOME_ZONE_GMT_LOCAL,       // 使用当前时区配置
    NET_EHOME_ZONE_GMT_MINUS_12,
    NET_EHOME_ZONE_GMT_MINUS_11,
    NET_EHOME_ZONE_GMT_MINUS_10,
    NET_EHOME_ZONE_GMT_MINUS_9,
    NET_EHOME_ZONE_GMT_MINUS_8,
    NET_EHOME_ZONE_GMT_MINUS_7,
    NET_EHOME_ZONE_GMT_MINUS_6,
    NET_EHOME_ZONE_GMT_MINUS_5,
    NET_EHOME_ZONE_GMT_MINUS_430,
    NET_EHOME_ZONE_GMT_MINUS_4,
    NET_EHOME_ZONE_GMT_MINUS_330,
    NET_EHOME_ZONE_GMT_MINUS_3,
    NET_EHOME_ZONE_GMT_MINUS_2,
    NET_EHOME_ZONE_GMT_MINUS_1,
    NET_EHOME_ZONE_GMT_0,           //UTC 时区
    NET_EHOME_ZONE_GMT_PLUS_1,
    NET_EHOME_ZONE_GMT_PLUS_2,
    NET_EHOME_ZONE_GMT_PLUS_3,
    NET_EHOME_ZONE_GMT_PLUS_330,
    NET_EHOME_ZONE_GMT_PLUS_4,
    NET_EHOME_ZONE_GMT_PLUS_430,
    NET_EHOME_ZONE_GMT_PLUS_5,
    NET_EHOME_ZONE_GMT_PLUS_530,
    NET_EHOME_ZONE_GMT_PLUS_545,
    NET_EHOME_ZONE_GMT_PLUS_6,
    NET_EHOME_ZONE_GMT_PLUS_630,
    NET_EHOME_ZONE_GMT_PLUS_7,
    NET_EHOME_ZONE_GMT_PLUS_8,      //东八区
    NET_EHOME_ZONE_GMT_PLUS_9,
    NET_EHOME_ZONE_GMT_PLUS_930,
    NET_EHOME_ZONE_GMT_PLUS_10,
    NET_EHOME_ZONE_GMT_PLUS_11,
    NET_EHOME_ZONE_GMT_PLUS_12,
    NET_EHOME_ZONE_GMT_PLUS_13,
};

//时间同步参数
typedef struct tagNET_EHOME_TIME_ADJUST_PARAM
{
    DWORD   dwTimeZone; //参考NET_EHOME_TIME_ZONE
    BYTE    szTime[32]; // YYYY-MM-DD HH:MM:SS
}NET_EHOME_TIME_ADJUST_PARAM, *LPNET_EHOME_TIME_ADJUST_PARAM;

//终端ADB控制信息
typedef struct tagNET_EHOME_ADB_DEBUG
{
    BOOL    bAdbEnable;
    BYTE    bySecreKey[128];
}NET_EHOME_ADB_DEBUG, *LPNET_EHOME_ADB_DEBUG;

//终端时区信息
typedef struct tagNET_EHOME_TERM_TIME_ZONE
{
    BYTE    szTimeZone[32]; //时区信息，GMT+08
}NET_EHOME_TERM_TIME_ZONE, *LPNET_EHOME_TERM_TIME_ZONE;

//终端的discovery模式参数，SADP是否使能
typedef struct tagNET_EHOME_DISCOVERY_MODE
{
    BOOL    bSadpMode;
}NET_EHOME_DISCOVERY_MODE, *LPNET_EHOME_DISCOVERY_MODE;

typedef enum
{
    ADDRTYPE_IPV4 = 0,
    ADDRTYPE_IPV6,
    HOSTNAME,
}NET_EHOME_ADDRFORMAT_CMD;

typedef struct tagNET_EHOME_PUBLISH_SERVERADDR
{
    NET_EHOME_ADDRFORMAT_CMD enumAddressingFormatType;
    BYTE    szIpAddress[NET_EHOME_MAX_TYPE_LEN];
    BYTE    szIpv6Address[NET_EHOME_MAX_TYPE_LEN];
    BYTE    szHostName[NET_EHOME_MAX_TYPE_LEN];
    DWORD   dwPortNo;
    BYTE    szUserName[NET_EHOME_MAX_TYPE_LEN];
    BYTE    szPassword[NET_EHOME_MAX_TYPE_LEN];
    BOOL    bRegStatus;
}NET_EHOME_PUBLISH_SERVERADDR, *LPNET_EHOME_PUBLISH_SERVERADDR;

typedef enum
{
    NET_EHOME_NORMAL_IPC,                   //普通IPC
    NET_EHOME_SMART_PASSENGER_IPC,          //客流量IPC
    NET_EHOME_SMART_VALID_PASSENGER_IPC,    //有效客流量IPC（人脸识别）
}NET_EHOME_IPC_TYPE;

typedef enum
{
    NET_EHOME_HOST_MODE,    //主机名
    NET_EHOME_IP_MODE,      //IP地址
}NET_EHOME_ADDRESS_TYPE;

//取流协议
typedef enum
{
    NET_EHOME_TCP = 1,
    NET_EHOME_UDP,
    NET_EHOME_MCAST,
}NET_EHOME_TRANSMINT_PROTOCAL;

//码流类型
typedef enum
{
    NET_EHOME_MAIN  = 1,    //主码流
    NET_EHOME_SUB,          //子码流
    NET_EHOME_THIRD,
}NET_EHOME_STREAM_TYPE;

//单个IPC信息
typedef struct tagNET_EHOME_IPC_INFO
{
    NET_EHOME_IPC_TYPE enumIPCType; //参考NET_EHOME_IPC_TYPE
    DWORD   dwId;
    DWORD   chanNum;                //ipc关联的节目中的通道号
    NET_EHOME_ADDRESS_TYPE enumAddressType; //IP地址类型，参考NET_EHOME_ADDRESS_TYPE
    BYTE    szIpcState[32];         //online和offline两种状态
    BYTE    szHostName[32];         //主机名
    BYTE    szIpVersion[16];        //v4或者v6
    BYTE    szIpAddress[32];        //ipv4地址
    BYTE    szIpv6Address[32];      //ipV6地址
    DWORD   dwPortNo;               //ipc的端口号
    DWORD   dwIpcChanNum;           //ipc通道号
    BYTE    szUserName[52];         //ipc登陆用户名
    BYTE    szPassWord[32];         //ipc登陆密码
    NET_EHOME_TRANSMINT_PROTOCAL enumTransmitProtocol;//取流协议，参考NET_EHOME_TRANSMINT_PROTOCAL
    NET_EHOME_STREAM_TYPE enumStreamType; //码流类型，参考NET_EHOME_STREAM_TYPE
}NET_EHOME_IPC_INFO, *LPNET_EHOME_IPC_INFO;

//终端关联IPC信息
typedef struct tagNET_EHOME_RELATE_IPC_INFO
{
    DWORD               dwIpcNum;       //IPC个数，添加了几个IPC
    NET_EHOME_IPC_INFO  struIpcInfo[6];
}NET_EHOME_RELATE_IPC_INFO, *LPNET_EHOME_RELATE_IPC_INFO;

//获取设备工作状态
typedef struct tagNET_EHOME_DEV_WORK_STATUS
{
    BYTE    byIdentifyCode[32];     //终端序列号
    DWORD   dwCpuUsage;             //cpu利用率
    DWORD   dwMemTotal;             //内存总大小
    DWORD   dwMemUsed;
    DWORD   dwDiskTotal;            //磁盘总容量
    DWORD   dwDiskUsed;             //磁盘利用率
    DWORD   dwTemperature;          //终端温度
    BYTE    bySoftwareVersion[32];  //软件版本号
    BYTE    byDspwareVersion[32];   //DSP版本号
    BYTE    byHardwareVersion[32];  //硬件版本号
    BYTE    bySystemVersion[32];    //系统版本号
    BYTE    byMacAddress[2][32];    //最大支持2个网卡
    BOOL    bySwitchEnable;         //定时开关机是否使能
    BOOL    byVolumeEnable;         //定时音量是否使能
    DWORD   dwRelateScheNo;         //日程id
    BYTE    byScheName[100];        //终端正在播放的日程名字
    BYTE    byInstallType[32];      //vertical or horizontal，横屏or竖屏
    DWORD   byResWidth;             //分辨率
    DWORD   byResHeight;            //分辨率
}NET_EHOME_DEV_WORK_STATUS, *LPNET_EHOME_DEV_WORK_STATUS;

//终端截屏
typedef struct tagNET_EHOME_SCREEN_SHOT
{
    DWORD   dwServerPort;       //服务器端口号
    DWORD   dwClientFdIndex;    //客户端fd索引
}NET_EHOME_SCREEN_SHOT, *LPNET_EHOME_SCREEN_SHOT;

//升级命令tcp链路协商
typedef struct tagNET_EHOME_UPGRADE_CONN_PARAM
{
    BYTE    szServerIP[32]; //服务器IP地址
    DWORD   dwServerPort;   //服务器端口号
}NET_EHOME_UPGRADE_CONN_PARAM, *LPNET_EHOME_UPGRADE_CONN_PARAM;

//终端回应升级状态结构体
typedef struct tagNET_EHOME_TERMINAL_UPG_ST_REPORT
{
    DWORD   dwLength;   //结构体总长度
    DWORD   dwCheckSum; //校验和
    DWORD   dwRetVal;   //1000升级成功，1001正在升级，1002升级失，1003解压缩失败，1004 APK版本不匹配，1005 ROM版本不匹配
    DWORD   dwUpgradPercent;
}NET_EHOME_TERMINAL_UPG_ST_REPORT, *LPNET_EHOME_TERMINAL_UPG_ST_REPORT;

//终端名称
typedef struct tagNET_EHOME_TERMINAL_NAME
{
    BYTE    szTermName[NET_EHOME_MAX_TERM_NAME_LEN];
}NET_EHOME_TERMINAL_NAME, *LPNET_EHOME_TERMINAL_NAME;

//终端组件信息
typedef struct tagNET_EHOME_SYSCOMPONENT_REDUCED_ADDR
{
    BYTE    szComponentName[32];    //组件名称，例如：SDK    
    BYTE    szComponentPkgName[32]; //组件包名，例如：com.hikvision.sdk，不需要在客户端显示
    BYTE    szComponentVersion[32]; //组件版本，例如：V2.0.2 build 20180705
    DWORD   dwIsEnableAutoStart;    //是否开机自启动，暂时不需要在客户端显示
    DWORD   dwIsSupportUpgrade;     //是否支持远程升级  
    BYTE    byRes[20];
}NET_EHOME_SYSCOMPONENT_REDUCED_ADDR, *LPNET_EHOME_SYSCOMPONENT_REDUCED_ADDR;

typedef struct tagNET_EHOME_COMPONENT_GROUP_INFO
{
    DWORD   dwComponentNum; //系统组件个数
    NET_EHOME_SYSCOMPONENT_REDUCED_ADDR struSysComponentAddr[6]; //组件信息
    BYTE    byRes[20];
}NET_EHOME_COMPONENT_GROUP_INFO, *LPNET_EHOME_COMPONENT_GROUP_INFO;

//素材替换
typedef struct tagNET_EHOME_REPLACE_MATERIAL
{
    BYTE    szServerIP[NET_EHOME_MAX_TYPE_LEN]; //服务器IP
    DWORD   dwServerPort;   //服务器端口
    DWORD   dwMaterialId;   //素材ID
    DWORD   dwMaterialSeq;  //素材Seq
}NET_EHOME_REPLACE_MATERIAL, *LPNET_EHOME_REPLACE_MATERIAL;

//IPAG日程、插播、升级XML透传
typedef struct tagNET_EHOME_POST_PUBLISH_XML
{
    BYTE    szServerIP[32];     //服务器IP
    DWORD   szServerPort;       //服务器的端口号
    DWORD   dwTerminalID;       //终端ID，返回进度时带上来  
    DWORD   dwXmlUniqueSeq;     //唯一xml标示
    DWORD   dwXmlSizeH;         //xml大小高32位
    DWORD   dwXmlSizeL;         //xml大小低32位
    BYTE    byStorageId[128];   //xml文件存放位置
    BYTE	byRes[16];          //保留字段
}NET_EHOME_POST_PUBLISH_XML, *LPNET_EHOME_POST_PUBLISH_XML;

//城市天气信息
typedef struct tagNET_EHOME_WEATHER_INFO
{
    BYTE   byCityCode[32];          //城市编码
    BYTE   byWeatherData[4 * 1024]; //天气数据
}NET_EHOME_WEATHER_INFO, *LPNET_EHOME_WEATHER_INFO;

//终端的序列号
typedef struct tagNET_EHOME_IDENTIFICATION
{
    BYTE bySerialNumber[32]; //这个字段暂时没用
    BYTE byIdentifyCode[32]; //终端的序列号，即特征码
}NET_EHOME_IDENTIFICATION, *LPNET_EHOME_IDENTIFICATION;

//终端信息
typedef struct tagNET_EHOME_TERMINAL_INFO
{
    NET_EHOME_IDENTIFICATION struDevIdentify;
    DWORD   dwNetUintType;
    BYTE    byDeviceID[NET_EHOME_MAX_TYPE_LEN];
    BYTE    byPassWord[NET_EHOME_MAX_TYPE_LEN];
    BYTE    byFirmWareVersion[NET_EHOME_MAX_TYPE_LEN];
    BYTE    byLocalIP[NET_EHOME_MAX_TYPE_LEN]; //终端通信IP
    DWORD   dwLocalPort;
    DWORD   dwDevType;
    DWORD   dwManufacture;  //0-hikvision
    BOOL    bDetectPackage; //1-探测包，0-注册包
    BOOL    bReliableTrans; //1-支持可靠传输，0-不支持可靠传输
    BYTE    byServerUserName[NET_EHOME_MAX_NAME_LEN]; //服务器用户名
    BYTE    byServerUserPassword[NET_EHOME_MAX_NAME_LEN]; //服务器密码
    BYTE    byTerminalName[NET_EHOME_MAX_NAME_LEN]; //来注册的终端的名字
}NET_EHOME_TERMINAL_INFO, *LPNET_EHOME_TERMINAL_INFO;

//信息发布服务器信息
typedef struct tagNET_EHOME_RELEASE_SERVER_INFO
{
    DWORD   dwKeepAliveSeconds;     //保活时间，默认50s
    BYTE    byAlarmServerIP[32];
    DWORD   dwAlarmServerPort;
    DWORD   dwAlarmServerType;      //通信类型，0-TCP, 1-UDP，默认为0
    DWORD   dwAlarmServerTcpPort;   //服务器为终端申请的报警端口
    BYTE    byNtpServerIP[32];
    DWORD   dwNtpServerPort;
    DWORD   dwNtpInterval;
    BYTE    byPicServerIP[32];
    DWORD   dwPicServerPort;
    DWORD   dwPicServerType;
    BYTE    byBlackListAddr[32];
    BYTE    byBlackListName[32];
    DWORD   dwBlackListPort;
    BYTE    byBlackListUser[32];
    BYTE    byBlackListPasswd[32];
    DWORD   dwTranserialSvrPort;
    BOOL    bReliableTransmission;
}NET_EHOME_RELEASE_SERVER_INFO, *LPNET_EHOME_RELEASE_SERVER_INFO;

//终端截屏（公安平台专用）
typedef struct tagNET_EHOME_SCREEN_SHOT_EX
{
    DWORD  dwServerPort;    //服务器端口号
    DWORD  dwClientFdIndex; //客户端fd索引
    BYTE   szKmsURL[256];   //终端要上传到KMS的URL地址
}NET_EHOME_SCREEN_SHOT_EX, *LPNET_EHOME_SCREEN_SHOT_EX;

//终端截屏返回结构
typedef struct tagNET_EHOME_SCREEN_SHOT_RET
{
    unsigned int    dwConnfd;       //终端和服务器异步通信套接字
    unsigned int    dwSeq;          //终端和服务器异步交互的标识
    char            szUUID[64];     //截屏图片的uuid唯一标示
    char            szPicURL[256];  //截图在KMS服务器上的URL地址
}NET_EHOME_SCREEN_SHOT_RET, *LPNET_EHOME_SCREEN_SHOT_RET;

NET_DVR_API BOOL CALLBACK NET_ECMS_STDBinaryCfg(LONG lUserID, LONG dwCommand, NET_EHOME_BINARY_SEND_DATA* pSendData, NET_EHOME_BINARY_RECV_DATA* pRecvData);

NET_DVR_API BOOL CALLBACK NET_ECMS_STDBinaryCtrl(LONG lUserID, LONG dwCommand, NET_EHOME_BINARY_SEND_DATA* pSendData);

/** 兼容信息发布私有协议 end */

NET_DVR_API LONG CALLBACK NET_ECMS_LongConfigCreate(LONG lUserlD, LPNET_EHOME_LONG_CFG_INPUT pLongCfgInput);

NET_DVR_API BOOL CALLBACK NET_ECMS_LongConfigSend(LONG lHandle, LPNET_EHOME_LONG_CFG_SEND pSend);

NET_DVR_API BOOL CALLBACK NET_ECMS_LongConfigDestory(LONG lHandle);

NET_DVR_API BOOL CALLBACK NET_ECMS_LongConfigGetSessionId(LONG iHandle, LONG* pSessionId);

typedef struct tagNET_EHOME_ASYNC_RESP_CB_DATA
{
    void*   pOutBuffer;     //设备响应数据
    DWORD   dwOutLen;       //设备响应数据长度
    DWORD   dwErrorNo;      //SDK错误码
    DWORD   dwHandle;       //消息句柄，唯一标识，匹配请求和响应
    LONG    lUserID;        //用户ID
    char    byRes[32];
}NET_EHOME_ASYNC_RESP_CB_DATA, *LPNET_EHOME_ASYNC_RESP_CB_DATA;

typedef BOOL(CALLBACK * ASYNC_RESPONSE_CB)(LPNET_EHOME_ASYNC_RESP_CB_DATA lpData, void* pUser);

NET_DVR_API BOOL CALLBACK NET_ECMS_SetXmlConfigResponseCB(ASYNC_RESPONSE_CB fnCB, void* pUser);

NET_DVR_API BOOL CALLBACK NET_ECMS_XMLConfigEx(LONG lUserID, NET_EHOME_XML_CFG* pXmlCfg, DWORD* dwHandle);

NET_DVR_API BOOL CALLBACK NET_ECMS_SetAliveTimeout(LONG lUserID, DWORD dwKeepAliveSec, DWORD dwTimeOutCount);

#endif //_HC_EHOME_CMS_H_
