#pragma once
#include <windows.h>
#include "Common.h"
class CRecordVideo
{
public:
	CRecordVideo(void);
	~CRecordVideo(void);
	//开始录像
	int StartRecord(const char *FilePath);
	//结束录像
	int StopRecord();
	//插入视频数据
	void InsertVideoData(const char *pData, int length, int FrameType);
private:
	//录像文件指针
	FILE *m_pFile;
	//带路径文件名称
	char m_FilePath[HOST_NAME_LENGTH];
	//文件大小
	int FileSize;
	//初始化标志
	bool m_InitFlag;
	//录像标志
	bool m_RecordFlag;
	//临界区
	CRITICAL_SECTION m_CS;
	//刚开始录像
	bool m_BeginingFlag;
};

