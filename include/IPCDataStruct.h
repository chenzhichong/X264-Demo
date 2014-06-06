/************************************************************************/
/* 
	�ļ����ƣ�IPCDataStruct.h
	�������ܣ�IPC���ݽṹͷ�ļ�
	�������ߣ�hg
	����ʱ�䣺2013-08-30
	�桡������1.0
---------------------------------------------------------------------------
�޸ļ�¼��
	�޸���			�޸�ʱ��		     	�汾		       �޸�����
	��Ԫ��			2013-11-07			V1.1		   ����˫��Խ�����������.
*/
/************************************************************************/

#pragma once

#pragma pack(1)

#define	UNIQUEID_LENTH		32					//Ψһ��ʶ������
#define TEMPLATE_LENGTH		32					//����ʹ�ó���Ϊ32���ֽڵĺ�

//�豸���Ͷ���
typedef enum E_Device_Type
{
	IPCS_TELEPHONE = 1,				//�ƶ��ն�
	IPCS_COMPUTER = 2				//PC�ն�
};

//�������ɼ�����
typedef enum E_EncodeType
{
	IPCS_VIDEO = 0,					//��Ƶ��
	IPCS_AUDIO = 1,					//��Ƶ��
	IPCS_MIXED = 2					//����Ƶ�����
};

//�ƹ⿪��
typedef enum E_LIGHT_STATE
{
	IPCS_LIGHT_SHUT = 0,			//�ƹ��
	IPCS_LIGHT_OPEN = 1				//�ƹ⿪
};

//��Ƶ�ɼ���ʽ
typedef enum E_VIDEO_FORMAT
{
	VIDEO_RGB24 = 0L,				//BI_RGB RGB24��ʽ�ɼ�
	VIDEO_I420 = 0x30323449			//I420(��YUV420P)��ʽ�ɼ�
};

//��ʼ����������������ʶ
typedef enum E_PTZ_STATE
{
	PTZ_STOP = 0,					//ֹͣ����
	PTZ_START = 1					//��ʼ����
};

//����״̬��ʶ
typedef enum E_PTZ_FLAG
{
	PTZ_CAMERA_FAR = 1,				//��ͷԶ
	PTZ_CAMERA_NEAR = 2,			//��ͷ��
	PTZ_FOCAL_FAR = 3,				//����Զ
	PTZ_FOCAL_NEAR = 4,				//�����
	PTZ_APERTURE_LARGE = 5,			//��Ȧ��
	PTZ_APERTURE_SMALL = 6,			//��ȦС
	PTZ_LIGHT_OPEN = 7,				//�ƹ⿪
	PTZ_LIGHT_CLOSED = 8			//�ƹ��
};

//�쳣��Ϣ����
enum EN_Exception
{
	NormalConnect,					//��������
	Login,							//�豸��½
	Logout,							//�豸�ǳ�
	ConnectError,					//���Ӵ���
	Delete,							//��ɾ��
};

//֡���ͣ�I��B��P��
typedef enum EN_FrameType
{
	FRAME_I = 1,					//I֡
	FRAME_B = 2,					//B֡
	FRAME_P = 3,					//P֡
	FRAME_AUDIO = 4					//Audio֡
};

//ʵʱ��������
typedef enum EN_RealDataType
{
	NON_DATA  = 0,					//δ֪����.		
	VIDEO_DATA = 1,					//��Ƶ����.
	AUDIO_DATA = 2,					//��Ƶ����.
	MIXED_DATA = 3					//����Ƶ����.				
};

//������ṹ��
typedef struct ST_Configure
{
	unsigned int Width;				//�ֱ��ʿ�
	unsigned int Height;			//�ֱ��ʸ�
	unsigned int FrameRate;			//֡�ʣ�10��(1-30)
	unsigned int BitRate;			//����
	unsigned int EncodeType;		//���뷽ʽ��Ĭ��H264�������ڱ���ͷ�ļ��У�
	unsigned int CaptureType;		//�������ɼ�����
	unsigned int Saturation;		//���Ͷ�(0-100)
	unsigned int Contrast;			//�Աȶ�(0-100)
	unsigned int KeyFrame;			//�ؼ�֡(��֡Ϊ��λ)
	E_LIGHT_STATE Light;			//�ƹ�

	ST_Configure()
	{
		FrameRate = 10;
		Saturation = 50;
		Contrast = 50;
		Light = IPCS_LIGHT_SHUT;
	}

}*PST_Configure;

//֧���ֻ�����ST_VideoInfo����ṹֻ��һ����ԱVideoFormat, ��˶������º�.
//�������ֻ���PC.
#define		VIDEO_INFO_LENGTH_FOR_TELEPHONE		4
#define		TELEPHONE_VIDEO_WIDTH				320
#define		TELEPHONE_VIDEO_HEIGHT				240

//��Ƶ�ɼ�����
typedef struct ST_VideoInfo
{
	int		VideoFormat;			//֡�ɼ���ʽ(E_VIDEO_FORMAT)
	int		Width;					//֡��
	int 	Height;					//֡��
}*PST_VideoInfo;


#pragma pack()