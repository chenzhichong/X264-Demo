// X264-DemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "X264-Demo.h"
#include "X264-DemoDlg.h"
#include "afxdialogex.h"

#include <ctime>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define VIDEO_HEIGHT 720
#define VIDEO_WIDTH 576
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CX264DemoDlg dialog



CX264DemoDlg::CX264DemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CX264DemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_MAIN);
}

void CX264DemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PREVIEW, m_Preview);
	DDX_Control(pDX, IDC_DECODEVIEW, m_DecodeView);
}

BEGIN_MESSAGE_MAP(CX264DemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CX264DemoDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CX264DemoDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDCANCEL, &CX264DemoDlg::OnBnClickedCancel)
	ON_MESSAGE(WM_SHOWTASK,&CX264DemoDlg::OnShowTask)
END_MESSAGE_MAP()


// CX264DemoDlg message handlers

BOOL CX264DemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon.

	//���ô����ص���ʾ
	m_DecodeView.ModifyStyle(0,WS_CLIPSIBLINGS);
	m_Preview.ModifyStyle(0,WS_CLIPSIBLINGS);

	//���ð�ť״̬
	GetDlgItem(IDC_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);

	// TODO: Add extra initialization here
	//��ʼ����־��
	GenewLog::Initialize("log.properties","Genewcptbtptp");

	//��ʼ�����ſ�
	m_PlayVideo.Initialize(m_DecodeView.GetSafeHwnd());
	//��ʼ�������
	m_Encodec.Initialzie(VIDEO_HEIGHT,VIDEO_WIDTH,X264FrameCallBack,this);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CX264DemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CX264DemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CX264DemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CX264DemoDlg::OnBnClickedStart()
{
	// TODO: Add your control notification handler code here

	GetDlgItem(IDC_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_STOP)->EnableWindow(TRUE);

	//��ʼ��RTP��
	m_RTPRecv.Initialize(RTP_RECV_PORT,RTPFrameCallBack,this,RTPRecvStatusCallBack,this);
	//��Ƶ��������Ƶ��׽��
	m_CaptureImage.Initialize(m_Preview.GetSafeHwnd(),VIDEO_HEIGHT,VIDEO_WIDTH,ImageCallBack,this);
	//m_H264StreamPlayer.Initialize(m_DecodeView);
	
	//*test
	char file[HOST_NAME_LENGTH];
	m_RecordVideo.StartRecord("d://record.264");
}


void CX264DemoDlg::OnBnClickedStop()
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);

	//m_H264StreamPlayer.UnInitialize();
	m_CaptureImage.Uninitialize();
	m_RTPRecv.UnInitialize();
	SetWindowToTray();

	//*test
	m_RecordVideo.StopRecord();
	

}


void CX264DemoDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnBnClickedStop();
	m_Encodec.UnInitialize();
	m_PlayVideo.Uninitialize();
	//ɾ��ϵͳ����ͼ��
	this->DeleteTray();
	CDialogEx::OnCancel();
}

int CX264DemoDlg::ImageCallBack( void *data, int len, void *UserData )
{
	CX264DemoDlg *pDlg = static_cast<CX264DemoDlg*>(UserData);
	pDlg->m_Encodec.InsertImageData(data,len);
	return true;
}

int CX264DemoDlg::X264FrameCallBack(int FrameType, void *data, int len, void *UserData )
{
	if (NULL==UserData)
	{
		return 0;
	}
	CX264DemoDlg *pDlg = static_cast<CX264DemoDlg*>(UserData);
	//�����ر������Ƶ���ͳ�ȥ
	//pDlg->m_RTPRecv.SendH264Nalu((unsigned char*)data,len);

	pDlg->m_RecordVideo.InsertVideoData((char*)data,len,FrameType);
	//pDlg->m_RecordVideo.InsertVideoData((char*)data,len,FrameType);
	//RTPFrameCallBack(FrameType,data,len,UserData);
	return true;
}

int CX264DemoDlg::RTPFrameCallBack(int FrameType, void *data, int len, void *UserData )
{
	//��Զ����Ƶ�ڱ��ز���
	CX264DemoDlg *pDlg = static_cast<CX264DemoDlg*>(UserData);
	pDlg->m_RecordVideo.InsertVideoData((char*)data,len,FrameType);

	char *pBuf= new char[len + sizeof(ST_VideoInfo)];
	ST_VideoInfo *pVideoInfo = reinterpret_cast<ST_VideoInfo*>(pBuf); 
	pVideoInfo->VideoFormat=htonl(BI_I420);
	pVideoInfo->Width=htonl(VIDEO_WIDTH);
	pVideoInfo->Height=htonl(VIDEO_HEIGHT);
	memcpy(pBuf+sizeof(ST_VideoInfo),data,len);
	//��ʾ
	pDlg->m_PlayVideo.InsertData(pBuf,len+sizeof(ST_VideoInfo));
	delete[] pBuf;
	return true;
}

