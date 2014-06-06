/************************************************************************/
/* 
	文件名称：RTPRecv.cpp
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
#include "stdafx.h"
#include "RTPRecv.h"
#include "x264-RTP.h"
#define RTP_RECV_MAX_INTERVAL 1000000  //当1秒没有收到视频，认为本地视频通话结束
const int TIMESTAMP=90000/FRAME_RATE;
CRTPRecv::CRTPRecv(void):m_InitializeFlag(false),m_pCallBack(NULL),m_pContext(0),m_pRTPRecvStatusCallBack(0),
	m_pRTPRecvStatusUserData(0),m_BufLen(0),m_RecvFlag(FALSE),m_RecvTimeStamp(0),m_CheckThreadStatus(FALSE),
	m_CheckThreadHandle(NULL),m_FrameType(EN_FrameType::I_FRAME)
{
	memset(m_Buf,0,MAX_DATA_LENGTH);
}


CRTPRecv::~CRTPRecv(void)
{
}

//uLocalPort：本地通讯端口， dTimeStamp：视频传输时间戳
int CRTPRecv::Initialize(int Port, FRAME_CALLBACK pCallBack, void* pContext, 
						 RTP_RECV_STATUS_CALLBACK pRecvStatusCallBack, void* pRecvStatusUserData)
{
	if (m_InitializeFlag)
	{
		LOG_INFO("CRTPRecv has been initialize");
		return 0;
	}

	//判断RTP是否已经初始化
	if(RTPSession::IsActive())
	{
		LOG_INFO("RTP alrealy start");
		return 0;
	}
	
	int Result=0;
	//初始化RTP库
	if(0!=InitRTP(Port))
	{
		LOG_ERROR("Create rtp error");
		return -1;
	}

	//回调函数赋值
	if (NULL == pCallBack)
	{
		LOG_WARN("FRAMECALLBACK is NULL");
	}
	m_pCallBack = pCallBack;
	m_pContext  = pContext;

	//回调函数赋值
	if (NULL==pRecvStatusCallBack)
	{
		LOG_WARN("RecvStatusCallBack is NULL");
	}
	m_pRTPRecvStatusUserData=pRecvStatusUserData;
	m_pRTPRecvStatusCallBack=pRecvStatusCallBack;

	/*//创建监测线程
	m_RecvFlag=FALSE;
	m_CheckThreadStatus=TRUE;
	m_CheckThreadHandle = CreateThread(0,0,CheckRecvThread,this,0,0);
	if (NULL==m_CheckThreadHandle)
	{
		LOG_ERROR("CreateThread ERROR");
		exit(-1);
	}*/
	//测试使用
	RTPIPv4Address addr(ntohl(inet_addr("10.28.5.188")),RTP_RECV_PORT);
	AddDestination(addr);
	m_InitializeFlag=true;
	LOG_DEBUG("Create RTP success, port is "<< Port);
	return 0;
}

//反初始化
void CRTPRecv::UnInitialize()
{
	if (m_InitializeFlag==false)
	{
		return;
	}
	m_InitializeFlag = false;
	/*//结束线程
	m_CheckThreadStatus=FALSE;
	WaitForSingleObject(m_CheckThreadHandle,INFINITE);
	CloseHandle(m_CheckThreadHandle);
	m_CheckThreadHandle=NULL;*/

	//关闭RTP
	BYEDestroy(RTPTime(RTP_BYE_DELAY_TIME,0), 0, 0);

	m_pContext = NULL;
	m_pCallBack = NULL;
	m_pRTPRecvStatusUserData=NULL;
	m_pRTPRecvStatusUserData=NULL;

	LOG_DEBUG("RTP Uninitialize");
}

void CRTPRecv::OnPollThreadStep()
{
	BeginDataAccess();
	// check incoming packets
	if (GotoFirstSourceWithData())
	{	
		//处理接收数据包.
		do
		{
			RTPPacket *pack = NULL;
			while ((pack = GetNextPacket()) != NULL)
			{
				m_RecvTimeStamp = GetTickCount();
				
				if (!m_RecvFlag)
				{
					//有新数据进来
					m_RecvFlag=TRUE;
					//获取对端地址
					RTPAddress *pAddress = (RTPAddress*)(GetCurrentSourceInfo()->GetRTPDataAddress());
					//添加远端到发送地址列表
					RTPIPv4Address *pIPV4Address = (RTPIPv4Address*)pAddress;
					//AddDestination(*pIPV4Address);
					
					//获取远端IP
					in_addr addr;
					addr.S_un.S_addr=pIPV4Address->GetIP();

					//通知出去
					//m_pRTPRecvStatusCallBack(EN_RTP_RECV_STATUS::StartRecv,inet_ntoa(addr),pIPV4Address->GetPort(),m_pRTPRecvStatusUserData);
				}
				HandleRTPData(pack);
				DeletePacket(pack);
			}
		} while (GotoNextSourceWithData());
	}
	EndDataAccess();
}

