#include "stdafx.h"
#include "CaptureImage.h"

CCaptureImage *pCaptureImage=NULL;
SampleGrabberCallback *pSampleGrabberCB;  //CallBack
CCaptureImage::CCaptureImage(void):m_hwnd(0),m_Width(0),m_Height(0),m_pCallBack(0),m_UseData(0),m_InitFlag(FALSE)
{
	pCaptureImage = this;
	InitializeCriticalSection(&m_cs);
}


CCaptureImage::~CCaptureImage(void)
{
	DeleteCriticalSection(&m_cs);
}

int CCaptureImage::Initialize( HWND hwnd, int Height, int Width, IMAGE_CALLBACK func, void *usedata )
{
	EnterCriticalSection(&m_cs);
	if (m_InitFlag)
	{
		LeaveCriticalSection(&m_cs);
		return TRUE;
	}
	LOG_DEBUG("CCaptureImage::Initialize");
	m_Height = Height;
	m_Width = Width;
	m_pCallBack = func;
	m_UseData = usedata;
	pSampleGrabberCB = new SampleGrabberCallback(func, usedata);
	HRESULT hr = S_OK;

	//Create the filter graph
	hr = CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC_SERVER,
		IID_IGraphBuilder,(LPVOID*)&m_pGraphBuilder);
	if(FAILED(hr))
		return hr;

	//Create the capture graph builder
	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2,NULL,CLSCTX_INPROC_SERVER,
		IID_ICaptureGraphBuilder2,(LPVOID*)&m_pCaptureGB);
	if(FAILED(hr))
		return hr;

	//Obtain interfaces for media control and Video Window
	hr = m_pGraphBuilder->QueryInterface(IID_IMediaControl,(LPVOID*)&m_pMediaControl);
	if(FAILED(hr))
		return hr;

	m_pCaptureGB->SetFiltergraph(m_pGraphBuilder);
	if(FAILED(hr))
		return hr;

	//add Capture devcie 
	hr = CreateFilterByName(CAPTURE_DEVICE_NAME[WHICH_DEVICE], CLSID_VideoCaptureSources, &m_pGrabberDevFilter);
	if(FAILED(hr))
	{
		printf("创建%ls失败！\n", CAPTURE_DEVICE_NAME[WHICH_DEVICE]);
		return hr;
	}
	hr = m_pGraphBuilder->AddFilter(m_pGrabberDevFilter, CAPTURE_DEVICE_NAME[WHICH_DEVICE]);
	if(FAILED(hr))
	{
		printf("添加%ls失败！\n", CAPTURE_DEVICE_NAME[WHICH_DEVICE]);
		return hr;
	}

	//add SM BDA Crossbar Filter 
	hr = CreateFilterByName(CROSSBAR_DEVICE_NAME[WHICH_DEVICE], CLSID_WDM, &m_pCrossbarFilter);
	if(FAILED(hr))
	{
		printf("创建%ls失败！\n", CROSSBAR_DEVICE_NAME[WHICH_DEVICE]);
		return hr;
	}
	hr = m_pGraphBuilder->AddFilter(m_pCrossbarFilter, CROSSBAR_DEVICE_NAME[WHICH_DEVICE]);
	if(FAILED(hr))
	{
		printf("添加%ls失败！\n");
		return hr;
	}

	//add SampleGrabber
	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (LPVOID*)&m_pSampGrabberFilter);
	if(FAILED(hr))
	{
		printf("创建SampleGrabber Filter失败！\n");
		return hr;
	}
	hr = m_pGraphBuilder->AddFilter(m_pSampGrabberFilter, L"SampleGrabber");
	if(FAILED(hr))
	{
		printf("添加SampleGrabber Filter失败！\n");
		return hr;
	} 
	hr = m_pSampGrabberFilter->QueryInterface(IID_ISampleGrabber, (LPVOID*)&m_pSampGrabber);
	if (FAILED(hr))
	{
		printf("查询ISampleGrabber接口失败！\n");
		return hr;
	}
	//set mediatype
	hr = SetSampleGrabberProperty();
	if (FAILED(hr))
	{
		printf("设置SampleGrabber属性失败！\n");
		return hr;
	}

	//add Null Renderer
	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (LPVOID*)&m_pRenderFilter);
	if(FAILED(hr))
	{
		printf("创建Null Renderer Filter失败！\n");
		return hr;
	}
	hr = m_pGraphBuilder->AddFilter(m_pRenderFilter, L"Null Renderer");
	if(FAILED(hr))
	{
		printf("添加Null Renderer Filter失败！\n");
		return hr;
	}

	//connect SM BDA Crossbar Filter and SMI Grabber Device
	IPin *pPinOut = NULL;
	IPin *pPinIn = NULL;
	hr = GetPin(m_pCrossbarFilter, CROSSBAR_DEVICE_PIN_OUT_NAME[WHICH_DEVICE], &pPinOut);
	if(FAILED(hr))
	{
		printf("GetPin[%ls:%ls]失败！\n", CROSSBAR_DEVICE_NAME[WHICH_DEVICE], CROSSBAR_DEVICE_PIN_OUT_NAME[WHICH_DEVICE]);
		return hr;
	}
	hr = GetPin(m_pGrabberDevFilter, CAPTURE_DEVICE_VEDIO_PIN_IN_NAME[WHICH_DEVICE], &pPinIn);
	if(FAILED(hr))
	{
		printf("GetPin[%ls:%ls]失败！\n", CAPTURE_DEVICE_NAME[WHICH_DEVICE], CAPTURE_DEVICE_VEDIO_PIN_IN_NAME[WHICH_DEVICE]);
		return hr;
	}
	hr = m_pGraphBuilder->ConnectDirect(pPinOut, pPinIn, NULL);
	if(FAILED(hr))
	{
		printf("连接失败！\n");
		SafeRelease(&pPinOut);
		SafeRelease(&pPinIn);
		return hr;
	}
	SafeRelease(&pPinOut);
	SafeRelease(&pPinIn);

	//connect SMI Grabber Device and SampleGrabber
	hr = GetPin(m_pGrabberDevFilter,CAPTURE_DEVICE_VEDIO_PIN_OUT_NAME[WHICH_DEVICE], &pPinOut);
	if(FAILED(hr))
	{
		printf("GetPin[%ls:%ls]失败！\n", CAPTURE_DEVICE_NAME[WHICH_DEVICE], CAPTURE_DEVICE_VEDIO_PIN_OUT_NAME[WHICH_DEVICE]);
		return hr;
	}
	hr = GetPin(m_pSampGrabberFilter, L"Input", &pPinIn);
	if(FAILED(hr))
	{
		printf("GetPin[%ls:%ls]失败！\n", L"SampleGrabber", L"Input");
		return hr;
	}
	hr = m_pGraphBuilder->ConnectDirect(pPinOut, pPinIn, NULL);
	if(FAILED(hr))
	{
		printf("连接失败！\n");
		SafeRelease(&pPinOut);
		SafeRelease(&pPinIn);
		return hr;
	}
	SafeRelease(&pPinOut);
	SafeRelease(&pPinIn);

	//connect SampleGrabber and Null Render
	hr = GetPin(m_pSampGrabberFilter, L"Output", &pPinOut);
	if(FAILED(hr))
	{
		printf("GetPin[%ls:%ls]失败！\n", L"SampleGrabber", L"Output");
		return hr;
	}
	hr = GetPin(m_pRenderFilter, L"In", &pPinIn);
	if(FAILED(hr))
	{
		printf("GetPin[%ls:%ls]失败！\n", L"Null Render", L"In");
		return hr;
	}
	hr = m_pGraphBuilder->ConnectDirect(pPinOut, pPinIn, NULL);
	if(FAILED(hr))
	{
		printf("连接失败！\n");
		SafeRelease(&pPinOut);
		SafeRelease(&pPinIn);
		return hr;
	}
	SafeRelease(&pPinOut);
	SafeRelease(&pPinIn);

	////创建播放窗口
	//RECT rect;
	//GetWindowRect(hwnd,&rect);
	//m_hwnd=::capCreateCaptureWindow(_T("Capture Window"),WS_CHILD|WS_VISIBLE,0,0,rect.right-rect.left,rect.bottom-rect.top,hwnd,0);
	//if (NULL==m_hwnd)
	//{
	//	LeaveCriticalSection(&m_cs);
	//	LOG_ERROR("capCreateCaptureWindow error");
	//	return false;
	//}
	//int res = 0;
	////连接设备
	////res = capDriverConnect(m_hwnd,0);
	//while((res = capDriverConnect(m_hwnd,0))!=TRUE)
	//	continue;
	//if (!res)
	//{
	//	LeaveCriticalSection(&m_cs);
	//	LOG_ERROR("capDriverConnect error");
	//	return false;
	//}

	////设置画面
	//InitBitmap();

	////设置捕捉参数
	//InitCaputerParms();

	//capPreviewScale(m_hwnd,TRUE);

	////设置预览速率
	//capPreviewRate(m_hwnd,FRAME_RATE);


	////开始预览
	//capPreview(m_hwnd,TRUE);
	//

	////设置回调函数
	//capSetCallbackOnVideoStream(m_hwnd,StreamCallBack);
	//capCaptureSequenceNoFile(m_hwnd);

	/*capOverlay(m_hwnd,TRUE);*/
	//set mediatype
	hr = SetGrabberProperty();
	if (FAILED(hr))
	{
		printf("设置Grabber属性失败！\n");
		return hr;
	}
	//set mediatype
	hr = SetCrossbarProperty();
	if (FAILED(hr))
	{
		printf("设置Crossbar属性失败！\n");
		return hr;
	}
	//run
	hr = m_pMediaControl->Run();
	if(FAILED(hr))
	{
		printf("运行失败！\n");
		return hr;
	}
	m_InitFlag = TRUE;
	LeaveCriticalSection(&m_cs);
	return hr;
}

