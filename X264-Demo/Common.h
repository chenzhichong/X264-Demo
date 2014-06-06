#pragma once

typedef unsigned char      uint8_t;

#define FRAME_RATE 25					//֡��
#define BI_I420 0x30323449				//����I420
//#define I420_320_240 115200				//I420��ʽ 320*240 ͼ���С
#define I420_320_240 460800				//I420��ʽ 640*480 ͼ���С
#define MAX_DATA_LENGTH 65535			//������ݳ���
#define IP_LENGTH 40					//IP��󳤶�
#define RTP_BYE_DELAY_TIME  2000		//rtp �ر�ͨ���ӳ�ʱ��
#define RTP_RECV_PORT 12306				//RTP���ն˿�
#define TARGET_PORT 6000				//destation port
#define HOST_NAME_LENGTH 128			//��������󳤶�
const char RECORD_FILEPATH[]="./Record";  //��ǰ¼���ļ�·�����������ļ���
const int FRAME_BUFFER_SIZE = 32*1024;	  //FFmpeg������BUFFER��С

//ͼ�����ݻص�����
typedef int (*IMAGE_CALLBACK)(void *data, int len, void *UserData);
//֡���ݻص�����
typedef int (*FRAME_CALLBACK)(int FrameType, void *pData, int Length, void* UserData);
//RTP��������״̬֪ͨ
typedef int (*RTP_RECV_STATUS_CALLBACK)(int Status, char *IP, int Port, void* UserData);

//ת������
#define MY(a,b,c) (( a*  0.2989  + b*  0.5866  + c*  0.1145))
#define MU(a,b,c) (( a*(-0.1688) + b*(-0.3312) + c*  0.5000 + 128))
#define MV(a,b,c) (( a*  0.5000  + b*(-0.4184) + c*(-0.0816) + 128))

//��С�ж�
#define DY(a,b,c) (MY(a,b,c) > 255 ? 255 : (MY(a,b,c) < 0 ? 0 : MY(a,b,c)))
#define DU(a,b,c) (MU(a,b,c) > 255 ? 255 : (MU(a,b,c) < 0 ? 0 : MU(a,b,c)))
#define DV(a,b,c) (MV(a,b,c) > 255 ? 255 : (MV(a,b,c) < 0 ? 0 : MV(a,b,c)))
#define CLIP(a) ((a) > 255 ? 255 : ((a) < 0 ? 0 : (a)))

//��Ƶ�ɼ�����
typedef struct ST_VideoInfo
{
	int		VideoFormat;			//֡�ɼ���ʽ(E_VIDEO_FORMAT)
	int		Width;					//֡��
	int 	Height;					//֡��
	ST_VideoInfo(int format, int width, int height):VideoFormat(format),Width(width),Height(height)
	{}
	ST_VideoInfo():VideoFormat(0),Width(0),Height(0){}
	~ST_VideoInfo(){}
}*PST_VideoInfo;

//��Ƶ����
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

//֡��������
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