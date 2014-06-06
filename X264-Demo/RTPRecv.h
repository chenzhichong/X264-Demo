/************************************************************************/
/* 
	�ļ����ƣ�RTPRecv.h
	�������ܣ�RTP��Ƶ���ݽ���ʵ��
	�������ߣ�����
	����ʱ�䣺2013-06-26
	�桡������1.0
---------------------------------------------------------------------------
�޸ļ�¼��
	�޸���			�޸�ʱ��		     	�汾		       �޸�����
	��Ԫ��			2013-11-07			V1.1		   ����˫��Խ�����������.
*/
/************************************************************************/

#pragma once
#include <winsock2.h>
#include "rtpsession.h"
#include "rtpsessionparams.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtptimeutilities.h"
#include "rtppacket.h"
#include "rtpsourcedata.h"
#include "rtptransmitter.h"

#include "Common.h"



class CRTPRecv : public RTPSession
{
public:
	CRTPRecv(void);
	virtual ~CRTPRecv(void);

	//��ʼ��RTP
	int Initialize(int Port, FRAME_CALLBACK pCallBack, void* pContext, RTP_RECV_STATUS_CALLBACK pRecvStatusCallBack, void* pRecvStatusUserData);

	//����ʼ��
	void UnInitialize();

	//���H264֡����ΪRTP��Ƶ��������
	void SendH264Nalu(unsigned char* m_h264Buf,int buflen);

protected:
	//���ݽ����߳�
	virtual void OnPollThreadStep();
	void HandleRTPData(RTPPacket *pPacket);
	void HandleH264Stream(RTPPacket *pPacket);
private:

	//����RTP
	int InitRTP(int Port);
	
	//��ȡ����IP
	char* GetLocalIP();

	//��ȡԶ�˶˿�
	int GetRemoteRTPPort();
	
	//��ȡԶ��IP
	char* GetRemoteRTPIP();
	
	//Ѱ����ʼ��
	int CRTPRecv::FindStartCode4 (unsigned char *Buf);
	int CRTPRecv::FindStartCode3 (unsigned char *Buf);

	//���������̣߳��ж��Ƿ�ֹͣ��������
	static DWORD WINAPI CheckRecvThread(void *arg);

protected:
	FRAME_CALLBACK  				m_pCallBack;				//RTP�������ݻص�ָ��
	void*							m_pContext;					//�豸������

	bool							m_InitializeFlag;			//��ʼ����־

	RTP_RECV_STATUS_CALLBACK		m_pRTPRecvStatusCallBack;	//RTP����״̬֪ͨ
	void*							m_pRTPRecvStatusUserData;			//�û�����

	//����RTP����
	char							m_Buf[MAX_DATA_LENGTH];		///���ܻ�����
	int								m_BufLen;
	//��־��һ��Nal
	//bool m_FirstNal;

	//֡����
	int m_FrameType;

	//����RTP��Ƶ����
	//int m_TimesTampInc;	//ʱ���

	//�Ƿ����ڽ������ݱ�־
	bool m_RecvFlag;
	//ʱ���
	DWORD m_RecvTimeStamp;
	//����̵߳�����״̬
	bool m_CheckThreadStatus;
	//�߳̾��
	HANDLE m_CheckThreadHandle;
};

