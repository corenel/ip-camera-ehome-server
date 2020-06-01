#ifndef _HC_EHOME_SS_H_
#define _HC_EHOME_SS_H_

#include "HCISUPPublic.h"

#define MAX_KMS_USER_LEN    512 //KMS用户名最大长度
#define MAX_KMS_PWD_LEN     512 //KMS密码最大长度
#define MAX_CLOUD_AK_SK_LEN 64  //EHome5.0存储协议AK SK最大长度
#define SS_CLIENT_FILE_PATH_PARAM_NAME "File-Path"
#define SS_CLIENT_VRB_FILENAME_CODE "Filename-Code"
#define SS_CLIENT_KMS_USER_NAME "KMS-Username"
#define SS_CLIENT_KMS_PASSWIRD "KMS-Password"
#define SS_CLIENT_CLOUD_AK_NAME "Access-Key"
#define SS_CLIENT_CLOUD_SK_NAME "Secret-Key"
#define SS_CLIENT_CLOUD_POOL_ID "Pool-Id"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

enum NET_EHOME_SS_MSG_TYPE
{
    NET_EHOME_SS_MSG_TOMCAT = 1,       //Tomcat回调函数
    NET_EHOME_SS_MSG_KMS_USER_PWD,     //KMS用户名密码校验
    NET_EHOME_SS_MSG_CLOUD_AK          //EHome5.0存储协议AK回调
};

enum NET_EHOME_SS_CLIENT_TYPE
{
    NET_EHOME_SS_CLIENT_TYPE_TOMCAT = 1,       //Tomcat图片上传客户端
    NET_EHOME_SS_CLIENT_TYPE_VRB,              //VRB图片上传客户端
    NET_EHOME_SS_CLIENT_TYPE_KMS,              //KMS图片上传客户端
    NET_EHOME_SS_CLIENT_TYPE_CLOUD             //EHome5.0存储协议客户端
};

//Tomcat图片服务器回调信息
typedef struct tagNET_EHOME_SS_TOMCAT_MSG
{
    char szDevUri[MAX_URL_LEN_SS];         //设备请求的URI
    DWORD dwPicNum;                         //图片数
    char* pPicURLs;                         //图片URL,每个URL MAX_URL_LEN_SS长度
    BYTE  byRes[64];
}NET_EHOME_SS_TOMCAT_MSG, *LPNET_EHOME_SS_TOMCAT_MSG;

//读写扩展回调函数结构体
typedef struct tagNET_EHOME_SS_RW_PARAM
{
    const char* pFileName;   //文件名
    void* pFileBuf;          //文件内容
    DWORD* dwFileLen;   //文件大小
    const char* pFileUrl;    //文件url
    void* pUser;             //
    BYTE byAct;         //读写操作：0-写文件，1-读文件
    BYTE  byRes[63];
}NET_EHOME_SS_RW_PARAM, *LPNET_EHOME_SS_RW_PARAM;

//云存储结构体
typedef struct tagNET_EHOME_SS_CLOUD_PARAM
{
    const char*  pPoolId;          //poolId 资源池id
    BYTE byPoolIdLength;   //poolid 长度
    int dwErrorCode;     //云存储错误码
    BYTE  byRes[503];
}NET_EHOME_SS_CLOUD_PARAM, *LPNET_EHOME_SS_CLOUD_PARAM;

//KMS结构体
typedef struct tagNET_EHOME_SS_KMS_PARAM
{
    BYTE  byRes[512];
}NET_EHOME_SS_KMS_PARAM, *LPNET_EHOME_SS_KMS_PARAM;

//TOMCAT结构体
typedef struct tagNET_EHOME_SS_TOMCAT_PARAM
{
    BYTE  byRes[512];
}NET_EHOME_SS_TOMCAT_PARAM, *LPNET_EHOME_SS_TOMCAT_PARAM;

//VRB结构体
typedef struct tagNET_EHOME_SS_VRB_PARAM
{
    BYTE  byRes[512];
}NET_EHOME_SS_VRB_PARAM, *LPNET_EHOME_SS_VRB_PARAM;

//回调函数返回的扩展结构体
typedef struct tagNET_EHOME_SS_EX_PARAM
{
    BYTE byProtoType;  //存储协议类型 1-云存储，2-tomcat，3-kms，4-vrb
    BYTE byRes[23];
    union
    {
        NET_EHOME_SS_CLOUD_PARAM struCloud;
        NET_EHOME_SS_TOMCAT_PARAM struTomcat;
        NET_EHOME_SS_KMS_PARAM struKms;
        NET_EHOME_SS_VRB_PARAM struVrb;
    }unionStoreInfo;
}NET_EHOME_SS_EX_PARAM, *LPNET_EHOME_SS_EX_PARAM;

