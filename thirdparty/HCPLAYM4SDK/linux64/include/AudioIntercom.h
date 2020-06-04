/**	@File   AUDIOINTERCOM.H
 *
 *	@Note   HangZhou Hikvision System Technology Co., Ltd. All Right Reserved.
 *
 *	@Brief  音频对讲库
 *
 *	@Author zhangxinyf3 张欣
 *
 *	@Date   2012/4/9
 *
 *  @Remark 
 */

#ifndef _AUDIO_INTERCOM_H_
#define _AUDIO_INTERCOM_H_

#ifdef WIN32
    #if defined(_WINDLL)
        #define AUDIOCOM_API  __declspec(dllexport) 
    #else 
        #define AUDIOCOM_API  __declspec(dllimport) 
    #endif
#else
    #ifndef __stdcall
        #define __stdcall
    #endif

    #ifndef AUDIOCOM_API
        #define AUDIOCOM_API
    #endif
#endif

///<错误码
#define ERROR_NO            1      ///<无错误
#define ERROR_ALLOC_MEMORY  2      ///<内存分配出错
#define ERROR_PARAMETER     3      ///<调用参数出错
#define ERROR_CALL_ORDER    4      ///<调用顺序出错
#define ERROR_FIND_DEVICE   5      ///<不能找到声卡设备
#define ERROR_OPEN_DEVICE   6      ///<不能打开声卡设备
#define ERROR_NO_CONTEXT    7      ///<设备音频引擎上下文出错
#define ERROR_NO_WAVEFILE   8      ///<WAV文件出错
#define ERROR_INVALID_TYPE  9      ///<无效的WAV参数类型
#define ERROR_ENCODE_FAIL   10     ///<音频编码失败
#define ERROR_DECODE_FAIL   11     ///<音频解码失败
#define ERROR_NO_PLAYBACK   12     ///<音频播放失败
#define ERROR_DENOISE_FAIL  13     ///<降噪失败
#define ERROR_SUPPORT       14     ///<类型不支持
#define ERROR_UNKNOWN       99     ///<未知错误

///<声卡设备信息结构体
typedef struct _SOUND_CARD_INFO
{
    char           byDeviceName[128];///<设备名称
    unsigned int   dwFrequency;      ///<采集频率
    unsigned int   dwRefresh;        ///<刷新频率
    unsigned int   dwSync;           ///<同步
    unsigned int   dwMonoSources;    ///<单声道源数量
    unsigned int   dwStereoSources;  ///<多声道源数量
    unsigned int   dwMajorVersion;   ///<主版本号
    unsigned int   dwMinorVersion;   ///<次版本号
    unsigned int   dwReserved[16];   ///<保留参数
}SoundCardInfo;

///<pcm 声道数
typedef enum _CHANNEL_PCM_
{
    CHANNEL_1 = 1,                  ///<单声道
    CHANNEL_2 = 2                   ///<双声道
}ChannelPcm;

///<位宽
typedef enum _BITWIDTH_PCM_
{
    BITS_08 = 8,                    ///<编解码库不支持
    BITS_16 = 16                    ///<16位
}BitsPcm;

///<pcm采样率
typedef enum _SAMPLERATE_PCM_
{
    SAMPLERATE_08K  =  8000,         ///<8k采样率
    SAMPLERATE_16K  = 16000,         ///<16k采样率
    SAMPLERATE_32K  = 32000,         ///<32k采样率
    SAMPLERATE_44K1 = 44100,         ///<44.1k采样率
    SAMPLERATE_48K  = 48000          ///<48k采样率
}SampleratePcm;

///<编码比特率
typedef enum _BITRATE_ENCODE_
{
    BITRATE_ENCODE_08k = 8000,      ///<8k比特率
    BITRATE_ENCODE_16k = 16000,     ///<16k比特率
    BITRATE_ENCODE_32k = 32000,     ///<32k比特率
	BITRATE_ENCODE_64k = 64000,     ///<64k比特率
	BITRATE_ENCODE_128k = 128000    ///<128k比特率
	
}BitRateEncode;

///<音频编解码类别
typedef enum _AUDIO_ENCODE_TYPE_EX
{
    AUDIO_TYPE_PCM    = 0x00,
    AUDIO_TYPE_G711A  = 0x01,
    AUDIO_TYPE_G711U  = 0x02,
    AUDIO_TYPE_G722   = 0x03,
    AUDIO_TYPE_G726   = 0x04,
    AUDIO_TYPE_MPEG2  = 0x05,
    AUDIO_TYPE_AAC    = 0x06,
    AUDIO_TYPE_G729   = 0x07,
    AUDIO_TYPE_ADPCM  = 0x08,
    AUDIO_TYPE_G722c  = 0x09,
    AUDIO_TYPE_MP3    = 0x0A
}AudioEncodeTypeEx;