int CRTPRecv::GetRemoteRTPPort()
{
	RTPAddress *pAddress = (RTPAddress*)(GetCurrentSourceInfo()->GetRTPDataAddress());
	//默认IPV4
	RTPIPv4Address *pIPV4Address = (RTPIPv4Address*)pAddress;
	return pIPV4Address->GetPort();
}

//获取远端IP
char* CRTPRecv::GetRemoteRTPIP()
{
	//static char RemoteIP[IP_LENGTH];
	RTPAddress *pAddress = (RTPAddress*)(GetCurrentSourceInfo()->GetRTPDataAddress());
	//默认IPV4
	RTPIPv4Address *pIPV4Address = (RTPIPv4Address*)pAddress;
	in_addr addr;
	addr.S_un.S_addr=pIPV4Address->GetIP();
	return inet_ntoa(addr);
}

char* CRTPRecv::GetLocalIP()
{
	//获取本机IP
	static char LocalIP[IP_LENGTH];
	in_addr addrLocal;
	char szName[HOST_NAME_LENGTH]={0}; 
	gethostname(szName, HOST_NAME_LENGTH); 
	hostent *pHost  = gethostbyname(szName); 
	if(NULL == pHost)//   failed 
	{
		LOG_ERROR("Get local ip error");
		return NULL; 
	}
	addrLocal.s_addr = **(DWORD**)(pHost-> h_addr_list); //获取当前主机IP
	memcpy(LocalIP, inet_ntoa(addrLocal), strlen(inet_ntoa(addrLocal)));
	return LocalIP;
}

int CRTPRecv::InitRTP(int Port)
{
	RTPUDPv4TransmissionParams TransParams;
	RTPSessionParams SessionParams;

	//设置本地通讯端口.
	TransParams.SetPortbase(Port);
	//设置时间戳,每秒钟发送的包数量.
	SessionParams.SetOwnTimestampUnit(1.0/90000.0);
	SessionParams.SetAcceptOwnPackets(true);	//接收自己发送的数据包

	//创建RTP对象.
	int Result = Create(SessionParams, &TransParams);
	if (Result < 0)
	{
		LOG_ERROR("RTP: "<<RTPGetErrorString(Result));
		return -1;
	}
	

	//设置默认负载类型.
	this->SetMaximumPacketSize(65535);
	//this->SetDefaultMark(true); 
	//this->SetDefaultTimestampIncrement(TIMESTAMP);
	//this->SetDefaultPayloadType(H264);

	return 0;
}

