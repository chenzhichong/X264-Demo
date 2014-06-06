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
	//��ʼ����ʾ����
	void InitBMP();
	int ShowPicture(char *data, int length, int Width, int Height);

	//�����߳�
	static DWORD WINAPI PlayStream(void *arg);
	//��ȡ��Ƶ����
	static int ReadData(void *opaque, uint8_t *buf, int buf_size);
	//��ʾͼ��

private:
	//��ʽ����
	AVFormatContext *m_pFormatCtx;
	//����������
	AVCodecContext *m_pCodecCtx;
	//������
	AVCodec *m_pCodec;
	//ͼ���ʽת��������
	SwsContext *m_pConvertCtx;
	//��ʾ����
	HWND m_View;
	//��ʾͼ����Ϣ
	BITMAPINFO	BmpInfo;			//֡��Ϣ
	//����������
	int m_StreamIndex;
	//����״̬
	int m_PlayStatus;
	//���Ž����¼�
	HANDLE PlayThreadHandle;
	//����BUFFER
	uint8_t m_pBuf[FRAME_BUFFER_SIZE];
	
	//��Ƶ���ݶ���
	std::queue<ST_VideoData*> m_VideoDataQueue;
	AVIOContext * m_pIOContext;
	AVInputFormat *m_pInputFormat;
};