typedef enum _AUDIO_ENCODE_TYPE
{
    AUDIO_TYPE_PCM_S16K    = 0x00,
    AUDIO_TYPE_G711A_S8K   = 0x01,
    AUDIO_TYPE_G711U_S8K   = 0x02,
    AUDIO_TYPE_G722_S16K   = 0x03,
    AUDIO_TYPE_G726_S8K    = 0x04,
    AUDIO_TYPE_MPEG2_S16K  = 0x05,
    AUDIO_TYPE_AAC_S32K    = 0x06,
    AUDIO_TYPE_PCM_S8K	   = 0x07,
    AUDIO_TYPE_PCM_S32K    = 0x08,
    AUDIO_TYPE_AAC_S16K	   = 0x09,
	AUDIO_TYPE_MP3_S16K  = 0x0A
}AudioEncodeType;

///<音频编解码格式(注：pcm三属性值、编码比特率、编解码类别不是任意组合都可以的)
typedef struct _AUDIO_PARAM_
{
    unsigned short    nChannel;           ///<PCM声道数
    unsigned short    nBitWidth;          ///<PCM位宽
    unsigned int      nSampleRate;        ///<PCM采样率
    unsigned int      nBitRate;           ///<编码比特率
    AudioEncodeTypeEx enAudioEncodeTypeEx;///<编解码类别
}AudioParam;

///<原音频编解码格式
/*****************************************************************
原音频枚举类型            声道, 位宽, 采样率, 编码比特率, 编码类别  
AUDIO_TYPE_MPEG2_S16K <――> {1, 16, 16000, 64000, AUDIO_TYPE_MPEG2}
AUDIO_TYPE_MP3_S16K    <――> {1, 16, 16000, 64000, AUDIO_TYPE_MP3}
AUDIO_TYPE_G726_S8K   <――> {1, 16,  8000, 16000,  AUDIO_TYPE_G726}
AUDIO_TYPE_G722_S16K  <――> {1, 16, 16000, 16000,  AUDIO_TYPE_G722}
AUDIO_TYPE_G711U_S8K  <――> {1, 16,  8000, 64000, AUDIO_TYPE_G711U}
AUDIO_TYPE_G711A_S8K  <――> {1, 16,  8000, 64000, AUDIO_TYPE_G711A}
AUDIO_TYPE_PCM_S32K   <――> {1, 16, 32000, xxxxx,   AUDIO_TYPE_PCM}
AUDIO_TYPE_PCM_S8K    <――> {1, 16,  8000, xxxxx,   AUDIO_TYPE_PCM}
AUDIO_TYPE_AAC_S16K   <――> {1, 16, 16000, 32000,   AUDIO_TYPE_AAC}
AUDIO_TYPE_AAC_S32K   <――> {1, 16, 32000, 32000,   AUDIO_TYPE_AAC}
*****************************************************************/

///<回调函数输出数据信息结构体
typedef struct _OUTPUT_DATA_INFO_EX_ 
{
    unsigned char*    pData;
    unsigned int      dwDataLen;
    AudioEncodeTypeEx enDataTypeEx;
}OutputDataInfoEx;

typedef struct _OUTPUT_DATA_INFO_
{
    unsigned char*    pData;
    unsigned int      dwDataLen;
    AudioEncodeType   enDataType;
}OutputDataInfo;

typedef void (__stdcall* OutputDataCallBack)(OutputDataInfo* pstDataInfo,void* pUser);
typedef void (__stdcall* OutputDataCallBackEx)(OutputDataInfoEx* pstDataInfo,void* pUser);