void CRTPRecv::HandleRTPData( RTPPacket *pPacket )
{
	//获取RTP负载类型，如果不是H264直接返回
	if (pPacket->GetPayloadType()!=H264)
	{
		LOG_DEBUG("RTP Recv error Data, payload type is " << pPacket->GetPayloadType());
		return;
	}
	LOG_DEBUG("squence is " << pPacket->GetSequenceNumber() << ", length is " << pPacket->GetPayloadLength());
	//
	unsigned long timestamp=0;
	unsigned char *pData=NULL;
	int Length=0;
	//解析数据
	timestamp=pPacket->GetTimestamp();
	pData = pPacket->GetPayloadData();
	Length = pPacket->GetPayloadLength();
	
	//解析Header
	NALU_HEADER *pHeader = reinterpret_cast<NALU_HEADER*>(&pData[0]);
	
	/*//帧分界符
	if (H264_PACKET_END == pHeader->TYPE)
	{
		try
		{
			if(m_pCallBack!=NULL)
			{
				m_pCallBack(FrameType,Buffer,m_BufLen,m_pContext);
			}
			else
			{
				LOG_WARN("Frame CallBack is NULL");
			}
		}
		catch (...)
		{
			LOG_ERROR("Call Frame callback error");
		}
		m_BufLen=0;
		m_FirstNal=true;
		return;
	}*/

	if (NALU_TYPE_FU_A!=pHeader->TYPE)
	{
		//单包
	
		//解析BitStream
		H264_BitStream *pBitStream = reinterpret_cast<H264_BitStream*>(&pData[1]);
		//如果是PPS sps 不对 H264 bit stream 进行判断
		if (pHeader->TYPE==EN_H264_NALU_TYPE::PPS||pHeader->TYPE==EN_H264_NALU_TYPE::SPS)
		{
			m_FrameType = pHeader->TYPE;
			//单帧首个nal
			m_Buf[m_BufLen]=0;
			m_Buf[m_BufLen+1]=0;
			m_Buf[m_BufLen+2]=0;
			m_Buf[m_BufLen+3]=1;
			m_BufLen+=4;
		}
		else if (EN_H264_NALU_TYPE::SEI==pHeader->TYPE)
		{
			m_FrameType = pHeader->TYPE;
			m_Buf[m_BufLen]=0;
			m_Buf[m_BufLen+1]=0;
			m_Buf[m_BufLen+2]=1;
			m_BufLen+=3;
		}
		//判断是否是帧的第一个宏块
		else if (pBitStream->GetFirstMBInSlice())
		{
			if (0!=m_BufLen)
			{
				//把之前的Nalu返回
				m_pCallBack(m_FrameType,m_Buf,m_BufLen,m_pContext);
				m_BufLen=0;
			}
			

			//判断接下来的帧类型
			if (pBitStream->isIFrame())
			{
				m_FrameType=EN_FrameType::I_FRAME;
			}
			else
			{
				m_FrameType==EN_FrameType::P_FRAME;
			}

			//单帧首个nal
			m_Buf[m_BufLen]=0;
			m_Buf[m_BufLen+1]=0;
			m_Buf[m_BufLen+2]=0;
			m_Buf[m_BufLen+3]=1;
			m_BufLen+=4;
		}
		else
		{
			m_Buf[m_BufLen]=0;
			m_Buf[m_BufLen+1]=0;
			m_Buf[m_BufLen+2]=1;
			m_BufLen+=3;
		}
		memcpy(m_Buf+m_BufLen,pData,Length);
		m_BufLen+=Length;
	}
	else
	{
		//分片
		//解析FU头
		FU_HEADER *pFUHeader = reinterpret_cast<FU_HEADER*>(&pData[1]);
		if (pFUHeader->S)
		{
			//解析BitStream
			H264_BitStream *pBitStream = reinterpret_cast<H264_BitStream*>(&pData[2]);
			if (pBitStream->GetFirstMBInSlice())
			{
				//如果上一帧是SPS PPS SEI， 后面紧跟的是内容
				if (m_FrameType==EN_H264_NALU_TYPE::SPS||m_FrameType==EN_H264_NALU_TYPE::PPS||m_FrameType==EN_H264_NALU_TYPE::SEI)
				{
					m_Buf[m_BufLen]=0;
					m_Buf[m_BufLen+1]=0;
					m_Buf[m_BufLen+2]=1;
					m_BufLen+=3;
				}
				else
				{
					if (0!=m_BufLen)
					{
						//把之前的Nalu返回
						m_pCallBack(m_FrameType,m_Buf,m_BufLen,m_pContext);
						m_BufLen=0;
					}
					m_Buf[m_BufLen]=0;
					m_Buf[m_BufLen+1]=0;
					m_Buf[m_BufLen+2]=0;
					m_Buf[m_BufLen+3]=1;
					m_BufLen+=4;
				}
			}
			else
			{
				m_Buf[m_BufLen]=0;
				m_Buf[m_BufLen+1]=0;
				m_Buf[m_BufLen+2]=1;
				m_BufLen+=3;
			}
			//填写Nalu头
			NALU_HEADER *pNaluHeader = reinterpret_cast<NALU_HEADER*>(m_Buf+m_BufLen);
			memcpy(pNaluHeader,pData,1);
			if (pBitStream->isIFrame())
			{
				pNaluHeader->TYPE=5;
			}
			else
			{
				pNaluHeader->TYPE=1;
			}
			
			m_BufLen+=1;
			//视频数据
			memcpy(m_Buf+m_BufLen,pData+2,Length-2);
			m_BufLen+=Length-2;
		}
		else if (pFUHeader->E)
		{
			//分片最后一个包
			memcpy(m_Buf+m_BufLen,pData+2,Length-2);
			m_BufLen+=Length-2;
		}
		else
		{
			//分片中间包
			memcpy(m_Buf+m_BufLen,pData+2,Length-2);
			m_BufLen+=Length-2;
		}
	}
	
}

