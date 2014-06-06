#include "stdafx.h"
#include "H264StreamPlayer.h"

CH264StreamPlayer::CH264StreamPlayer(void)
{
}


CH264StreamPlayer::~CH264StreamPlayer(void)
{
}

int CH264StreamPlayer::ReadData(void *opaque, uint8_t *buf, int buf_size)
{
	if (opaque==NULL)
	{
		LOG_ERROR("ReadData Param opaque is NULL");
		return FALSE;
	}
	CH264StreamPlayer *pPlayer = static_cast<CH264StreamPlayer*>(opaque);
	int size=0;
	ST_VideoData *pVideoData=NULL;

	do
	{
		if (pPlayer->m_VideoDataQueue.size()<=0)
		{
			Sleep(10);
			continue;
		}
		pVideoData=pPlayer->m_VideoDataQueue.front();
		pPlayer->m_VideoDataQueue.pop();
		size=pVideoData->len>buf_size?buf_size:pVideoData->len;
		memcpy(buf,pVideoData->pData,size);
		delete pVideoData;
		pVideoData=NULL;
		break;
	}while(1);
	return size;
}

int CH264StreamPlayer::Initialize( HWND Hwnd )
{
	m_View = Hwnd;
	//��ʼ��ͼ�����
	InitBMP();

	//����AVIOContext
	m_pIOContext = avio_alloc_context(m_pBuf,FRAME_BUFFER_SIZE,0,this,ReadData,NULL,NULL);
	
	//�������ʽ���˴������read_data����
	if (av_probe_input_buffer(m_pIOContext, &m_pInputFormat, "", NULL, 0, 0) < 0) 
	{
		LOG_ERROR("Cannot probe format!!");
		return FALSE;
	}

	//����AVFormatContext
	m_pFormatCtx = avformat_alloc_context();
	m_pFormatCtx->pb=m_pIOContext;

	//��AVFormatContext
	avformat_open_input(&m_pFormatCtx,"",m_pInputFormat,NULL);

	//Ѱ����Ƶ��
	m_StreamIndex=-1;
	for(int i=0; i<m_pFormatCtx->nb_streams; i++)
	{
		if(m_pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
		{
			m_StreamIndex=i;
			break;
		}
		if(m_StreamIndex==-1)
		{
			//�ڴ�й¶
			LOG_WARN("find video stream error");
			return FALSE;
		}
	}

	//���ұ������������
	m_pCodecCtx = m_pFormatCtx->streams[m_StreamIndex]->codec;
	//���ұ�����
	m_pCodec = avcodec_find_decoder(m_pCodecCtx->codec_id);
	if (m_pCodec==NULL)
	{
		LOG_ERROR("Find codec error, code id is " << m_pCodecCtx->codec_id);
		return FALSE;
	}
	m_pCodecCtx->time_base.num = 1; //�����У�һ����25֡
	m_pCodecCtx->time_base.den = 25;
	m_pCodecCtx->bit_rate = 0; //��ʼ��Ϊ0
	m_pCodecCtx->frame_number = 1; //ÿ��һ����Ƶ֡
	m_pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	m_pCodecCtx->width = 640;
	m_pCodecCtx->height = 480;
	m_pCodecCtx->pix_fmt = PIX_FMT_YUV420P;
	if(m_pCodec->capabilities&CODEC_CAP_TRUNCATED)
	{
		// we do not send complete frames 
		m_pCodecCtx->flags|= CODEC_FLAG_TRUNCATED;
	}
	//�򿪱�����
	if (avcodec_open2(m_pCodecCtx,m_pCodec,0))
	{
		LOG_ERROR("Open codec error");
		return FALSE;
	}
	//���������߳�
	DWORD PlayThreadID=0;
	m_PlayStatus=TRUE;
	PlayThreadHandle=CreateThread(0,0,PlayStream,this,0,&PlayThreadID);
	if(PlayThreadHandle==NULL)
	{
		LOG_ERROR("Create play thread error");
	}
	LOG_INFO("Create thread success, id is " << PlayThreadID);
}

void CH264StreamPlayer::UnInitialize()
{
	//�ȴ������߳̽���
	m_PlayStatus=FALSE;
	WaitForSingleObject(PlayThreadHandle,INFINITE);
	CloseHandle(PlayThreadHandle);
	PlayThreadHandle=NULL;

	if (m_pIOContext)
	{
		av_free(m_pIOContext);
		m_pIOContext=NULL;
	}

	//�ͷŸ�ʽ����
	if (m_pFormatCtx)
	{
		avformat_close_input(&m_pFormatCtx);
		av_free(m_pFormatCtx);
		m_pFormatCtx=NULL;
	}
	
	//�ͷű�����
	if (m_pCodecCtx)
	{
		avcodec_close(m_pCodecCtx);
		// 		av_free(m_pCodecCtx);
		m_pCodec=NULL;
		m_pCodecCtx=NULL;
	}
}

void CH264StreamPlayer::InitBMP()
{
	//BMS��ʾ����
	BmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BmpInfo.bmiHeader.biPlanes = 1;
	BmpInfo.bmiHeader.biBitCount = 24;
	BmpInfo.bmiHeader.biCompression = BI_RGB;
	BmpInfo.bmiHeader.biSizeImage = 0;
	BmpInfo.bmiHeader.biXPelsPerMeter = 0;
	BmpInfo.bmiHeader.biYPelsPerMeter = 0;
	BmpInfo.bmiHeader.biClrImportant = 0;
	BmpInfo.bmiHeader.biClrUsed = 0;
}

DWORD WINAPI CH264StreamPlayer::PlayStream( void *arg )
{
	CH264StreamPlayer *pPlayer = static_cast<CH264StreamPlayer*>(arg);

	//����֡����
	AVFrame *pFrameRGB,*pFrame;
	pFrame=avcodec_alloc_frame();
	pFrameRGB=avcodec_alloc_frame();
	if (pFrameRGB==NULL)
	{
		LOG_ERROR("alloc frame error");
		return 0;
	}
	//��ȡһ֡����buffer��С
	int numBytes=avpicture_get_size(PIX_FMT_RGB24, pPlayer->m_pCodecCtx->width,pPlayer->m_pCodecCtx->height);
	uint8_t * buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
	//��Buffer��֡�ṹ
	avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24,pPlayer->m_pCodecCtx->width,pPlayer->m_pCodecCtx->height);

	//����ͼ��ת����
	pPlayer->m_pConvertCtx = sws_getCachedContext(pPlayer->m_pConvertCtx,pPlayer->m_pCodecCtx->width,
		pPlayer->m_pCodecCtx->height, pPlayer->m_pCodecCtx->pix_fmt,pPlayer->m_pCodecCtx->width, pPlayer->m_pCodecCtx->height,
		PIX_FMT_BGR24, SWS_BICUBIC,NULL, NULL, NULL);

	int prepts=0;
	int FrameEnd;
	AVPacket packet;
	int res=0;
	int PicLenth;
	int64_t interval=0;
	int64_t t=0;
	int fps=pPlayer->m_pFormatCtx->streams[pPlayer->m_StreamIndex]->r_frame_rate.num;
	if (!fps)
	{
		fps=pPlayer->m_pFormatCtx->streams[pPlayer->m_StreamIndex]->nb_frames/(pPlayer->m_pFormatCtx->duration/1000000);
	}
	fps=30;
	int delaytime=1000/30;
	LARGE_INTEGER freq;
	LARGE_INTEGER startCount;
	LARGE_INTEGER endCount;
	QueryPerformanceFrequency(&freq);
	while(pPlayer->m_PlayStatus) 
	{
		QueryPerformanceCounter(&startCount);
		res=av_read_frame(pPlayer->m_pFormatCtx, &packet);
		if (res!=0)
		{
			LOG_WARN("av_read_frame error");
			break;
		}
		if(packet.stream_index==pPlayer->m_StreamIndex)
		{
			res=avcodec_decode_video2(pPlayer->m_pCodecCtx, pFrame, &FrameEnd,&packet);
			if (res<0)
			{
				LOG_WARN("Decode error!");
				continue;
			}
			if(FrameEnd)
			{
				//QueryPerformanceCounter(&endCount);
				//LOG_INFO("decode time is " <<((double)(endCount.QuadPart-startCount.QuadPart))/freq.QuadPart);
				//��ȡת������
				sws_scale(pPlayer->m_pConvertCtx, (const uint8_t* const*)pFrame->data,
					pFrame->linesize, 0, pPlayer->m_pCodecCtx->height, pFrameRGB->data,
					pFrameRGB->linesize);
				//��ʾͼ��
				PicLenth = pFrameRGB->linesize[0]*pPlayer->m_pCodecCtx->height;
				pPlayer->ShowPicture((char*)(pFrameRGB->data[0]),PicLenth,pPlayer->m_pCodecCtx->width,pPlayer->m_pCodecCtx->height);
				QueryPerformanceCounter(&endCount);
				//LOG_INFO("exchange and show time is " <<((double)(startCount.QuadPart-endCount.QuadPart))/freq.QuadPart);
				delaytime = 35-(((double)(endCount.QuadPart-startCount.QuadPart))/freq.QuadPart)*1000;
				if (delaytime<0)
				{
					delaytime=0;
				}
				//LOG_INFO("show delay time is " << delaytime);
				Sleep(delaytime);
				//SaveFrame(pFrameRGB, m_pCodecCtx->width, m_pCodecCtx->height, 0, "a.bmp");
			}
		}
	}
	av_free_packet(&packet);
	av_free(buffer);
	av_free(pFrame);
	av_free(pFrameRGB);
	return 0;
}

