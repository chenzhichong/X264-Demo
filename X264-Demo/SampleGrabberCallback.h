#ifndef __SAMPLEGRABBERCALLBACK_H__
#define __SAMPLEGRABBERCALLBACK_H__

#include "Common.h"
#include <windows.h>
#include <dshow.h>
#include <dshowutil.h>
#include <initguid.h>
#include <iostream>
using namespace std;
#pragma include_alias( "dxtrans.h", "qedit.h" )
#define __IDxtCompositor_INTERFACE_DEFINED__
#define __IDxtAlphaSetter_INTERFACE_DEFINED__
#define __IDxtJpeg_INTERFACE_DEFINED__
#define __IDxtKey_INTERFACE_DEFINED__
#include "qedit.h"

//设备名称
const wchar_t CAPTURE_DEVICE_NAME[][64] = {L"SMI Grabber Device", L"AVerMedia 7231 Analog Capture"};
const wchar_t CROSSBAR_DEVICE_NAME[][64] = {L"SM BDA Crossbar Filter", L"AVerMedia 7231 Analog Xbar"};
const wchar_t CAPTURE_DEVICE_VEDIO_PIN_IN_NAME[][64] = {L"Video Tuner In", L"模拟视频输入"};
const wchar_t CAPTURE_DEVICE_VEDIO_PIN_OUT_NAME[][64] = {L"Capture Out", L"捕获"};
const wchar_t CROSSBAR_DEVICE_PIN_OUT_NAME[][64] = {L"0: Video Decoder Out", L"0: Video Decoder Out"};
//设备后
const int WHICH_DEVICE = 1;
//GUID
// SMI Grabber Device
DEFINE_GUID(CLSID_VideoCaptureSources,
0x860BB310, 0x5D01, 0x11D0, 0xBD, 0x3B, 0x00, 0xA0, 0xC9, 0x11, 0xCE, 0x86);
// (A799A801-A46D-11D0-A18C-00A02401DCD4}
// SM BDA Crossbar Filter
DEFINE_GUID(CLSID_WDM,
0xA799A801, 0xA46D, 0x11D0, 0xA1, 0x8C, 0x00, 0XA0, 0x24, 0x01, 0xDC, 0XD4);
//
template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

class SampleGrabberCallback : public ISampleGrabberCB
{
public:
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void** ppvObject);

	HRESULT STDMETHODCALLTYPE SampleCB(double Time, IMediaSample *pSample);
	HRESULT STDMETHODCALLTYPE BufferCB(double Time, BYTE *pBuffer, long BufferLen);
	
	SampleGrabberCallback(IMAGE_CALLBACK func, void *usedata);
	static int X264FrameCallBack(int FrameType, void *pData, int Length, void *pContext);
	static int RTPFrameCallBack(int FrameType, void *pData, int Length, void *pContext);
	HRESULT ConvertYUY2ToYUV420(BYTE *pSrc, BYTE *pDst, int iWidth, int iHeight);
	HRESULT ConvertUYVYToYUV420(BYTE *pSrc, BYTE *pDst, int iWidth, int iHeight);
	BOOL SaveBitmap(BYTE * pBuffer, long lBufferSize ); //保存bitmap图片
	BOOL SaveRaw(BYTE * pBuffer, long lBufferSize ); //保存一帧原始数据
	BOOL SaveRawToSequence(BYTE * pBuffer, long lBufferSize ); //保存为视频序列
	BOOL HandleRaw(BYTE * pBuffer, long lBufferSize ); // handle data
public:
	BOOL m_bGetPicture;  // is get a picture
	long m_lWidth;       //存储图片的宽度
	long m_lHeight;		 //存储图片的长度
	int  m_iBitCount;    //the number of bits per pixel (bpp)
	void *m_UseData;
	TCHAR m_chTempPath[MAX_PATH];
	TCHAR m_chSwapStr[MAX_PATH];
	TCHAR m_chDirName[MAX_PATH];
	long m_lTotalFrame;	//总帧数
	BOOL m_bIsFirst;	//保存为视频序列时，指示是否第一次创建文件
	IMAGE_CALLBACK m_pCallBack;
};

#endif //__SAMPLEGRABBERCALLBACK_H__