int CX264DemoDlg::RTPRecvStatusCallBack( int Status, char *IP, int Port, void* UserData )
{
	if (NULL==UserData)
	{
		return 0;
	}
	CX264DemoDlg *pX264Dlg = static_cast<CX264DemoDlg*>(UserData);
	//����״̬������ز���
	switch (Status)
	{
	case EN_RTP_RECV_STATUS::StartRecv:
		//��ʼ¼��
		//char FilePath[HOST_NAME_LENGTH];
		//pX264Dlg->m_RecordVideo.StartRecord(pX264Dlg->GetRecordFilePath(FilePath));
		//��ʼ������Ƶץȡ
		//pX264Dlg->OnBnClickedStart();
		//pX264Dlg->OnShowTask(NULL,WM_LBUTTONDBLCLK);
		
		break;
	case EN_RTP_RECV_STATUS::StopRecv:
		//ֹͣ¼��
		//pX264Dlg->m_RecordVideo.StopRecord();
		//ֹͣ������Ƶץȡ
		//pX264Dlg->OnBnClickedStop();
		//pX264Dlg->SetWindowToTray();
		break;
	}
	return 0;
}

char* CX264DemoDlg::GetRecordFilePath( char *FilePath )
{
	if (NULL==FilePath)
	{
		return NULL;
	}
	CString cstrFilePath(RECORD_FILEPATH);
	if (!PathFileExists(cstrFilePath))
	{
		CreateDirectory(cstrFilePath,NULL);
	}
	time_t t = time(0);
	tm *tt = localtime(&t);
	//sprintf(FilePath, "%s//%04d%02d%02d%02d%02d%02d.264",RECORD_FILEPATH,tt->tm_year+1900,tt->tm_mon+1,tt->tm_mday,tt->tm_hour,tt->tm_min,tt->tm_sec);
	return FilePath;
}

void CX264DemoDlg::SetWindowMiniSize()
{
	WINDOWPLACEMENT WindowSize;
	this->GetWindowPlacement(&WindowSize);
	WindowSize.showCmd = SW_SHOWMINIMIZED;
	this->SetWindowPlacement(&WindowSize);
}

void CX264DemoDlg::SetWindowNormalSize()
{
	WINDOWPLACEMENT WindowSize;
	this->GetWindowPlacement(&WindowSize);
	WindowSize.showCmd = SW_SHOWNORMAL;
	this->SetWindowPlacement(&WindowSize);
}

void CX264DemoDlg::SetWindowHide()
{
	WINDOWPLACEMENT WindowSize;
	this->GetWindowPlacement(&WindowSize);
	WindowSize.showCmd = SW_HIDE;
	this->SetWindowPlacement(&WindowSize);
}

void CX264DemoDlg::SetWindowToTray()
 {
	   NOTIFYICONDATA nid;
	   nid.cbSize=(DWORD)sizeof(NOTIFYICONDATA);
	   nid.hWnd=this->m_hWnd;
	   nid.uID=IDR_MAINFRAME;
	   nid.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP ;
	   nid.uCallbackMessage=WM_SHOWTASK;//�Զ������Ϣ����
	   nid.hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_MAIN));
	   memcpy(nid.szTip,_T("��Ƶͨ��"),sizeof(_T("��Ƶͨ��")));
	   Shell_NotifyIcon(NIM_ADD,&nid); //�����������ͼ��
	   ShowWindow(SW_HIDE); //����������
}

afx_msg LRESULT CX264DemoDlg::OnShowTask(WPARAM wParam, LPARAM lParam)
{
	if(wParam!=IDR_MAINFRAME)
		return 1;
	switch(lParam)
	{
	case WM_RBUTTONUP://�Ҽ�����ʱ������ݲ˵�������ֻ��һ�����رա�
		{ LPPOINT lpoint=new tagPOINT;
		::GetCursorPos(lpoint);//�õ����λ��
		CMenu menu;
		menu.CreatePopupMenu();//����һ������ʽ�˵�
		//���Ӳ˵���رա������������ϢWM_DESTROY�������ڽ����������
		menu.AppendMenu(MF_STRING,WM_DESTROY,_T("�ر�")); //ȷ������ʽ�˵���λ��
		menu.TrackPopupMenu(TPM_LEFTALIGN,lpoint->x,lpoint->y,this); //��Դ����
		HMENU hmenu=menu.Detach();
		menu.DestroyMenu();
		delete lpoint;
		} break;
	case WM_LBUTTONDBLCLK://˫������Ĵ���
		{ this->ShowWindow(SW_SHOW);//��ʾ������
		DeleteTray();
		} break;
	default: break;
	}
	return 0;
}

void CX264DemoDlg::DeleteTray()
{
	NOTIFYICONDATA nid;
	nid.cbSize=(DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd=this->m_hWnd;
	nid.uID=IDR_MAINFRAME;
	nid.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP ;
	nid.uCallbackMessage=WM_SHOWTASK;//�Զ������Ϣ����
	nid.hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_MAIN));
	memcpy(nid.szTip,_T("��Ƶͨ��"),sizeof(_T("��Ƶͨ��")));
	Shell_NotifyIcon(NIM_DELETE,&nid); //��������ɾ��ͼ��
}