void CCaptureImage::Uninitialize()
{
	EnterCriticalSection(&m_cs);
	if (!m_InitFlag)
	{
		LeaveCriticalSection(&m_cs);
		return;
	}
	int res=0;
	if (m_pMediaControl)
		m_pMediaControl->Stop();

	if (pSampleGrabberCB)
	{
		delete pSampleGrabberCB;
	}
	CloseInterface();
	m_hwnd=0;
	m_InitFlag=FALSE;
	LeaveCriticalSection(&m_cs);
}

HRESULT CCaptureImage::CreateFilterByName(PCWSTR pszFilterName, const GUID& clsidDeviceClass, IBaseFilter** ppFilter)
{
	HRESULT hr = S_OK;
	if (!ppFilter)
		return E_POINTER;

	// Create the System Device Enumerator.
	ICreateDevEnum *pSysDevEnum = NULL;
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void **)&pSysDevEnum);
	if (FAILED(hr))
	{
		return hr;
	}

	// Obtain a class enumerator for the video compressor category.
	IEnumMoniker *pEnumCat = NULL;
	hr = pSysDevEnum->CreateClassEnumerator(clsidDeviceClass, &pEnumCat, 0);

	if (hr == S_OK) 
	{
		// Enumerate the monikers.
		IMoniker *pMoniker = NULL;
		ULONG cFetched;
		while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
		{
			IPropertyBag *pPropBag;
			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, 
				(void **)&pPropBag);
			if (SUCCEEDED(hr))
			{
				// To retrieve the filter's friendly name, do the following:
				VARIANT varName;
				VariantInit(&varName);
				hr = pPropBag->Read(L"FriendlyName", &varName, 0);
				if (SUCCEEDED(hr))
				{
					if (wcscmp(pszFilterName, varName.bstrVal) == 0)
					{
						// To create an instance of the filter, do the following:
						hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter,
							(void**)ppFilter);
						if (FAILED(hr))
						{
							printf("Can't bind moniker to filter object.\n");
						}
						printf("Success bind moniker [%ls] to filter object.\n", pszFilterName);
						pPropBag->Release();
						pMoniker->Release();
						pEnumCat->Release();
						pSysDevEnum->Release();
						return hr;
						// Now add the filter to the graph. 
					}
				}
				VariantClear(&varName);
				//Remember to release pFilter later.
				pPropBag->Release();
			}
			pMoniker->Release();
		}
		pEnumCat->Release();
	}
	pSysDevEnum->Release();
	return hr;
}

