/************************************************************************/
/* 
	文件名称：IPCDataStruct.h
	功　　能：IPC数据结构头文件
	作　　者：hg
	创建时间：2013-08-30
	版　　本：1.0
---------------------------------------------------------------------------
修改记录：
	修改人			修改时间		     	版本		       修改内容
	陈元丰			2013-11-07			V1.1		   增加双向对讲语音处理功能.
*/
/************************************************************************/

#pragma once

#pragma pack(1)

#define	UNIQUEID_LENTH		32					//唯一标识符长度
#define TEMPLATE_LENGTH		32					//所有使用长度为32个字节的宏

//设备类型定义
typedef enum E_Device_Type
{
	IPCS_TELEPHONE = 1,				//移动终端
	IPCS_COMPUTER = 2				//PC终端
};

//数据流采集类型
typedef enum E_EncodeType
{
	IPCS_VIDEO = 0,					//视频流
	IPCS_AUDIO = 1,					//音频流
	IPCS_MIXED = 2					//音视频混合流
};

//灯光开关
typedef enum E_LIGHT_STATE
{
	IPCS_LIGHT_SHUT = 0,			//灯光关
	IPCS_LIGHT_OPEN = 1				//灯光开
};

//视频采集格式
typedef enum E_VIDEO_FORMAT
{
	VIDEO_RGB24 = 0L,				//BI_RGB RGB24格式采集
	VIDEO_I420 = 0x30323449			//I420(即YUV420P)格式采集
};

//开始动作、结束动作标识
typedef enum E_PTZ_STATE
{
	PTZ_STOP = 0,					//停止动作
	PTZ_START = 1					//开始动作
};

//控制状态标识
typedef enum E_PTZ_FLAG
{
	PTZ_CAMERA_FAR = 1,				//镜头远
	PTZ_CAMERA_NEAR = 2,			//镜头近
	PTZ_FOCAL_FAR = 3,				//焦距远
	PTZ_FOCAL_NEAR = 4,				//焦距近
	PTZ_APERTURE_LARGE = 5,			//光圈大
	PTZ_APERTURE_SMALL = 6,			//光圈小
	PTZ_LIGHT_OPEN = 7,				//灯光开
	PTZ_LIGHT_CLOSED = 8			//灯光关
};

//异常消息类型
enum EN_Exception
{
	NormalConnect,					//正常链接
	Login,							//设备登陆
	Logout,							//设备登出
	ConnectError,					//连接错误
	Delete,							//待删除
};

//帧类型（I、B、P）
typedef enum EN_FrameType
{
	FRAME_I = 1,					//I帧
	FRAME_B = 2,					//B帧
	FRAME_P = 3,					//P帧
	FRAME_AUDIO = 4					//Audio帧
};

//实时数据类型
typedef enum EN_RealDataType
{
	NON_DATA  = 0,					//未知数据.		
	VIDEO_DATA = 1,					//视频数据.
	AUDIO_DATA = 2,					//音频数据.
	MIXED_DATA = 3					//音视频数据.				
};

//配置项结构体
typedef struct ST_Configure
{
	unsigned int Width;				//分辨率宽
	unsigned int Height;			//分辨率高
	unsigned int FrameRate;			//帧率（10）(1-30)
	unsigned int BitRate;			//码率
	unsigned int EncodeType;		//编码方式（默认H264，定义在编码头文件中）
	unsigned int CaptureType;		//数据流采集类型
	unsigned int Saturation;		//饱和度(0-100)
	unsigned int Contrast;			//对比度(0-100)
	unsigned int KeyFrame;			//关键帧(以帧为单位)
	E_LIGHT_STATE Light;			//灯光

	ST_Configure()
	{
		FrameRate = 10;
		Saturation = 50;
		Contrast = 50;
		Light = IPCS_LIGHT_SHUT;
	}

}*PST_Configure;

//支持手机可能ST_VideoInfo这个结构只有一个成员VideoFormat, 因此定义以下宏.
//来兼容手机和PC.
#define		VIDEO_INFO_LENGTH_FOR_TELEPHONE		4
#define		TELEPHONE_VIDEO_WIDTH				320
#define		TELEPHONE_VIDEO_HEIGHT				240

//视频采集参数
typedef struct ST_VideoInfo
{
	int		VideoFormat;			//帧采集格式(E_VIDEO_FORMAT)
	int		Width;					//帧宽
	int 	Height;					//帧高
}*PST_VideoInfo;


#pragma pack()