#include "stdafx.h"
#include "RecordVideo.h"


CRecordVideo::CRecordVideo(void):m_pFile(0),FileSize(0),m_InitFlag(FALSE),m_RecordFlag(FALSE),m_BeginingFlag(FALSE)
{
	memset(m_FilePath,0,HOST_NAME_LENGTH);
	InitializeCriticalSection(&m_CS);
}


CRecordVideo::~CRecordVideo(void)
{
	DeleteCriticalSection(&m_CS);
}

int CRecordVideo::StartRecord( const char *FilePath )
{
	EnterCriticalSection(&m_CS);
	if (m_RecordFlag)
	{
		LeaveCriticalSection(&m_CS);
		LOG_INFO("alrealy Record, file name is " << m_FilePath);
		return TRUE;
	}
	if (NULL==FilePath)
	{
		LeaveCriticalSection(&m_CS);
		LOG_WARN("FilePath is NULL!!");
		return FALSE;
	}
	if (strlen(FilePath)>=HOST_NAME_LENGTH)
	{
		LeaveCriticalSection(&m_CS);
		LOG_WARN("FilePath length more than " << HOST_NAME_LENGTH);
		return FALSE;
	}
	//保存文件名，供日志打印使用
	memcpy(m_FilePath,FilePath,strlen(FilePath));
	m_pFile = fopen(m_FilePath,"wb");
	if (NULL==m_pFile)
	{
		LeaveCriticalSection(&m_CS);
		LOG_ERROR("fopen error, file name is " << m_FilePath);
		return FALSE;
	}
	m_RecordFlag=TRUE;
	m_BeginingFlag=TRUE;
	LeaveCriticalSection(&m_CS);
	LOG_INFO("Start Record, file name is " << m_FilePath);
	return TRUE;
}

int CRecordVideo::StopRecord()
{
	EnterCriticalSection(&m_CS);
	if (!m_RecordFlag)
	{
		LeaveCriticalSection(&m_CS);
		LOG_WARN("Has not been started Record");
		return FALSE;
	}
	fclose(m_pFile);

	m_RecordFlag=FALSE;
	m_BeginingFlag=FALSE;

	LeaveCriticalSection(&m_CS);
	LOG_INFO("StopRecord, file name is " << m_FilePath);
	return TRUE;
}

void CRecordVideo::InsertVideoData( const char *pData, int length, int FrameType)
{
	EnterCriticalSection(&m_CS);
	if (m_RecordFlag)
	{

		if (EN_FrameType::I_FRAME==FrameType||!m_BeginingFlag)
		{
			m_BeginingFlag=FALSE;
			fwrite(pData,length,1,m_pFile);
		}
	}
	LeaveCriticalSection(&m_CS);
}
