/************************************************************************/
/* 
	�ļ����ƣ�PlaySDK.h
	�������ܣ�PlaySDK�ӿ��ļ�
	�������ߣ�hg
	����ʱ�䣺2013-08-05
	�桡������1.0
---------------------------------------------------------------------------
�޸ļ�¼��
	�޸���			�޸�ʱ��		     	�汾		       �޸�����
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
	REALTIME_STREAM = 0,			//ʵʱ��
	FILE_STREAM						//�ļ���
};

typedef enum E_MediaType
{
	MEDIA_VIDEO = 0,				//��Ƶ��
	MEDIA_AUDIO = 1,				//��Ƶ��
	MEDIA_MIX						//�����
};

typedef	struct  ST_VideoAttribute
{
	BITMAPINFO	BmpInfo;			//֡��Ϣ
}*PST_VideoAttribute;

typedef struct ST_AudioAttribute
{
	int			Volume;				//������С
}*PST_AudioAttribute;

typedef struct U_MediaAttribute
{
	E_MediaType	MediaType;				//����������
	ST_VideoAttribute VideoAttribute;	//��Ƶ����
	ST_AudioAttribute AudioAttribute;	//��Ƶ����
}*PU_MediaAttribute;

typedef void(__stdcall *AUDIOTALLBACKCALLBACKFUN)(char *pData, int Length, void *pParam);


//�ӿڶ��岿��
//////////////////////////////////////////////////////////////////////////////////////////
/*
*	Initialize ˵��
*	����: ��ʼ��
*	��������
*	����ֵ: 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_Initialize();

/*
*	UnInitialize ˵��
*	����: ����ʼ�����ͷ���Դ
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿�
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_UnInitialize();

/*
*	GetPlayPort ˵��
*	����: ��ȡ���ݲ��Ŷ˿�
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿�
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_GetPlayPort(long *pPort);

/*
*	OpenPlayer ˵��
*	����: �򿪲�����
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿�
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_OpenPlayer(int Port);

/*
*	ClosePlayer ˵��
*	����: �رղ�����
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿�
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_ClosePlayer(int Port);

/*
*	PlayMedia ˵��
*	����: ��ʼ��������
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ� 
		2��hWnd����������� ��ʾ���ݲ��Ŵ��ھ��;
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_PlayMedia(int Port, HWND hWnd);

/*
*	StopMedia ˵��
*	����: ֹͣ��������
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ� 
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_StopMedia(int Port);

/*
*	SetMediaAttribute ˵��
*	����: ������������
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ�
		2��MediaType�������������ʾ��������;
		3��pMediaAttribute�������������ʾ��������;
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_SetMediaAttribute(int Port, U_MediaAttribute MediaAttribute);

/*
*	GetMediaAttribute ˵��
*	����: ��ȡ��������
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ�
		2��MediaType�������������ʾ��ǰ��������;
		3��pMediaAttribute�������������ʾ��ǰ��������;
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_GetMediaAttribute(int Port, U_MediaAttribute *pMediaAttribute);

/*
*	SetStreamOpenMod ˵��
*	����: ����������ģʽ��1��ʵʱ���� 2���ļ�����
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ�
		2��StreamMode:�����������ʾ������ģʽ��
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_SetStreamOpenMod(int Port, E_StreamMode StreamMode);

/*
*	GetStreamOpenMod ˵��
*	����: ��ȡ������ģʽ
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ�
		2��StreamMode:�����������ʾ������ģʽ��
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_GetStreamOpenMod(int Port, E_StreamMode *pStreamMode);

/*
*	ProcessVideoData ˵��
*	����: ����ʵʱ��Ƶ��
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ�
		2��pData:�����������ʾʵʱ��Ƶ���ݣ�
		3��Length�������������ʾʵʱ���ݳ��ȣ�
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_ProcessVideoData(int Port, const char *pData, int Length);

/*
*	ProcessAudioData ˵��
*	����: ����ʵʱ��Ƶ��
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ�
		2��pData:�����������ʾʵʱ��Ƶ���ݣ�
		3��Length�������������ʾʵʱ���ݳ��ȣ�
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_ProcessAudioData(int Port, const char *pData, int Length);

/*
*	ProcessMixData ˵��
*	����: ����ʵʱ�����
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ�
		2��pData:�����������ʾʵʱ��������ݣ�
		3��Length�������������ʾʵʱ���ݳ��ȣ�
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_ProcessMixData(int Port, const char *pData, int Length);

/*
*	StartRecord ˵��
*	����: ��ʼ¼��
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ�
		2��pFilePath:�����������ʾ¼���ļ�����ȫ·���������ļ�������
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_StartRecord(int Port, const char *pFilePath);

/*
*	StopRecord ˵��
*	����: ֹͣ¼��
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ�
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_StopRecord(int Port);

/*
*	SnapShot ˵��
*	����: ��ͼ
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ�
		2��pFilePath:�����������ʾ��ͼ�ļ�����ȫ·���������ļ�������
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_SnapShot(int Port, const char *pFilePath);

/*
*	OpenStreamFile ˵��
*	����: ���������ļ�
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ�
		2��pFilePath:�����������ʾ�ļ����ļ�·����
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_OpenStreamFile(int Port, const char *pFilePath);

/*
*	CloseStreamFile ˵��
*	����: �ر��������ļ�
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ�
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_CloseStreamFile(int Port);

/*
*	PauseStreamFile ˵��
*	����: ��ͣ�����������ļ�
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ�
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_PauseStreamFile(int Port);

/*
*	StepStreamFile ˵��
*	����: ���������������ļ�
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ�
		2��Next�������������ʾ�Ƿ�ִ����һ��(0:��ǰһ����1���һ��)��
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_StepStreamFile(int Port, unsigned char Next);

/*
*	SetFilePlaySpeed ˵��
*	����: �����ļ������ٶ�
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ�
		2��nSpeed�������������ʾ�ļ������ٶ�(1~100)��
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_SetFilePlaySpeed(int Port, int nSpeed);

/*
*	GetFilePlaySpeed ˵��
*	����: ��ȡ�ļ������ٶ�
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ�
		2��nSpeed�������������ʾ�ļ������ٶ�(1~100)��
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_GetFilePlaySpeed(int Port, int *pSpeed);

/*
*	SetFilePlayPos ˵��
*	����: �����ļ�����λ��
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ�
		2��PlayTime�������������ʾ���õĲ���ʱ�䣻
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_SetFilePlayPos(int Port, DWORD PlayTime);

/*
*	GetFilePlayTime ˵��
*	����: ��ȡ�ļ�����ʱ���
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ�
		2��pPlayTime�������������ʾ�ļ��Ѳ���ʱ�䣻
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_GetFilePlayTime(int Port, DWORD *pPlayTime);

/*
*	GetFileTotleTime ˵��
*	����: ��ȡ�ļ��ܵĲ���ʱ��
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ�
		2��pPlayTime�������������ʾ�ļ��ܵĲ���ʱ�䣻
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_GetFileTotleTime(int Port, DWORD *pPlayTime);

/*
*	SetVolume ˵��
*	����: ����������С
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ�
		2��Volume�������������ʾ���õ�������С(1~100)��
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_SetVolume(int Port, int Volume);

/*
*	GetVolume ˵��
*	����: ��ȡ������С
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ�
		2��Volume�������������ʾ��ǰ������С��
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_GetVolume(int Port, int *pVolume);

/*
*	PlaySound ˵��
*	����: ��������
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ�
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_OpenSound(int Port);

/*
*	StopSound ˵��
*	����: ֹͣ��������
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ�
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_CloseSound(int Port);

/*
*	StartAudioTallBack ˵��
*	����: ������Ƶ�Խ�
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ�
		2��pCallBackFun����Ƶ�������ָ��;
		3��pParam: ���÷�����������ص�����
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_StartAudioTallBack(int Port, AUDIOTALLBACKCALLBACKFUN pCallBackFun, void *pParam);

/*
*	StopAudioTallBack ˵��
*	����: ������Ƶ�Խ�
*	������
		1��Port�������������ʾ���ݲ��Ŷ˿ڣ�
*	����ֵ 0:�ɹ� else:ʧ��
*	----END-----------------------------------------------------------*/
STDMETHOD_INT PLAYSDK_StopAudioTallBack(int Port);


#pragma pack(4)

#ifdef	__cplusplus
}
#endif