#ifdef __cplusplus
extern "C" {
#endif

/**********************************获取声卡信息*******************************************************************************/

///<获取声卡数量
AUDIOCOM_API int __stdcall AUDIOCOM_GetSoundCardNum(unsigned int* pdwDeviceNum);

///<获取指定声卡的设备信息
AUDIOCOM_API int __stdcall AUDIOCOM_GetOneSoundCardInfo(unsigned int dwDeviceIndex, SoundCardInfo* pstDeviceInfo);


///<获取采集设备的数量
AUDIOCOM_API int __stdcall AUDIOCOM_GetCaptureDeviceNum(unsigned int* pCaptureDeviceNum);

///<获取采集设备的名称
AUDIOCOM_API int __stdcall AUDIOCOM_GetOneCaptureDeviceName(unsigned int dwCaptureDeviceIndex, char* pCaptureDeviceName);


///<获取播放设备的数量
AUDIOCOM_API int __stdcall AUDIOCOM_GetPlayDeviceNum(unsigned int* pPlayDeviceNum);

///<获取播放设备的名称
AUDIOCOM_API int __stdcall AUDIOCOM_GetOnePlayDeviceName(unsigned int dwPlayDeviceIndex, char* pPlayDeviceName);


/**********************************采集编码模块接口***************************************************************************/

///<根据指定设备名称，创建采集编码模块端口号
AUDIOCOM_API int __stdcall AUDIOCOM_CreateCaptureHandle(int* pnCapturePort, const char* pDeviceName);


///<根据指定采集设备名称，创建采集编码模块端口号
AUDIOCOM_API int __stdcall AUDIOCOM_CreateCaptureHandleEx(int* pnCapturePort, const char* pCapDeviceName);


///<注册回调函数，可以指定不同格式的编码数据，并通过回调函数输出(旧接口)
AUDIOCOM_API int __stdcall AUDIOCOM_RegisterOutputDataCallBack(int nCapturePort, 
                                                               AudioEncodeType enDataType, 
                                                               OutputDataCallBack pfnOutputDataCallBack, 
                                                               void* pUser);



///<注册回调函数，可以指定不同格式的编码数据，并通过回调函数输出
AUDIOCOM_API int __stdcall AUDIOCOM_RegisterOutputDataCallBackEx(int nCapturePort, 
                                                                 AudioParam *pstAudioParam, 
                                                                 OutputDataCallBackEx pfnOutputDataCallBack, 
                                                                 void* pUser);

///<开始采集编码
AUDIOCOM_API int __stdcall AUDIOCOM_StartCapture(int nCapturePort);

///<停止采集编码
AUDIOCOM_API int __stdcall AUDIOCOM_StopCapture(int nCapturePort);

///<注册采集编码前回调函数(旧接口)
AUDIOCOM_API int __stdcall AUDIOCOM_RegisterCaptureDataCallBack(int nCapturePort, 
                                                                OutputDataCallBack pfnCaptureDataCallBack, 
                                                                void* pUser);

///<注册采集编码前回调函数
AUDIOCOM_API int __stdcall AUDIOCOM_RegisterCaptureDataCallBackEx(int nCapturePort, 
                                                                  OutputDataCallBackEx pfnCaptureDataCallBack, 
                                                                  void* pUser);

///<释放采集编码模块端口号
AUDIOCOM_API int __stdcall AUDIOCOM_ReleaseCaptureHandle(int nCapturePort);

///<释放采集编码模块端口号
AUDIOCOM_API int __stdcall AUDIOCOM_ReleaseCaptureHandleEx(int nCapturePort);

/**********************************播放解码模块接口**************************************************************************/

///<根据指定设备名称，创建播放解码模块端口号
AUDIOCOM_API int __stdcall AUDIOCOM_CreatePlayHandle(int* pnPlayPort, const char* pDeviceName);


///<根据指定设备名称，创建播放解码模块端口号
AUDIOCOM_API int __stdcall AUDIOCOM_CreatePlayHandleEx(int* pnPlayPort, const char* pDeviceName);
/*
///<指定文件路径，打开WAV文件
AUDIOCOM_API int __stdcall AUDIOCOM_OpenWaveFile(int nPlayPort, const char* pFilePath);
*/

///<指定流类型，打开流
AUDIOCOM_API int __stdcall AUDIOCOM_OpenStream(int nPlayPort, AudioEncodeType enDataType);

///<指定流类型，打开流
AUDIOCOM_API int __stdcall AUDIOCOM_OpenStreamEx(int nPlayPort, AudioParam *pstAudioParam);

///<输入流数据
AUDIOCOM_API int __stdcall AUDIOCOM_InputStreamData(int nPlayPort, unsigned char* pData, unsigned int dwDataLen);

///<开始解码播放
AUDIOCOM_API int __stdcall AUDIOCOM_StartPlay(int nPlayPort);

///<停止解码播放
AUDIOCOM_API int __stdcall AUDIOCOM_StopPlay(int nPlayPort);

///<解码后数据回调(旧接口)
AUDIOCOM_API int __stdcall AUDIOCOM_RegisterDecodeDataCallBack(int nPlayPort, 
                                                               OutputDataCallBack pfnDecodeDataCallBack, 
                                                               void* pUser);

///<解码后数据回调
AUDIOCOM_API int __stdcall AUDIOCOM_RegisterDecodeDataCallBackEx(int nPlayPort, 
                                                                 OutputDataCallBackEx pfnDecodeDataCallBack, 
                                                                 void* pUser);

///<设置音量
AUDIOCOM_API int __stdcall AUDIOCOM_SetVolume(int nPlayPort, float fVolume);

///<获取音量
AUDIOCOM_API int __stdcall AUDIOCOM_GetVolume(int nPlayPort, float* fVolume);

///<释放播放解码模块端口号
AUDIOCOM_API int __stdcall AUDIOCOM_ReleasePlayHandle(int nPlayPort);

///<释放声卡资源
//AUDIOCOM_API int __stdcall AUDIOCOM_Release

/******************************************************************************************************************************/

///<获取错误信息
AUDIOCOM_API int __stdcall AUDIOCOM_GetLastError(int nPort);

///<获取版本号
AUDIOCOM_API unsigned int __stdcall AUDIOCOM_GetVersion();

///<设定采集状态
AUDIOCOM_API int __stdcall AUDIOCOM_SetCaptureStatus(int nCapturePort, bool isPlay);
///<获取剩余播放缓冲
AUDIOCOM_API int __stdcall AUDIOCOM_GetPlayBufferRemian(int nPlayPort, int *nBufferSize);
///<设定播放状态
AUDIOCOM_API int __stdcall AUDIOCOM_SetPlayStatus(int nPlayPort, bool isPlay);



#ifdef __cplusplus
}
#endif


#endif//_AUDIO_INTERCOM_H_