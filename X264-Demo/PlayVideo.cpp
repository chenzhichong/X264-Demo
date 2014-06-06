#include "stdafx.h"
#include "PlayVideo.h"


CPlayVideo::CPlayVideo(void):m_Hwnd(0),m_Port(0)
{
	PLAYSDK_Initialize();
}


CPlayVideo::~CPlayVideo(void)
{
	PLAYSDK_UnInitialize();
}

int CPlayVideo::Initialize( HWND hwnd )
{
	m_Hwnd=hwnd;
	if (m_Port!=0)
	{
		CPlayVideo::Uninitialize();
	}
	int res=PLAYSDK_GetPlayPort(&m_Port);
	res=PLAYSDK_OpenPlayer(m_Port);
	res=PLAYSDK_SetStreamOpenMod(m_Port,REALTIME_STREAM);
	res=PLAYSDK_PlayMedia(m_Port,m_Hwnd);
	return true;
}

void CPlayVideo::Uninitialize()
{
	PLAYSDK_StopMedia(m_Port);
	PLAYSDK_ClosePlayer(m_Port);
	m_Port=0;
}

void CPlayVideo::InsertData( void *pData, int len )
{
	int res = PLAYSDK_ProcessVideoData(m_Port,(char*)pData,len);
	if (res)
	{
		LOG_ERROR("PLAYSDK_ProcessVideoData error");
	}
}
