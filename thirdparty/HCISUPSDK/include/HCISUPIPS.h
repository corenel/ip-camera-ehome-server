#ifndef _HC_EHOME_IPS_H_
#define _HC_EHOME_IPS_H_

#include "HCISUPPublic.h"


#define   MAX_INFO_PUBLISH_LISTEN_NUM       10    //最大信息发布服务器监听路数
#define   MAX_FILE_PATH  260  //最大文件路径长度

/* 升级状态 */
#define	NET_SDK_UPG_STATUS_SUCCESS		1000   //代表升级成功
#define NET_SDK_UPG_STATUS_PROCESSING	1001   //代表正在升级
#define	NET_SDK_UPG_STATUS_FAILED		1002   //代表升级失败
#define NET_SDK_UPG_EXTRACT_FAILED      1003    //代表解压缩失败升级失败
#define NET_SDK_UPG_APK_VERSION_FAILED  1004    //代表APK版本不匹配升级失败
#define NET_SDK_UPG_ROM_VERSION_FAILED  1005   //代表ROM版本不匹配升级失败
#define NET_SDK_UPG_PKG_VER_FAIL        1006   //非升级包
#define NET_SDK_UPG_SIGN_FAILED         1007   //非签名升级包
#define NET_SDK_UPG_PKG_VER_SUCCESS     1008   //升级包校验成功

//信息发布交互状态
#define	NET_SDK_INFO_PUBLISH_PROCESSING		 1   //信息发布中
#define	NET_SDK_INFO_PUBLISH_FAILED		 2   //信息发布失败
#define NET_SDK_INFO_PUBLISH_NO_MEMORY   3    //存储空间不足
#define NET_SDK_INFO_PUBLISH_CANCEL      4    //取消日程发布
#define NET_SDK_INFO_PUBLISH_LEN_FAILED  6   //接收长度失败
#define NET_SDK_INFO_PUBLISH_DATA_FAILED 7   //接收数据失败
#define NET_SDK_INFO_PUBLISH_SEND_FAILED 8   //发送数据失败
//日程发布文件头
typedef struct
{
    DWORD dwType;    /* 1-日程，2-节目，3-素材*/
    DWORD dwId;       /* 日程号，或节目号，或素材号 */
    DWORD dwIndex;     /* 日程索引，或节目索引，或素材索引：用于标识某个日程或节目或素材是否改变 */
    BYTE  byIdentifyCode[32]; /* 终端的特征码 */
    DWORD dwTimingPlanId; /*定时日程id*/
    BYTE  byRes[16];
}NET_EHOME_SCHEDULE_PUBLISH_HEAD;

//日程发布单文件 
typedef struct
{
    DWORD dwFileType;    /* 1-日程xml，2-节目xml，3-素材xml，4-素材内容，5-素材文件发送完成 6-xml发送完成 7-页面-xml*/
    DWORD dwId;      /* 日程号，或节目号，或素材号 */
    DWORD dwPageId;       /* 页面号 （根据节目号生成唯一id）*/
    DWORD dwIndex;     /* 日程索引，或节目索引，或素材索引：用于标识某个日程或节目或素材是否改变 */
    DWORD dwFileLen; /* 日程xml长度，或节目xml长度，或素材xml长度，或素材内容长度 */
    BYTE  byRes[12];
    char * pXMLData;     // xml数据（dwFileType为1、2、3时有效）
    char  szFilePath[MAX_FILE_PATH];     // 素材文件路径（dwFileType为4时有效）
}NET_EHOME_PUBLISH_SINGLE_FILE;

//日程发布文件 
typedef struct
{
    WORD wFileNumber;  // 文件数量
    BYTE  byRes[2];
    char * pFilesData;     // 文件数据，wFileNumber个NET_EHOME_PUBLISH_SINGLE_FILE
    BYTE  byRes1[32];
}NET_EHOME_SCHEDULE_PUBLISH_FILE;

typedef BOOL(CALLBACK * INFO_PUBLISH_CB)(LONG iLinkHandle,DWORD dwDataType, void *pOutBuffer, DWORD dwOutLen,
    void *pInBuffer, DWORD dwInLen, void *pUser);

typedef struct{
    NET_EHOME_IPADDRESS    struIPAddress;//本地监听信息，IP为0.0.0.0的情况下，默认为本地地址，多个网卡的情况下，默认为从操作系统获取到的第一个
    INFO_PUBLISH_CB        fnCB;//信息回调函数
    void                   *pUser;  //用户数据
    BYTE                   byType;//0-保留,1-升级(因为升级交互，终端请求时没有带命令码，服务器无法区分，因此增加该字段)
    BYTE                   byRes[31];
}NET_EHOME_INFO_PUBLISH_PARAM, *LPNET_EHOME_INFO_PUBLISH_PARAM;

//终端截屏结构体
typedef struct{
    DWORD dwSeq;//截屏索引
    DWORD dwScreenPicLen;//截屏图片大小
    char *pScreenPic;  //截屏图片数据
    BYTE                   byRes[32];
}NET_EHOME_SCREEN_SHOT_PARAM, *LPNET_EHOME_SCREEN_SHOT_PARAM;

//终端升级信息
typedef struct{
    char sUpgradeFilePath[MAX_FILE_PATH]; //升级文件的路径名
    BYTE byRes[32]; //保留
}NET_EHOME_UPGRADE_INFO, *LPNET_EHOME_UPGRADE_INFO;

//终端升级状态
typedef struct{
    DWORD dwSize;//结构体大小
    DWORD dwStatus;//升级状态
    DWORD dwPercent;//升级进度
}NET_EHOME_UPGRADE_STATUS, *LPNET_EHOME_UPGRADE_STATUS;


//信息发布
typedef enum tagNET_EHOME_INFO_PUBLISH_TYPE{
    ENUM_SCHEDULE_PUBLICH = 0,             //日程发布
    ENUM_SCREEN_SHOT,                //终端屏幕截图
    ENUM_DEV_UPGRADE,    //终端升级
}NET_EHOME_INFO_PUBLISH_TYPE;

//初始化，反初始化
NET_DVR_API BOOL  CALLBACK NET_EIPS_Init();
NET_DVR_API BOOL  CALLBACK NET_EIPS_Fini();

//获取错误码
NET_DVR_API DWORD CALLBACK NET_EIPS_GetLastError();

//日志
NET_DVR_API BOOL CALLBACK NET_EIPS_SetLogToFile(LONG iLogLevel, const char *strLogDir, BOOL bAutoDel);

//信息发布监听
NET_DVR_API LONG CALLBACK NET_EIPS_InfoPublishListen(LPNET_EHOME_INFO_PUBLISH_PARAM lpInfoPublishPara);
NET_DVR_API BOOL CALLBACK NET_EIPS_StopInfoPublisListen(LONG lHandle);
NET_DVR_API BOOL CALLBACK NET_EIPS_GetProgress(LONG lHandle, LONG iLinkHandle, LPDWORD pProgress);
NET_DVR_API BOOL CALLBACK NET_EIPS_GetUploadState(LONG lHandle, LONG iLinkHandle, LPDWORD pStatus);

#endif //_HC_EHOME_IPS_H_
