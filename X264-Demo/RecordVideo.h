#pragma once
#include <windows.h>
#include "Common.h"
class CRecordVideo
{
public:
	CRecordVideo(void);
	~CRecordVideo(void);
	//��ʼ¼��
	int StartRecord(const char *FilePath);
	//����¼��
	int StopRecord();
	//������Ƶ����
	void InsertVideoData(const char *pData, int length, int FrameType);
private:
	//¼���ļ�ָ��
	FILE *m_pFile;
	//��·���ļ�����
	char m_FilePath[HOST_NAME_LENGTH];
	//�ļ���С
	int FileSize;
	//��ʼ����־
	bool m_InitFlag;
	//¼���־
	bool m_RecordFlag;
	//�ٽ���
	CRITICAL_SECTION m_CS;
	//�տ�ʼ¼��
	bool m_BeginingFlag;
};

