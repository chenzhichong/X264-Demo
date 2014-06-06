/************************************************************************/
/* 
	文件名称：RTPRecv.h
	功　　能：RTP视频数据接收实现
	作　　者：唐龙
	创建时间：2013-06-26
	版　　本：1.0
---------------------------------------------------------------------------
修改记录：
	修改人			修改时间		     	版本		       修改内容
	陈元丰			2013-11-07			V1.1		   增加双向对讲语音处理功能.
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

	//初始化RTP
	int Initialize(int Port, FRAME_CALLBACK pCallBack, void* pContext, RTP_RECV_STATUS_CALLBACK pRecvStatusCallBack, void* pRecvStatusUserData);

	//反初始化
	void UnInitialize();

	//拆解H264帧数据为RTP视频流并发送
	void SendH264Nalu(unsigned char* m_h264Buf,int buflen);

protected:
	//数据接收线程
	virtual void OnPollThreadStep();
	void HandleRTPData(RTPPacket *pPacket);
	void HandleH264Stream(RTPPacket *pPacket);
private:

	//创建RTP
	int InitRTP(int Port);
	
	//获取本机IP
	char* GetLocalIP();

	//获取远端端口
	int GetRemoteRTPPort();
	
	//获取远端IP
	char* GetRemoteRTPIP();
	
	//寻找起始码
	int CRTPRecv::FindStartCode4 (unsigned char *Buf);
	int CRTPRecv::FindStartCode3 (unsigned char *Buf);

	//创建监听线程，判断是否停止发送数据
	static DWORD WINAPI CheckRecvThread(void *arg);

protected:
	FRAME_CALLBACK  				m_pCallBack;				//RTP接收数据回调指针
	void*							m_pContext;					//设备上下文

	bool							m_InitializeFlag;			//初始化标志

	RTP_RECV_STATUS_CALLBACK		m_pRTPRecvStatusCallBack;	//RTP接收状态通知
	void*							m_pRTPRecvStatusUserData;			//用户数据

	//接受RTP流用
	char							m_Buf[MAX_DATA_LENGTH];		///接受缓冲区
	int								m_BufLen;
	//标志第一个Nal
	//bool m_FirstNal;

	//帧类型
	int m_FrameType;

	//发送RTP视频流用
	//int m_TimesTampInc;	//时间戳

	//是否正在接收数据标志
	bool m_RecvFlag;
	//时间戳
	DWORD m_RecvTimeStamp;
	//检测线程的运行状态
	bool m_CheckThreadStatus;
	//线程句柄
	HANDLE m_CheckThreadHandle;
};

