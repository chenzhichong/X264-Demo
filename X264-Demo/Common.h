#pragma once

typedef unsigned char      uint8_t;

#define FRAME_RATE 25					//帧率
#define BI_I420 0x30323449				//定义I420
//#define I420_320_240 115200				//I420格式 320*240 图像大小
#define I420_320_240 460800				//I420格式 640*480 图像大小
#define MAX_DATA_LENGTH 65535			//最大数据长度
#define IP_LENGTH 40					//IP最大长度
#define RTP_BYE_DELAY_TIME  2000		//rtp 关闭通道延迟时间
#define RTP_RECV_PORT 12306				//RTP接收端口
#define TARGET_PORT 6000				//destation port
#define HOST_NAME_LENGTH 128			//主机名最大长度
const char RECORD_FILEPATH[]="./Record";  //当前录像文件路径，不包含文件夹
const int FRAME_BUFFER_SIZE = 32*1024;	  //FFmpeg流数据BUFFER大小

//图像数据回调函数
typedef int (*IMAGE_CALLBACK)(void *data, int len, void *UserData);
//帧数据回调函数
typedef int (*FRAME_CALLBACK)(int FrameType, void *pData, int Length, void* UserData);
//RTP接受数据状态通知
typedef int (*RTP_RECV_STATUS_CALLBACK)(int Status, char *IP, int Port, void* UserData);

//转换矩阵
#define MY(a,b,c) (( a*  0.2989  + b*  0.5866  + c*  0.1145))
#define MU(a,b,c) (( a*(-0.1688) + b*(-0.3312) + c*  0.5000 + 128))
#define MV(a,b,c) (( a*  0.5000  + b*(-0.4184) + c*(-0.0816) + 128))

//大小判断
#define DY(a,b,c) (MY(a,b,c) > 255 ? 255 : (MY(a,b,c) < 0 ? 0 : MY(a,b,c)))
#define DU(a,b,c) (MU(a,b,c) > 255 ? 255 : (MU(a,b,c) < 0 ? 0 : MU(a,b,c)))
#define DV(a,b,c) (MV(a,b,c) > 255 ? 255 : (MV(a,b,c) < 0 ? 0 : MV(a,b,c)))
#define CLIP(a) ((a) > 255 ? 255 : ((a) < 0 ? 0 : (a)))

//视频采集参数
typedef struct ST_VideoInfo
{
	int		VideoFormat;			//帧采集格式(E_VIDEO_FORMAT)
	int		Width;					//帧宽
	int 	Height;					//帧高
	ST_VideoInfo(int format, int width, int height):VideoFormat(format),Width(width),Height(height)
	{}
	ST_VideoInfo():VideoFormat(0),Width(0),Height(0){}
	~ST_VideoInfo(){}
}*PST_VideoInfo;

//视频数据
typedef struct ST_VideoData
{
	uint8_t pData[FRAME_BUFFER_SIZE];
	int len;
	ST_VideoData(const uint8_t *data, int length):len(length)
	{
		memcpy(pData,data,length);
	}
	ST_VideoData():len(0)
	{

	}
};

//帧数据类型
enum EN_FrameType 
{
	UNKNOW=0,
	I_FRAME=1,
	P_FRAME=2,
	B_FRAME=3,
};

enum EN_RTP_RECV_STATUS
{
	StartRecv=0,
	StopRecv,
};