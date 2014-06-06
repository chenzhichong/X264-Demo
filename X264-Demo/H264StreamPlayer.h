#pragma once
#include <queue>
extern "C"
{
#include "libavcodec\avcodec.h"
#include "libavformat\avformat.h"
#include "libswscale\swscale.h"
#include "libavutil\avutil.h"
};
#include "Common.h"

class CH264StreamPlayer
{
public:
	CH264StreamPlayer(void);
	~CH264StreamPlayer(void);
	int Initialize(HWND Hwnd);
	void UnInitialize();
	void InsertH264Steam(void *Data, int length);
private:
	//初始化显示参数
	void InitBMP();
	int ShowPicture(char *data, int length, int Width, int Height);

	//解码线程
	static DWORD WINAPI PlayStream(void *arg);
	//读取视频数据
	static int ReadData(void *opaque, uint8_t *buf, int buf_size);
	//显示图像

private:
	//格式环境
	AVFormatContext *m_pFormatCtx;
	//编码器环境
	AVCodecContext *m_pCodecCtx;
	//编码器
	AVCodec *m_pCodec;
	//图像格式转换器环境
	SwsContext *m_pConvertCtx;
	//显示窗口
	HWND m_View;
	//显示图像信息
	BITMAPINFO	BmpInfo;			//帧信息
	//数据流内容
	int m_StreamIndex;
	//播放状态
	int m_PlayStatus;
	//播放结束事件
	HANDLE PlayThreadHandle;
	//数据BUFFER
	uint8_t m_pBuf[FRAME_BUFFER_SIZE];
	
	//视频数据队列
	std::queue<ST_VideoData*> m_VideoDataQueue;
	AVIOContext * m_pIOContext;
	AVInputFormat *m_pInputFormat;
};