HRESULT CCaptureImage::GetPin(IBaseFilter* pFilter, PCWSTR pszPinName, IPin** ppPin)
{
	HRESULT hr = S_OK;
	if (!ppPin)
		return E_POINTER;

	IEnumPins *pEnum;
	hr = pFilter->EnumPins(&pEnum);
	if (FAILED(hr))
	{
		printf("Can't enumerate pins.");
		return hr;
	}

	while (pEnum->Next(1, ppPin, 0) == S_OK)
	{
		PIN_INFO pInfo;
		hr = (*ppPin)->QueryPinInfo(&pInfo);
		BOOL bFound = !_wcsicmp(pszPinName, pInfo.achName);
		if (pInfo.pFilter)
		{
			pInfo.pFilter->Release();
		}
		if (bFound)
		{
			return hr;
		}
	}
	return hr;
}

HRESULT CCaptureImage::SaveGraphToFile(WCHAR *wszPath)
{
	HRESULT hr = S_OK;
	if (!m_pGraphBuilder)
		return E_POINTER;

	hr = SaveGraphFile(m_pGraphBuilder, wszPath);
	if (FAILED(hr))
	{
		printf("保存Graph失败！");
		return hr;
	}
	return hr;
}

HRESULT CCaptureImage::SetProperty(IBaseFilter* pFilter)
{
	HRESULT hr = S_OK;
	if (!pFilter)
		return E_POINTER;

	hr = ShowFilterPropertyPage(pFilter, NULL);
	if (FAILED(hr))
	{
		//printf("显示属性对话框失败！");
		return hr;
	}
	return hr;
}

