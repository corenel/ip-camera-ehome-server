#ifndef _HC_EHOME_PUBLIC_H_
#define _HC_EHOME_PUBLIC_H_

//多SDK公共定义
#ifndef _HC_NET_SDK_H_

/*******************平台相关的数据类型定义 begin**********************/

#if (defined(_WIN32)) //windows
    #define NET_DVR_API  extern "C" __declspec(dllimport)
    typedef  unsigned __int64 UINT64;
#elif defined(__linux__) || defined(__APPLE__) //linux
    typedef    unsigned int         DWORD;
    typedef    unsigned short       WORD;
    typedef    unsigned short       USHORT;
    typedef    short                SHORT;
    typedef    int                  LONG;
    typedef    unsigned char        BYTE;
    #define    BOOL                 int
    typedef    unsigned int         UINT;
    typedef    void*                LPVOID;
    typedef    void*                HANDLE;
    typedef    unsigned int*        LPDWORD;
    typedef    unsigned long long   UINT64;

    #ifndef    TRUE
    #define    TRUE     1
    #endif
    #ifndef    FALSE
    #define    FALSE    0
    #endif
    #ifndef    NULL
    #define    NULL     0
    #endif

    #define __stdcall
    #define CALLBACK

    #define NET_DVR_API extern "C"
#endif //linux

#if defined(_WIN64)
#define OS_WINDOWS64    1
#endif

#if defined(__LP64__)
#define OS_POSIX64    1
#endif

/*******************平台相关的数据类型定义 end**********************/

/*******************全局错误码 begin**********************/
#define NET_DVR_NOERROR                 0   //没有错误
#define NET_DVR_PASSWORD_ERROR          1   //用户名密码错误
#define NET_DVR_NOENOUGHPRI             2   //权限不足
#define NET_DVR_NOINIT                  3   //没有初始化
#define NET_DVR_CHANNEL_ERROR           4   //通道号错误
#define NET_DVR_OVER_MAXLINK            5   //连接到DVR的客户端个数超过最大
#define NET_DVR_VERSIONNOMATCH          6   //版本不匹配
#define NET_DVR_NETWORK_FAIL_CONNECT    7   //连接服务器失败
#define NET_DVR_NETWORK_SEND_ERROR      8   //向服务器发送失败
#define NET_DVR_NETWORK_RECV_ERROR      9   //从服务器接收数据失败
#define NET_DVR_NETWORK_RECV_TIMEOUT    10  //从服务器接收数据超时
#define NET_DVR_NETWORK_ERRORDATA       11  //传送的数据有误
#define NET_DVR_ORDER_ERROR             12  //调用次序错误
#define NET_DVR_OPERNOPERMIT            13  //无此权限
#define NET_DVR_COMMANDTIMEOUT          14  //DVR命令执行超时c

#define NET_DVR_PARAMETER_ERROR         17  //参数错误

#define NET_DVR_NOSUPPORT               23  //服务器不支持

#define NET_DVR_DVROPRATEFAILED         29  //DVR操作失败

#define NET_DVR_DIR_ERROR               40  //路径错误
#define NET_DVR_ALLOC_RESOURCE_ERROR    41  //资源分配错误
#define NET_DVR_AUDIO_MODE_ERROR        42  //声卡模式错误
#define NET_DVR_NOENOUGH_BUF            43  //缓冲区太小
#define NET_DVR_CREATESOCKET_ERROR      44  //创建SOCKET出错
#define NET_DVR_SETSOCKET_ERROR         45  //设置SOCKET出错
#define NET_DVR_MAX_NUM                 46  //个数达到最大
#define NET_DVR_USERNOTEXIST            47  //用户不存在

#define NET_DVR_GETLOCALIPANDMACFAIL    53  //获得本地的IP地址或物理地址失败

#define NET_DVR_VOICEMONOPOLIZE         69  //声卡被独占

#define NET_DVR_CREATEDIR_ERROR         71  //建立日志文件目录失败
#define NET_DVR_BINDSOCKET_ERROR        72  //绑定套接字失败
#define NET_DVR_SOCKETCLOSE_ERROR       73  //socket连接中断，此错误通常是由于连接中断或目的地不可达
#define NET_DVR_USERID_ISUSING          74  //注销时用户ID正在进行某操作
#define NET_DVR_SOCKETLISTEN_ERROR      75  //监听失败

#define NET_DVR_CONVERT_SDK_ERROR       85  //加载转码库失败

#define NET_DVR_FUNCTION_NOT_SUPPORT_OS 98  //此功能不支持该操作系统

#define NET_DVR_USE_LOG_SWITCH_FILE         103 //正在使用日志开关文件

#define NET_DVR_PACKET_TYPE_NOT_SUPPORT     105 //码流封装格式错误

#define NET_DVR_STREAM_ENCRYPT_CHECK_FAIL   130 //码流加密校验失败

