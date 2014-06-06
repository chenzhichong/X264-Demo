/************************************************************************/
/* 
	文件名称：PlaySDK.h
	功　　能：PlaySDK接口文件
	作　　者：hg
	创建时间：2013-08-05
	版　　本：1.0
---------------------------------------------------------------------------
修改记录：
	修改人			修改时间		     	版本		       修改内容
*/
/************************************************************************/
#pragma once


#ifdef PLAYSDK_DLL

#ifdef PLAYSDK_EXPORTS
#define DllAPI __declspec(dllexport) 
#else 
#define DllAPI __declspec(dllimport) 
#endif

#else
#define DllAPI
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#define STDMETHOD_INT DllAPI int __stdcall


#ifdef		__cplusplus
extern "C"
{
#endif

#pragma pack(1)

typedef enum E_StreamMode
{
	REALTIME_STREAM = 0,			//实时流
	FILE_STREAM						//文件流
};

typedef enum E_MediaType
{
	MEDIA_VIDEO = 0,				//视频流
	MEDIA_AUDIO = 1,				//音频流
	MEDIA_MIX						//混合流
};

typedef	struct  ST_VideoAttribute
{
	BITMAPINFO	BmpInfo;			//帧信息
}*PST_VideoAttribute;

typedef struct ST_AudioAttribute
{
	int			Volume;				//声音大小
}*PST_AudioAttribute;

typedef struct U_MediaAttribute
{
	E_MediaType	MediaType;				//数据流类型
	ST_VideoAttribute VideoAttribute;	//视频属性
	ST_AudioAttribute AudioAttribute;	//音频属性
}*PU_MediaAttribute;

typedef void(__stdcall *AUDIOTALLBACKCALLBACKFUN)(char *pData, int Length, void *pParam);


//接口定义部分
//////////////////////////////////////////////////////////////////////////////////////////
/*
*	Initialize 说明
*	功能: 初始化
*	参数：无
*	返回值: 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_Initialize();

/*
*	UnInitialize 说明
*	功能: 反初始化，释放资源
*	参数：
		1、Port：输入参数，表示数据播放端口
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_UnInitialize();

/*
*	GetPlayPort 说明
*	功能: 获取数据播放端口
*	参数：
		1、Port：输出参数，表示数据播放端口
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_GetPlayPort(long *pPort);

/*
*	OpenPlayer 说明
*	功能: 打开播放器
*	参数：
		1、Port：输入参数，表示数据播放端口
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_OpenPlayer(int Port);

/*
*	ClosePlayer 说明
*	功能: 关闭播放器
*	参数：
		1、Port：输入参数，表示数据播放端口
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_ClosePlayer(int Port);

/*
*	PlayMedia 说明
*	功能: 开始播放数据
*	参数：
		1、Port：输入参数，表示数据播放端口； 
		2、hWnd：输入参数， 表示数据播放窗口句柄;
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_PlayMedia(int Port, HWND hWnd);

/*
*	StopMedia 说明
*	功能: 停止播放数据
*	参数：
		1、Port：输入参数，表示数据播放端口； 
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_StopMedia(int Port);

/*
*	SetMediaAttribute 说明
*	功能: 设置数据属性
*	参数：
		1、Port：输入参数，表示数据播放端口；
		2、MediaType：输入参数，表示数据类型;
		3、pMediaAttribute：输入参数，表示数据属性;
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_SetMediaAttribute(int Port, U_MediaAttribute MediaAttribute);

/*
*	GetMediaAttribute 说明
*	功能: 获取数据属性
*	参数：
		1、Port：输入参数，表示数据播放端口；
		2、MediaType：输出参数，表示当前数据类型;
		3、pMediaAttribute：输出参数，表示当前数据属性;
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_GetMediaAttribute(int Port, U_MediaAttribute *pMediaAttribute);

/*
*	SetStreamOpenMod 说明
*	功能: 设置数据流模式（1、实时流； 2、文件流）
*	参数：
		1、Port：输入参数，表示数据播放端口；
		2、StreamMode:输入参数，表示数据流模式；
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_SetStreamOpenMod(int Port, E_StreamMode StreamMode);

/*
*	GetStreamOpenMod 说明
*	功能: 获取数据流模式
*	参数：
		1、Port：输入参数，表示数据播放端口；
		2、StreamMode:输出参数，表示数据流模式；
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_GetStreamOpenMod(int Port, E_StreamMode *pStreamMode);

/*
*	ProcessVideoData 说明
*	功能: 处理实时视频流
*	参数：
		1、Port：输入参数，表示数据播放端口；
		2、pData:输入参数，表示实时视频数据；
		3、Length：输入参数，表示实时数据长度；
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_ProcessVideoData(int Port, const char *pData, int Length);

/*
*	ProcessAudioData 说明
*	功能: 处理实时音频流
*	参数：
		1、Port：输入参数，表示数据播放端口；
		2、pData:输入参数，表示实时音频数据；
		3、Length：输入参数，表示实时数据长度；
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_ProcessAudioData(int Port, const char *pData, int Length);

/*
*	ProcessMixData 说明
*	功能: 处理实时混合流
*	参数：
		1、Port：输入参数，表示数据播放端口；
		2、pData:输入参数，表示实时混合流数据；
		3、Length：输入参数，表示实时数据长度；
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_ProcessMixData(int Port, const char *pData, int Length);

/*
*	StartRecord 说明
*	功能: 开始录像
*	参数：
		1、Port：输入参数，表示数据播放端口；
		2、pFilePath:输入参数，表示录像文件保存全路径（包含文件名）；
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_StartRecord(int Port, const char *pFilePath);

/*
*	StopRecord 说明
*	功能: 停止录像
*	参数：
		1、Port：输入参数，表示数据播放端口；
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_StopRecord(int Port);

/*
*	SnapShot 说明
*	功能: 截图
*	参数：
		1、Port：输入参数，表示数据播放端口；
		2、pFilePath:输入参数，表示截图文件保存全路径（包含文件名）；
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_SnapShot(int Port, const char *pFilePath);

/*
*	OpenStreamFile 说明
*	功能: 打开数据流文件
*	参数：
		1、Port：输入参数，表示数据播放端口；
		2、pFilePath:输入参数，表示文件流文件路径；
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_OpenStreamFile(int Port, const char *pFilePath);

/*
*	CloseStreamFile 说明
*	功能: 关闭数据流文件
*	参数：
		1、Port：输入参数，表示数据播放端口；
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_CloseStreamFile(int Port);

/*
*	PauseStreamFile 说明
*	功能: 暂停播放数据流文件
*	参数：
		1、Port：输入参数，表示数据播放端口；
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_PauseStreamFile(int Port);

/*
*	StepStreamFile 说明
*	功能: 单步播放数据流文件
*	参数：
		1、Port：输入参数，表示数据播放端口；
		2、Next：输入参数，表示是否执行下一步(0:向前一步，1向后一步)；
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_StepStreamFile(int Port, unsigned char Next);

/*
*	SetFilePlaySpeed 说明
*	功能: 设置文件播放速度
*	参数：
		1、Port：输入参数，表示数据播放端口；
		2、nSpeed：输入参数，表示文件播放速度(1~100)；
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_SetFilePlaySpeed(int Port, int nSpeed);

/*
*	GetFilePlaySpeed 说明
*	功能: 获取文件播放速度
*	参数：
		1、Port：输入参数，表示数据播放端口；
		2、nSpeed：输出参数，表示文件播放速度(1~100)；
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_GetFilePlaySpeed(int Port, int *pSpeed);

/*
*	SetFilePlayPos 说明
*	功能: 设置文件播放位置
*	参数：
		1、Port：输入参数，表示数据播放端口；
		2、PlayTime：输入参数，表示设置的播放时间；
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_SetFilePlayPos(int Port, DWORD PlayTime);

/*
*	GetFilePlayTime 说明
*	功能: 获取文件播放时间点
*	参数：
		1、Port：输入参数，表示数据播放端口；
		2、pPlayTime：输出参数，表示文件已播放时间；
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_GetFilePlayTime(int Port, DWORD *pPlayTime);

/*
*	GetFileTotleTime 说明
*	功能: 获取文件总的播放时间
*	参数：
		1、Port：输入参数，表示数据播放端口；
		2、pPlayTime：输出参数，表示文件总的播放时间；
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_GetFileTotleTime(int Port, DWORD *pPlayTime);

/*
*	SetVolume 说明
*	功能: 设置音量大小
*	参数：
		1、Port：输入参数，表示数据播放端口；
		2、Volume：输入参数，表示设置的声音大小(1~100)；
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_SetVolume(int Port, int Volume);

/*
*	GetVolume 说明
*	功能: 获取音量大小
*	参数：
		1、Port：输入参数，表示数据播放端口；
		2、Volume：输出参数，表示当前音量大小；
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_GetVolume(int Port, int *pVolume);

/*
*	PlaySound 说明
*	功能: 播放声音
*	参数：
		1、Port：输入参数，表示数据播放端口；
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_OpenSound(int Port);

/*
*	StopSound 说明
*	功能: 停止播放声音
*	参数：
		1、Port：输入参数，表示数据播放端口；
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_CloseSound(int Port);

/*
*	StartAudioTallBack 说明
*	功能: 开启音频对讲
*	参数：
		1、Port：输入参数，表示数据播放端口；
		2、pCallBackFun：音频输出函数指针;
		3、pParam: 调用方传入参数给回调函数
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_StartAudioTallBack(int Port, AUDIOTALLBACKCALLBACKFUN pCallBackFun, void *pParam);

/*
*	StopAudioTallBack 说明
*	功能: 开启音频对讲
*	参数：
		1、Port：输入参数，表示数据播放端口；
*	返回值 0:成功 else:失败
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_StopAudioTallBack(int Port);


#pragma pack(4)

#ifdef	__cplusplus
}
#endif
