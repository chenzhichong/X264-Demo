
// X264-DemoDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include "CaptureImage.h"
#include "H264Encoderc.h"
#include "PlayVideo.h"
#include "RTPRecv.h"
#include "RecordVideo.h"
#include "H264StreamPlayer.h"

// CX264DemoDlg dialog
class CX264DemoDlg : public CDialogEx
{
// Construction
public:
	CX264DemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_X264DEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	static int ImageCallBack(void *data, int len, void *UserData);
	static int X264FrameCallBack(int FrameType, void *data, int len, void *UserData);
	static int RTPFrameCallBack(int FrameType, void *data, int len, void *UserData);
	static int RTPRecvStatusCallBack(int Status, char *IP, int Port, void* UserData);
private:
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedCancel();
private:
	char* GetRecordFilePath(char *FilePath);
	//��С������
	void SetWindowMiniSize();
	//��ԭ����
	void SetWindowNormalSize();
	//���ش���
	void SetWindowHide();

	//��С��������
	void SetWindowToTray();
	//ɾ������
	void DeleteTray();
private:
	//����Ԥ������
	CStatic m_Preview;
	//Զ�̲��Ŵ���
	CStatic m_DecodeView;
	//����ͼ��ɼ���
	CCaptureImage m_CaptureImage;
	//H264������
	CH264Encoderc m_Encodec;
	//��Ƶ��ʾ��
	CPlayVideo m_PlayVideo;
	//RTP������
	CRTPRecv m_RTPRecv;
	//¼����
	CRecordVideo m_RecordVideo;
	//H264������ʾ��
	CH264StreamPlayer m_H264StreamPlayer;
protected:
	afx_msg LRESULT OnShowTask(WPARAM wParam, LPARAM lParam);
};