//信息回调函数
typedef BOOL(CALLBACK *EHomeSSMsgCallBack)(LONG iHandle, NET_EHOME_SS_MSG_TYPE enumType
    , void *pOutBuffer, DWORD dwOutLen, void *pInBuffer, DWORD dwInLen, void *pUser);

//存储回调函数
typedef BOOL(CALLBACK *EHomeSSStorageCallBack)(LONG iHandle, const char* pFileName, void *pFileBuf, DWORD dwFileLen, char *pFilePath, void *pUser);


//读写回调函数 byAct 0-写 1-读 2-删
typedef BOOL(CALLBACK *EHomeSSRWCallBack)(LONG iHandle, BYTE byAct, const char* pFileName
    , void *pFileBuf, LONG* dwFileLen, const char* pFileUrl, void *pUser);

//读写回调函数扩展 byAct 0-写 1-读 2-删  pRwParam参数用于传递四种存储协议共用字段，pExStruct用于传递存储协议特有字段
typedef BOOL(CALLBACK *EHomeSSRWCallBackEx)(LONG iHandle, NET_EHOME_SS_RW_PARAM* pRwParam, NET_EHOME_SS_EX_PARAM* pExStruct);

//监听参数
typedef struct tagNET_EHOME_SS_LISTEN_PARAM
{
    NET_EHOME_IPADDRESS struAddress;  //本地监听信息，IP为0.0.0.0的情况下，默认为本地地址，多个网卡的情况下，默认为从操作系统获取到的第一个
    char szKMS_UserName[MAX_KMS_USER_LEN];//KMS用户名
    char szKMS_Password[MAX_KMS_PWD_LEN];//KMS密码
    EHomeSSStorageCallBack fnSStorageCb; //图片服务器信息存储回调函数
    EHomeSSMsgCallBack    fnSSMsgCb; //图片服务器信息Tomcat回调函数
    char szAccessKey[MAX_CLOUD_AK_SK_LEN]; //EHome5.0存储协议AK
    char szSecretKey[MAX_CLOUD_AK_SK_LEN]; //EHome5.0存储协议SK
    void * pUserData;   //用户数据
    BYTE  byHttps;                 //是否启用HTTPs
    BYTE  byRes1[3];
    EHomeSSRWCallBack fnSSRWCb;         //读写回调函数
    EHomeSSRWCallBackEx fnSSRWCbEx;         //读写回调函数扩展
    BYTE bySecurityMode; // [add] by yangzheng 2020/03/13 存储服务器新增安全模式开关，默认开启，0-开启，1-关闭
    BYTE  byRes[51];
}NET_EHOME_SS_LISTEN_PARAM, *LPNET_EHOME_SS_LISTEN_PARAM;


//监听Https参数
typedef struct tagNET_EHOME_SS_LISTEN_HTTPS_PARAM
{
    BYTE byHttps;      //0-不启用HTTPS 1-启用HTTPS
    BYTE byVerifyMode; //0-单向认证(暂只支持单向认证)
    BYTE byCertificateFileType; //证书类型0-pem, 1-ANS1
    BYTE byPrivateKeyFileType;  //私钥类型0-pem, 1-ANS1
    char szUserCertificateFile[MAX_PATH]; //公钥证书文件路径
    char szUserPrivateKeyFile[MAX_PATH];  //私钥证书文件路径

    DWORD dwSSLVersion; //SSL Method版本
    //0 - SSL23, 1 - SSL2, 2 - SSL3, 3 - TLS1.0, 4 - TLS1.1, 5 - TLS1.2
    //SSL23是兼容模式，会协商客户端和服务端使用的最高版本

    BYTE byRes3[360];
}NET_EHOME_SS_LISTEN_HTTPS_PARAM, *LPNET_EHOME_SS_LISTEN_HTTPS_PARAM;

//图片上传客户端参数
typedef struct tagNET_EHOME_SS_CLIENT_PARAM
{
    NET_EHOME_SS_CLIENT_TYPE enumType; //图片上传客户端类型
    NET_EHOME_IPADDRESS struAddress;    //图片服务器地址
    BYTE  byHttps;                 //是否启用HTTPs
    BYTE  byRes[63];
}NET_EHOME_SS_CLIENT_PARAM, *LPNET_EHOME_SS_CLIENT_PARAM;

enum NET_EHOME_SS_INIT_CFG_TYPE
{
    NET_EHOME_SS_INIT_CFG_SDK_PATH = 1,            //图片存储数据库路径设置，路径请以双斜杠\\结尾
    NET_EHOME_SS_INIT_CFG_CLOUD_TIME_DIFF,         //设置运存储的请求时间差值,默认15分钟,最小5分钟,最大60分钟
    NET_EHOME_SS_INIT_CFG_PUBLIC_IP_PORT,           //设置公网地址(当存在内外网映射时)
    NET_EHOME_SS_INIT_CFG_LIBEAY_PATH,              //设置OpenSSL的加密库路径
    NET_EHOME_SS_INIT_CFG_SSLEAY_PATH               //设置OpenSSL的通信库路径
};