HRESULT CCaptureImage::SetGrabberProperty()
{
	HRESULT hr = S_OK;
	hr = ShowFilterPropertyPage(m_pGrabberDevFilter, NULL);
	if (FAILED(hr))
	{
		printf("显示GrabberDevFilter属性对话框失败！\n");
		return hr;
	}
	return hr;
}

HRESULT CCaptureImage::SetCrossbarProperty()
{
	HRESULT hr = S_OK;
	hr = ShowFilterPropertyPage(m_pCrossbarFilter, NULL);
	if (FAILED(hr))
	{
		printf("显示Crossbar属性对话框失败！\n");
		return hr;
	}
	return hr;
}

HRESULT CCaptureImage::SetSampleGrabberProperty()
{
	HRESULT hr = S_OK;
	if (!m_pSampGrabber)
		return E_POINTER;

	AM_MEDIA_TYPE mt;
	ZeroMemory(&mt, sizeof(mt));
	mt.majortype = MEDIATYPE_Video;
	switch (WHICH_DEVICE)
	{
	case 0:
		mt.subtype = MEDIASUBTYPE_UYVY;
		break;
	case 1:
		mt.subtype = MEDIASUBTYPE_YUY2;
		break;
	default:
		break;
	}

	hr = m_pSampGrabber->SetMediaType(&mt);
	if (FAILED(hr))
	{
		printf("设置sample格式失败！");
		return hr;
	}
	hr = m_pSampGrabber->SetBufferSamples(TRUE);
	if (FAILED(hr))
	{
		printf("设置buffer失败！");
		return hr;
	}
	hr = m_pSampGrabber->SetCallback(pSampleGrabberCB, 1);
	if (FAILED(hr))
	{
		printf("设置callback失败！");
		return hr;
	}
	return hr;
}
void CCaptureImage::CloseInterface()
{
	if (m_pMediaControl)
		m_pMediaControl->Stop();
	/*if(m_pVideoWindow)
	{
	m_pVideoWindow->get_Visible(FALSE);
	m_pVideoWindow->put_Owner(NULL);
	}*/

	/*if(m_pMediaEvent)
	m_pMediaEvent->SetNotifyWindow(NULL,WM_GRAPHNOTIFY,0);*/

	//release interface
	SafeRelease(&m_pGrabberDevFilter);
	SafeRelease(&m_pCrossbarFilter);
	SafeRelease(&m_pCaptureGB);
	SafeRelease(&m_pGraphBuilder);
	SafeRelease(&m_pMediaControl);
	//SafeRelease(&m_pMediaEvent);
	SafeRelease(&m_pSampGrabber);
	//SafeRelease(m_pVideoWindow);
}