//语音对讲库错误码
#define NET_AUDIOINTERCOM_OK                   600 //无错误
#define NET_AUDIOINTECOM_ERR_NOTSUPORT         601 //不支持
#define NET_AUDIOINTECOM_ERR_ALLOC_MEMERY      602 //内存申请错误
#define NET_AUDIOINTECOM_ERR_PARAMETER         603 //参数错误
#define NET_AUDIOINTECOM_ERR_CALL_ORDER        604 //调用次序错误
#define NET_AUDIOINTECOM_ERR_FIND_DEVICE       605 //未发现设备
#define NET_AUDIOINTECOM_ERR_OPEN_DEVICE       606 //不能打开设备诶
#define NET_AUDIOINTECOM_ERR_NO_CONTEXT        607 //设备上下文出错
#define NET_AUDIOINTECOM_ERR_NO_WAVFILE        608 //WAV文件出错
#define NET_AUDIOINTECOM_ERR_INVALID_TYPE      609 //无效的WAV参数类型
#define NET_AUDIOINTECOM_ERR_ENCODE_FAIL       610 //编码失败
#define NET_AUDIOINTECOM_ERR_DECODE_FAIL       611 //解码失败
#define NET_AUDIOINTECOM_ERR_NO_PLAYBACK       612 //播放失败
#define NET_AUDIOINTECOM_ERR_DENOISE_FAIL      613 //降噪失败
#define NET_AUDIOINTECOM_ERR_UNKOWN            619 //未知错误

#define NET_ERR_INFOPUBLISH_APP_NOTRUN         1065 //信息发布应用未运行
#define NET_ERR_ATTENDANCE_APP_NOTRUN          1066 //考勤应用未运行
#define NET_ERR_TERM_NAME_REPEAT               1313 //终端名称重复
#define NET_ERR_TERM_SERIAL_REPEAT             1314 //终端序列号重复

#define NET_SS_CLIENT_ERR_KMS_TOKEN_FAIL       3601 //KMS图片上传协议,获取Token失败
#define NET_SS_CLIENT_ERR_KMS_UPLOAD_FAIL      3602 //KMS图片上传协议,上传失败
#define NET_SS_CLIENT_ERR_CLOUD_POOLIST_FAIL   3603 //云存储协议,获取资源池失败
#define NET_SS_CLIENT_ERR_CLOUD_BESTNODE_FAIL  3604 //云存储协议,获取最优节点失败
#define NET_SS_CLIENT_ERR_DOWNLOAD_PIC_FAIL     3605   //EHome5.0存储协议,下载图片失败
#define NET_SS_CLIENT_ERR_DELETE_PIC_FAIL 3606     //EHome5.0存储协议,删除图片失败
#define NET_SS_CLIENT_ERR_PROTO_UNSAFE  3607    // [add] by yangzheng 2020/03/13 EHome5.0, storage server in security mode, nonsupport unsafe protocol VRB/Tomcat
#define NET_SS_CLIENT_ERR_FILE_INEXISTED  3608 // [add] by yangzheng 2020/03/13 EHome5.0, delete or download file not existed
#define NET_SS_CLIENT_ERR_AUTH_FAILED  3609 // [add] by yangzheng 2020/03/13 authentication failed

//休眠唤醒错误码段   8301~8330
#define NET_ERR_DEV_SLEEP               8301    //设备已休眠
#define NET_ERR_DEV_GOINGTOSLEEP        8302    //当前设备处于执行休眠动作的过程中（预计13S），即将进入休眠状态，此时无法进行NET_ECMS_WakeUp接口的唤醒操作；直至执行休眠动作结束，正式进入休眠状态才可正常调用唤醒接口


/*******************全局错误码 begin**********************/
#define MAX_PASSWD_LEN          32
#define NAME_LEN                32      //用户名长度

#endif //_HC_NET_SDK_H_
#define MAX_DEVICE_ID_LEN       256     //设备ID长度
#define NET_EHOME_SERIAL_LEN    12
#define MAX_FULL_SERIAL_NUM_LEN 64
#define MAX_MASTER_KEY_LEN      16
#define MAX_TIME_LEN            32      //时间字符串长度
#define MAX_URL_LEN_SS          4096    //图片服务器回调URL长度

#define REGISTER_LISTEN_MODE_ALL    0      //默认为此种方式，监听TCP和UDP两种方式
#define REGISTER_LISTEN_MODE_UDP    1      //注册只对UDP进行监听
#define REGISTER_LISTEN_MODE_TCP    2      //注册只对TCP进行监听

typedef struct tagNET_EHOME_IPADDRESS
{
    char szIP[128];
    WORD wPort;     //端口
    char byRes[2];
}NET_EHOME_IPADDRESS, *LPNET_EHOME_IPADDRESS;

typedef struct tagNET_EHOME_ZONE
{
    DWORD dwX;          //X轴坐标
    DWORD dwY;          //Y轴坐标
    DWORD dwWidth;      //宽度
    DWORD dwHeight;     //高度
}NET_EHOME_ZONE, *LPNET_EHOME_ZONE;