int CRTPRecv::FindStartCode3 (unsigned char *Buf)
{
	if(Buf[0]!=0 || Buf[1]!=0 || Buf[2] !=1) 
	{
		return 0;
	}
	else return 1;
}

int CRTPRecv::FindStartCode4 (unsigned char *Buf)
{
	if(Buf[0]!=0 || Buf[1]!=0 || Buf[2] !=0 || Buf[3] !=1) return 0;
	else return 1;
}

void CRTPRecv::SendH264Nalu(unsigned char* m_h264Buf,int buflen) 
{
	//时间戳
	//int TimesTampInc=3600;
	//Mark标志
	//int Mark = 1;
	//NAL单元（非发送结构）
	NALU_t  Nalu;

	//待处理的单帧数据指针s
	unsigned char *pSendbuf=NULL;
	int SendLen=0;

	//发送的数据缓冲
	char sendbuf[1430];
	memset(sendbuf,0,1430);

	//上一个startcode位置
	int LastStartPos=0;
	//本次STARTCODE位置
	int StartPos=0;
	//前导码为 0x00 00 01 或者 0x00 00 00 01
	int DataCount=3;
	unsigned char *pData=m_h264Buf;
	int res=0;
	while (DataCount<buflen)
	{
		res=FindStartCode3(pData+DataCount-3);
		if (res!=true)
		{	
			//未找到前导码
			if (DataCount!=buflen-1)
			{
				DataCount++;
				continue;
			}
			else
			{
				//最后一段数据
				StartPos = buflen;
			}
		}
		else
		{
			//找到前导码 0x00 00 01
			StartPos=DataCount-3;
			res=FindStartCode4(pData+DataCount-4);
			if(res==true)
			{

				//找到前导码 0x00 00 00 01
				StartPos=DataCount-4;
			}
		} 

		//切一个nal进行处理
		pSendbuf = pData+LastStartPos;
		SendLen = StartPos-LastStartPos;
		LastStartPos = StartPos;
		DataCount++;
		if (SendLen==0)
		{
			//第一个前导码不用处理
			continue;
		}
		int frontFlag=0;
		//去除前导码0x000001 或者0x00000001
		if( 0x01 == pSendbuf[2] )
		{
			pSendbuf = &pSendbuf[3];
			SendLen -= 3;
			frontFlag=3;
		}
		else if(0x01==pSendbuf[3])
		{
			pSendbuf = &pSendbuf[4];
			SendLen -= 4;
			frontFlag=4;
		}
		if (SendLen==0)
		{
			//前导码后没有数据
			LOG_WARN("NULL 264 packet");
			return;
		}
		//如果前导码为0x00 00 00 01 表示上一帧已经结束 发送分界nal
		/*if (frontFlag==4)
		{
			m_TimesTampInc=3600;
			char buf[6];
			memset(buf,0,6);
			buf[0]=0x09;
			buf[1]=0x30;
			SendPacket(buf,6,H264,1,m_TimesTampInc);
			m_TimesTampInc=0;
		}*/
		//解析nal头
		NALU_t na;
		na.forbidden_bit      = pSendbuf[0] & 0x80;
		na.nal_reference_idc  = (pSendbuf[0] & 0x60)>>5;
		na.nal_unit_type      = pSendbuf[0] & 0x1f;

		int status=0; 
		//单包
		if ( SendLen <= MAX_RTP_PKT_LENGTH )
		{
			//SetDefaultMark(true);
			status = this->SendPacket((void *)pSendbuf,SendLen,H264,1,TIMESTAMP);
		}  
		//分片
		else if(SendLen > MAX_RTP_PKT_LENGTH)
		{
			FU_INDICATOR    *pstFuInd;
			FU_HEADER       *pstFuHdr;
			char			*pNaluPayLoad;

			//解析nal头
			Nalu.forbidden_bit      = pSendbuf[0] & 0x80;
			Nalu.nal_reference_idc  = pSendbuf[0] & 0x60;
			Nalu.nal_unit_type      = pSendbuf[0] & 0x1f;
			pSendbuf+=1;
			SendLen-=1;
			//分片数量
			int PackCount=0;
			//最后一个分片长度
			int RemainLen=0; 
			PackCount = SendLen / MAX_RTP_PKT_LENGTH;
			RemainLen = SendLen % MAX_RTP_PKT_LENGTH;
			int PackNumber=0;//用指示当前发送的是第几个分片RTP包

			while (PackNumber<=PackCount)
			{
				pstFuInd       = (FU_INDICATOR*)&sendbuf[0];
				pstFuInd->F    = Nalu.forbidden_bit;
				pstFuInd->NRI  = Nalu.nal_reference_idc >> 5;
				pstFuInd->TYPE = NALU_TYPE_FU_A;
				//第一个分片
				if (PackNumber==0)
				{

					pstFuHdr         = (FU_HEADER*)&sendbuf[1];
					pstFuHdr->E    = 0;
					pstFuHdr->R    = 0;
					pstFuHdr->S    = 1;
					pstFuHdr->TYPE = Nalu.nal_unit_type;
					pNaluPayLoad     = &sendbuf[2];
					memcpy(pNaluPayLoad,pSendbuf+PackNumber*MAX_RTP_PKT_LENGTH,MAX_RTP_PKT_LENGTH);
					//SetDefaultMark(false);
					status = this->SendPacket((void *)sendbuf,MAX_RTP_PKT_LENGTH+2,H264,0,0);
					PackNumber++;
					continue;
				}
				//最后一个分片
				else if (((PackNumber==PackCount-1)&&RemainLen==0)||((PackNumber==PackCount)&&(RemainLen!=0)))
				{
					pstFuHdr         = (FU_HEADER*)&sendbuf[1];
					pstFuHdr->E    = 1;
					pstFuHdr->R    = 0;
					pstFuHdr->S    = 0;
					pstFuHdr->TYPE = Nalu.nal_unit_type;
					pNaluPayLoad     = &sendbuf[2];
					//SetDefaultMark(true);
					if (RemainLen==0)
					{
						memcpy(pNaluPayLoad,pSendbuf+PackNumber*MAX_RTP_PKT_LENGTH,MAX_RTP_PKT_LENGTH);
						status = this->SendPacket((void *)sendbuf,MAX_RTP_PKT_LENGTH+2,H264,1,0);
					}
					else
					{
						memcpy(pNaluPayLoad,pSendbuf+PackNumber*MAX_RTP_PKT_LENGTH,RemainLen);
						status = this->SendPacket((void *)sendbuf,RemainLen+2,H264,1,TIMESTAMP);
					}
					break;
				}
				//中间分片
				else
				{
					pstFuHdr         = (FU_HEADER*)&sendbuf[1];
					pstFuHdr->E    = 0;
					pstFuHdr->R    = 0;
					pstFuHdr->S    = 0;
					pstFuHdr->TYPE = Nalu.nal_unit_type;
					pNaluPayLoad     = &sendbuf[2];
					memcpy(pNaluPayLoad,pSendbuf+PackNumber*MAX_RTP_PKT_LENGTH,MAX_RTP_PKT_LENGTH);
					//SetDefaultMark(false);
					status = this->SendPacket((void *)sendbuf,MAX_RTP_PKT_LENGTH+2,H264,0,0);
					PackNumber++;
					continue;
				}

			}
		}


	}

}

DWORD WINAPI CRTPRecv::CheckRecvThread( void *arg )
{
	if (NULL==arg)
	{
		return NULL;
	}
	CRTPRecv *pRTPRecv = static_cast<CRTPRecv*>(arg);
	while (pRTPRecv->m_CheckThreadStatus)
	{
		if (pRTPRecv->m_RecvFlag)
		{
			//正在接收，检测数据相隔的时间差
			if (GetTickCount()-pRTPRecv->m_RecvTimeStamp>RTP_RECV_MAX_INTERVAL)
			{
				//时间差大于规定值，停止视频数据发送，并通知出去
				pRTPRecv->m_RecvFlag=FALSE;
				pRTPRecv->ClearDestinations();
				pRTPRecv->m_pRTPRecvStatusCallBack(EN_RTP_RECV_STATUS::StopRecv,0,0,pRTPRecv->m_pRTPRecvStatusUserData);
			}
		}
		Sleep(100);
	}
}
