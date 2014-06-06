#pragma once
#include "../include/PlaySDK.h"
#pragma comment (lib, "../lib/IPCPlaySDK.lib")
class CPlayVideo
{
public:
	CPlayVideo(void);
	~CPlayVideo(void);
	int Initialize(HWND hwnd);
	void Uninitialize();
	void InsertData(void *pData, int len);
private:
	HWND m_Hwnd;
	long m_Port;
};