//本地配置
typedef enum tagNET_EHOME_LOCAL_CFG_TYPE
{
    UNDEFINE                = -1,   //暂时没有具体的定义
    ACTIVE_ACCESS_SECURITY  = 0,    //设备主动接入的安全性
    AMS_ADDRESS             = 1,    //报警服务器本地回环地址
    SEND_PARAM              = 2,    //发送参数配置
    SET_REREGISTER_MODE     = 3,    //设置设备重复注册模式
    LOCAL_CFG_TYPE_GENERAL  = 4,    //通用参数配置
    COM_PATH                = 5,    //COM路径
    SESSIONKEY_REQ_MOD      = 6,    //sessionkey请求是否回调，lpInBuff类型为HPR_BOOL*,HPR_TRUE/HPR_FALSE：回调/不回调
    DEV_DAS_PINGREO_CALLBACK = 7,   //设备心跳注册回调
    REGISTER_LISTEN_MODE    = 8,    //注册监听模式 对应结构体为NET_EHOME_REGISTER_LISTEN_MODE
    STREAM_PLAYBACK_PARAM   = 9     //回放本地参数配置
}NET_EHOME_LOCAL_CFG_TYPE, *LPNET_EHOME_LOCAL_CFG_TYPE;

typedef struct tagNET_EHOME_LOCAL_ACCESS_SECURITY
{
    DWORD   dwSize;
    BYTE    byAccessSecurity;    //0-兼容模式（允许任意版本的协议接入），1-普通模式（只支持4.0以下版本，不支持协议安全的版本接入） 2-安全模式（只允许4.0以上版本，支持协议安全的版本接入）
    BYTE    byRes[127];
}NET_EHOME_LOCAL_ACCESS_SECURITY,*LPNET_EHOME_LOCAL_ACCESS_SECURITY;

typedef struct tagNET_EHOME_AMS_ADDRESS
{
    DWORD dwSize;
    BYTE  byEnable;  //0-关闭CMS接收报警功能，1-开启CMS接收报警功能
    BYTE  byRes1[3];
    NET_EHOME_IPADDRESS  struAddress;   //AMS本地回环地址
    BYTE byRes2[32];
}NET_EHOME_AMS_ADDRESS, *LPNET_EHOME_AMS_ADDRESS;

typedef struct tagNET_EHOME_SEND_PARAM
{
    DWORD dwSize;
    DWORD dwRecvTimeOut;    //接收超时时间，单位毫秒
    BYTE  bySendTimes;      //报文发送次数，为了应对网络环境较差的情况下，丢包的情况，默认一次，最大3次
    BYTE  byRes2[127];
}NET_EHOME_SEND_PARAM, *LPNET_EHOME_SEND_PARAM;

//Ehome5.0协议设备EhomeKey信息
typedef struct tagNET_EHOME_DEV_SESSIONKEY
{
    BYTE   sDeviceID[MAX_DEVICE_ID_LEN];        //设备ID/*256*/
    BYTE   sSessionKey[MAX_MASTER_KEY_LEN];     //设备Sessionkey/*16*/
} NET_EHOME_DEV_SESSIONKEY, *LPNET_EHOME_DEV_SESSIONKEY;

typedef struct tagNET_EHOME_SET_REREGISTER_MODE
{
    DWORD dwSize;
    DWORD dwReRegisterMode;    //设置设备重复注册模式，仅针对EHome2.0协议，若为0，表示在保活时间内设备重复注册，EHomeSDK接受，默认为此方式，若为1，表示在保活时间内，设备重复注册，不接受，回复设备403重复注册错误码
}NET_EHOME_SET_REREGISTER_MODE, *LPNET_EHOME_SET_REREGISTER_MODE;

typedef struct tagNET_EHOME_REGISTER_LISTEN_MODE
{
    DWORD dwSize;
    DWORD dwRegisterListenMode;    //0-表示监听TCP和UDP，1表示监听UDP，2表示监听TCP;
    BYTE  byRes[128];
}NET_EHOME_REGISTER_LISTEN_MODE, *LPNET_EHOME_REGISTER_LISTEN_MODE;

typedef struct tagNET_EHOME_LOCAL_GENERAL_CFG
{
    BYTE byAlarmPictureSeparate;    //控制透传ISAPI报警数据和图片是否分离，0-不分离，1-分离（分离后走EHOME_ISAPI_ALARM回调返回）
    BYTE byRes[127];                //保留
}NET_EHOME_LOCAL_GENERAL_CFG, *LPNET_EHOME_LOCAL_GENERAL_CFG;

typedef struct tagNET_EHOME_LOCAL_PLAYBACK_PARAM
{
    DWORD dwSize;
    BYTE  byPlayBackSync;    //是否开启回放同步接收，0-表示使用异步方式，1-表示使用同步方式;
    BYTE  byRes[131];
}NET_EHOME_LOCAL_PLAYBACK_PARAM, *LPNET_EHOME_LOCAL_PLAYBACK_PARAM;

typedef struct tagNET_EHOME_LOCAL_DEV_PINGREO
{
    DWORD   dwSize;
    BYTE    byEnablePingReoCallback;    //0- 不回调，1-回调心跳, 默认为0
    BYTE    byRes[63];
}NET_EHOME_LOCAL_DEV_PINGREO, *LPNET_EHOME_LOCAL_DEV_PINGREO;

#endif //_HC_EHOME_PUBLIC_H_