typedef struct tagNET_EHOME_SS_LOCAL_SDK_PATH{
    char    sPath[MAX_PATH];
    BYTE    byRes[128];
}NET_EHOME_SS_LOCAL_SDK_PATH, *LPNET_EHOME_SS_LOCAL_SDK_PATH;

enum NET_EHOME_SS_TYPE
{
    NET_EHOME_SS_TYPE_TOMCAT = 1,       //Tomcat
    NET_EHOME_SS_TYPE_VRB,              //VRB
    NET_EHOME_SS_TYPE_KMS,              //KMS
    NET_EHOME_SS_TYPE_CLOUD             //EHome5.0存储协议
};

//图片URI
typedef struct tagNET_EHOME_SS_STORAGE_URI
{
    NET_EHOME_SS_TYPE enumType;         //[in]图片类型
    char  szFilename[256];         // [in]输入的文件标识
    char  szUri[MAX_URL_LEN_SS];   // [out]生成的URL
    BYTE  byRes[64];
}NET_EHOME_SS_STORAGE_URI, *LPNET_EHOME_SS_STORAGE_URI;

//初始化，反初始化
NET_DVR_API BOOL  CALLBACK NET_ESS_Init();
NET_DVR_API BOOL  CALLBACK NET_ESS_Fini();
//获取错误码
NET_DVR_API DWORD CALLBACK NET_ESS_GetLastError();

//日志
NET_DVR_API BOOL CALLBACK NET_ESS_SetLogToFile(LONG iLogLevel, const char *strLogDir, BOOL bAutoDel);

//获取版本号
NET_DVR_API DWORD CALLBACK NET_ESS_GetBuildVersion();

//设置HTTP监听的Https参数
NET_DVR_API BOOL CALLBACK NET_ESS_SetListenHttpsParam(NET_EHOME_SS_LISTEN_HTTPS_PARAM* pSSHttpsParam);

//生成图片URI
NET_DVR_API BOOL CALLBACK NET_ESS_GenerateStorageUri(NET_EHOME_SS_STORAGE_URI* pSSStorageUri);

//开启监听
NET_DVR_API LONG CALLBACK NET_ESS_StartListen(NET_EHOME_SS_LISTEN_PARAM* pSSListenParam);
//关闭监听
NET_DVR_API BOOL  CALLBACK NET_ESS_StopListen(LONG lListenHandle);

//设置初始化参数
NET_DVR_API BOOL CALLBACK NET_ESS_SetSDKInitCfg(NET_EHOME_SS_INIT_CFG_TYPE enumType, void* const lpInBuff);

//创建图片上传/下载客户端
NET_DVR_API LONG CALLBACK NET_ESS_CreateClient(NET_EHOME_SS_CLIENT_PARAM* pClientParam);

//设置图片上传/下载客户端超时时间,单位ms,默认为5s
NET_DVR_API BOOL CALLBACK NET_ESS_ClientSetTimeout(LONG lHandle, DWORD dwSendTimeout, DWORD dwRecvTimeout);

//设置图片上传/下载客户端参数
NET_DVR_API BOOL CALLBACK NET_ESS_ClientSetParam(LONG lHandle, const char* strParamName, const char* strParamVal);

//图片上传/下载客户端执行上传
NET_DVR_API BOOL CALLBACK NET_ESS_ClientDoUpload(LONG lHandle, char* strUrl, LONG dwUrlLen);

//图片上传/下载客户端执行下载
NET_DVR_API BOOL CALLBACK NET_ESS_ClientDoDownload(LONG lHandle, char* strUrl, void** pFileContent, DWORD& dwContentLen);

//销毁客户端
NET_DVR_API BOOL CALLBACK NET_ESS_DestroyClient(LONG lHandle);

//计算HMAC-SHA256
NET_DVR_API BOOL CALLBACK NET_ESS_HAMSHA256(const char* pSrc, const char* pSecretKey, char* pSingatureOut, DWORD dwSingatureLen);

//计算加密秘钥
NET_DVR_API BOOL CALLBACK NET_ESS_GENERATE_SECRETKEY(const char* pSrc, const char* pKey, char* pSecretKeyOut, DWORD dwSecretKeyLen);

//图片上传客户端执行删除
NET_DVR_API BOOL CALLBACK NET_ESS_ClientDoDelete(LONG lHandle, char* strUrl);

//图片上传客户端执行上传使用图片buffer
NET_DVR_API BOOL CALLBACK NET_ESS_ClientDoUploadBuffer(LONG lHandle, char* strUrl, DWORD dwUrlLen, void*pFileContent, DWORD dwContentLen);
#endif //_HC_EHOME_SS_H_

