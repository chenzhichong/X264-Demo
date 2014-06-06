#pragma once
#include "SampleGrabberCallback.h"

class CCaptureImage
{
public:
	CCaptureImage(void);
	~CCaptureImage(void);
	int Initialize(HWND hwnd, int Height, int Width, IMAGE_CALLBACK func, void *usedata);
	void Uninitialize();
protected:
	HRESULT CreateFilterByName(PCWSTR pszFilterName, const GUID& clsidDeviceClass, IBaseFilter** ppFilter);
	HRESULT GetPin(IBaseFilter* pFilter, PCWSTR pszPinName, IPin** ppPin);
	HRESULT SaveGraphToFile(WCHAR *wszPath);
	HRESULT SetProperty(IBaseFilter* pFilter);
	HRESULT SetGrabberProperty();
	HRESULT SetCrossbarProperty();
	HRESULT SetSampleGrabberProperty();
	void CloseInterface(); //close all interface
private:
	IGraphBuilder *m_pGraphBuilder;
	ICaptureGraphBuilder2 *m_pCaptureGB;
	IMediaControl *m_pMediaControl;
	IBaseFilter *m_pGrabberDevFilter;
	IBaseFilter *m_pCrossbarFilter;
	IBaseFilter *m_pSampGrabberFilter;
	ISampleGrabber *m_pSampGrabber;
	IBaseFilter *m_pRenderFilter;
	HWND m_hwnd;
	int m_Height;
	int m_Width;
	IMAGE_CALLBACK m_pCallBack;
	void *m_UseData;
	BITMAPINFO m_BmpInfo;
	int m_InitFlag;
	CRITICAL_SECTION m_cs;
};