//��ʾ����
int CH264StreamPlayer::ShowPicture(char *data, int length, int Width, int Height)
{
	CRect staRect;
	CWnd *pWnd = CWnd::FromHandle(m_View);
	CDC *pDC = pWnd->GetDC();
	pWnd->GetWindowRect(&staRect);

	BmpInfo.bmiHeader.biWidth=Width;
	BmpInfo.bmiHeader.biHeight=Height;
	if (Height < 0)
	{
		Height = -Height;
	}


	if (NULL != pDC)
	{
		//���ܣ��ú�����������ָ���豸�����е�λͼ����ģʽ
		//COLORONCOLOR��ɾ�����ء���ģʽɾ�����������������У�����������Ϣ��
		if (!SetStretchBltMode(pDC->GetSafeHdc(), COLORONCOLOR))		//��һ������Ҫ������ͼ����ʾ��Ѫ˿����ԭͼ���ز��Ƚϴ�
		{
			DWORD ErrorCode = GetLastError();
		}

		//��ʾͼ��
		if (length > 0)
		{
			StretchDIBits(pDC->GetSafeHdc(), 0, 0, staRect.Width(), staRect.Height(), 0, 0, Width, Height, 
				data, &BmpInfo, DIB_RGB_COLORS, SRCCOPY);
		}

		pDC->DeleteDC();
	}

	return 0;
}

void CH264StreamPlayer::InsertH264Steam( void *Data, int length )
{
	ST_VideoData *pVideoData = new ST_VideoData((uint8_t*)Data,length);
	if (NULL==pVideoData)
	{
		LOG_ERROR("new ST_VideoData, length is " << length);
	}
	m_VideoDataQueue.push(pVideoData);
}
