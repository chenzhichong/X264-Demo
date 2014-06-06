#include "stdafx.h"
#include "H264Encoderc.h"

CH264Encoderc::CH264Encoderc(void):m_FrameNum(0),m_pX264Handle(0),m_pPicIn(0),m_Height(0),m_Width(0),m_InitFlag(FALSE)
{
	
}


CH264Encoderc::~CH264Encoderc(void)
{
}

int CH264Encoderc::Initialzie( int Height, int Width, FRAME_CALLBACK func, void *UserData )
{
	if(m_InitFlag)
	{
		return TRUE;
	}
	m_Height=Height;
	m_Width=Width;
	m_FrameCallBACK = func;
	m_UserData = UserData;
	x264_param_t X264Param,*pX264Param;
	pX264Param = &X264Param;


	//* ʹ��Ĭ�ϲ�������������Ϊ�ҵ���ʵʱ���紫�䣬������ʹ����zerolatency��ѡ�ʹ�����ѡ��֮��Ͳ�����delayed_frames�������ʹ�õĲ��������Ļ�������Ҫ�ڱ������֮��õ�����ı���֡   
	x264_param_default_preset(pX264Param, "ultrafast"/*"veryfast"*/, "zerolatency");  

	//* cpuFlags   
	pX264Param->i_threads  = X264_THREADS_AUTO/*X264_SYNC_LOOKAHEAD_AUTO*/;//* ȡ�ջ���������ʹ�ò������ı�֤.  
	pX264Param->i_log_level = X264_LOG_NONE; 
	//* ��Ƶѡ��      
	pX264Param->i_width   = Width; //* Ҫ�����ͼ����.   
	pX264Param->i_height  = Height; //* Ҫ�����ͼ��߶�   
	//pX264Param->i_frame_total = 0; //* ������֡��.��֪����0.   
	//pX264Param->i_keyint_max = 10;   

	//* ������   
	pX264Param->i_bframe  = 0;  
// 	pX264Param->b_open_gop  = 0;  
// 	pX264Param->i_bframe_pyramid = 0;  
// 	pX264Param->i_bframe_adaptive = 0; 

	//* Log����������Ҫ��ӡ������Ϣʱֱ��ע�͵�����   
	//pX264Param->i_log_level  = X264_LOG_DEBUG;  

	//* ���ʿ��Ʋ���   
	//pX264Param->rc.i_bitrate = 1024 * 10;//* ����(������,��λKbps)  

	//* muxing parameters   
	pX264Param->i_fps_den  = 1; //* ֡�ʷ�ĸ   
	pX264Param->i_fps_num  = FRAME_RATE;//* ֡�ʷ���   
// 	pX264Param->i_timebase_den = pX264Param->i_fps_num;  
// 	pX264Param->i_timebase_num = pX264Param->i_fps_den;  
	/*
	//����x264����������С��IDR֡����ƹؼ�֡����ࡣ
	//�����Ĭ��ֵ��fps��10�����Դ������Ƶ���ܺã���С�����fps���
	pX264Param->i_keyint_max = 150;
	pX264Param->i_keyint_min = 50;

	//* ����Profile.ʹ��Baseline profile   
	x264_param_apply_profile(pX264Param, x264_profile_names[0]);  
	
	//��ȫ�ر�����ӦI֡���ߡ�
	pX264Param->i_scenecut_threshold = 0;

	//���������ع��Ƶĸ��Ӷȡ�ֵԽ��Խ�á�����1-5�򵥿��������ص�ϸ�����ȡ�����6��ģʽ���߿���RDO������ʧ���Ż�ģʽ����
	//����8���˶�ʸ����֡��Ԥ��ģʽ����RDO������RDO���������Ӻ�ʱ��
	//ʹ��С��2��ֵ�Ὺ��һ�����ٵġ���������Ԥ��ģʽ��Ч����ͬ������һ����С�� �Cscenecutֵ�����Ƽ��������á�
	//pX264Param->analyse.i_subpel_refine = 1;

	//Ϊmb-tree ratecontrol��Macroblock Tree Ratecontrol����vbv-lookahead���ÿ��õ�֡����������������Ϊ250��
	//����mb-tree���ԣ��������ֵ��õ���׼ȷ�ؽ������Ҳ�������
	//mb-tree��ʹ�õ����ֵ�ǨCrc-lookahead�ͨCkeyint�н�С����һ����
	pX264Param->rc.i_lookahead = 0; 

	//i_luma_deadzone[0]��i_luma_deadzone[1]�ֱ��Ӧinter��intra��ȡֵ��Χ1~32
	//���Կ��Ե�֪�������������ĵ������Զ��������кܴ�Ӱ�죬ֵԽ����������ӦԽ�٣�ռ�ô���Խ��.
	pX264Param->analyse.i_luma_deadzone[0] = 32;
	pX264Param->analyse.i_luma_deadzone[1] = 32;

	//���ʿ���ģʽ��ABR��ƽ�����ʣ���CQP���㶨��������CRF���㶨���ʣ�.
	//ABRģʽ�µ���i_bitrate��CQP�µ���i_qp_constant����QPֵ����Χ0~51��ֵԽ��ͼ��Խģ����Ĭ��23.
	//̫ϸ��������Ҳ�ֱ治������Ϊ�����ӱ����ٶȽ����������������Щ��,CRF�µ���f_rf_constant��f_rf_constant_maxӰ������ٶȺ�ͼ������������������
	pX264Param->rc.i_rc_method = X264_RC_CQP;
	pX264Param->rc.i_qp_constant = 30;

	//����Ӧ������ģʽ����ʹ������Ӧ�����Ļ���x264������ʹ�ý��ٵ�bit��ȱ��ϸ�ڵĳ��������Ӧ����������������Ƶ�ĺ������õط�����ء���������ѡ�
	//0-��ȫ�ر�����Ӧ������;1-��������Ӧ��������������Ƶ֡�ڲ��������;2-����ǰһ֡ǿ�Ⱦ��ߵ��Ա���������ʵ���Եģ���Ĭ��ֵ=1
	pX264Param->rc.i_aq_mode = 0;

	//Ϊ��direct�����͵��˶�ʸ���趨Ԥ��ģʽ�������ֿ�ѡ��ģʽ��spatial���ռ�Ԥ�⣩��temporal��ʱ��Ԥ�⣩��Ĭ�ϣ���spatial��
	//��������Ϊ��none���ر�Ԥ�⣬Ҳ��������Ϊ��auto����x264ȥѡ������Ϊ���õ�ģʽ��x264���ڱ������ʱ����������ѡ��
	pX264Param->analyse.i_direct_mv_pred = X264_DIRECT_PRED_NONE;

	//������ȷ��Ȩ��Ԥ��������P֡ѹ����Խ�߼���ģʽԽ��ʱ��������ģʽ��
	//0 : �ر�; 1 : ��̬��������ԶΪ-1��; 2 : ����ͳ�ƾ�̬֡���ر�Ϊ�������뵭��Ч����ѹ���ʶ����
	//pX264Param->analyse.i_weighted_pred = X264_WEIGHTP_NONE;

	//����ȫ�ֵ��˶�Ԥ�ⷽ����������5��ѡ��dia���ı���������, hex��������������, umh�������ȵĶ�������������
	//esa��ȫ����������tesa���任ȫ����������Ĭ�ϣ���hex��
	pX264Param->analyse.i_me_method = X264_ME_DIA;

	//merange�����˶�������������ط�Χ������hex��dia����Χ��������4-16���أ�Ĭ�Ͼ���16��
	//����umh��esa�����Գ���Ĭ�ϵ� 16���ؽ��д�Χ��������������Ը߷ֱ�����Ƶ�Ϳ����˶���Ƶ���Ժ����á�
	//ע�⣬����umh��esa��tesa������merange�����������ӱ����ʱ��Ĭ�ϣ�16
	pX264Param->analyse.i_me_range = 4;

	//�رջ�����Ϊ�������Ӿ�������psnr��ssim���Ż�����ѡ��ͬʱҲ��ر�һЩ����ͨ��x264���������õ��ڲ��������Ӿ��Ż�������
	//pX264Param->analyse.b_psy = 0;

	//Mixed refs����ϲ��գ�����8��8���п�Ϊ����ȡ�����������Ϊ���ա���������֡���յ�֡������������һЩʱ�����.
	//pX264Param->analyse.b_mixed_references = 0;

	//ͨ���˶����ƶ���ͬʱ�������Ⱥ�ɫ�����ء�������ѡ������ɫ�����ػ�ȡһЩ�ٶȵ�������
	pX264Param->analyse.b_chroma_me = 1;

	//ʹ�����������������������Ч�ʣ�0-�ر�, 1-���ں�����ձ���ʱ����, 2-����ģʽ�¾�����.
	//ѡ��1�ṩ���ٶȺ�Ч�ʼ�Ϻõľ��⣬ѡ��2��������ٶ�.
	//ע�⣺��Ҫ���� �Ccabacѡ����Ч.
	//pX264Param->b_cabac = 1;
	//pX264Param->analyse.i_trellis = 1;

	//ͣ�õ������ݵĶ������������루CABAC��Context Adaptive Binary Arithmetic Coder��������ѹ����
	//�л���Ч�ʽϵ͵ĵ������ݵĿɱ䳤�ȱ��루CAVLC��Context Adaptive Variable Length Coder��ϵͳ��
	//�������ѹ��Ч�ʣ�ͨ��10~20%���ͽ����Ӳ������
	//pX264Param->b_cabac = 1;
	//pX264Param->i_cabac_init_idc = 0;

	//�ر�P֡�������������ߡ�������ʱ�Ļ��طǳ�С������������
	pX264Param->analyse.b_fast_pskip = 0;

	//DCT�����ᶪ������ȥ�����ࡱ��DCT�顣�����ӱ���Ч�ʣ�ͨ��������ʧ���Ժ��ԡ�
	pX264Param->analyse.b_dct_decimate = 1;

	//open-gop��һ�����Ч�ʵı��뼼����������ģʽ:none-ͣ��open-gop;normal-����open-gop;
	//bluray-����open-gop��һ��Ч�ʽϵ͵�open-gop�汾����Ϊnormalģʽ�޷������������.
	//ĳЩ����������ȫ֧Ԯopen-gop�������������Ϊʲô��ѡ�δĬ��Ϊ���á����������open-gop��Ӧ���Ȳ������п����������ŵĽ�������
	pX264Param->b_open_gop = 1;

	//��ȫ�ر�����ȥ���˾������Ƽ�ʹ�á�
	//����H.264��׼�е�����ȥ���˾������Ǹ��Լ۱Ⱥܸߵ�ѡ��, �ر�.
	//pX264Param->b_deblocking_filter = 1;
	//pX264Param->i_deblocking_filter_alphac0 = 0;
	//pX264Param->i_deblocking_filter_beta = 0;

	//ȥ������ȵļ��㣬��Ϊ�ڽ����Ҳ���õ�.
	pX264Param->analyse.b_psnr = 0; //�Ƿ�ʹ�������.
	*/
	//��������ļ���profile�����������������������ֵ����ѡ���ܱ�֤���profile���ݵ���Ƶ����
	//���ʹ�������ѡ������ܽ�������ѹ������ѡ��baseline��main��high
	x264_param_apply_profile(pX264Param, "baseline");
 

 
	//* �򿪱�����
	m_pX264Handle = x264_encoder_open(pX264Param);

	//����X264ͼ������
	m_pPicIn = new x264_picture_t;  
	x264_picture_alloc(m_pPicIn, X264_CSP_I420, Width, Height); 
	m_pPicIn->img.i_csp = X264_CSP_I420;  /* yuv 4:2:0 planar */
	m_pPicIn->img.i_plane = 3; 

	m_InitFlag=true;
	return true;
}

void CH264Encoderc::UnInitialize()
{
	if (m_InitFlag)
	{
		m_InitFlag=FALSE;
		if (NULL != m_pX264Handle)
		{
			//* �رձ��������   
			x264_encoder_close(m_pX264Handle);  
			m_pX264Handle = NULL;  
		}

		//* ���ͼ������   
		if (NULL != m_pPicIn)
		{
			x264_picture_clean(m_pPicIn);  
			delete m_pPicIn;  
			m_pPicIn = NULL; 
		}
	}
}

bool CH264Encoderc::InsertImageData( void *data, int len )
{
	if (!m_InitFlag)
	{
		return FALSE;
	}
	/*unsigned char *YUV,*RGB;
	RGB=(unsigned char *)malloc(m_Height*m_Width*3);
	YUV=(unsigned char *)malloc(m_Height*m_Width*1.5);
	//BGRתRGB
	BRGtoRGB(RGB,(unsigned char*)data,m_Height,m_Width);

	//RGBתYUV
	Convert((unsigned char*)data, YUV,m_Width,m_Height);//RGB to YUV
	*/
	//���ͼ��
	imgPalneCopy(m_pPicIn, (unsigned char*)data, m_Height,m_Width);

	
	/*
	Y����ȫ������һ�飬UV����ʹ��interleave��ʽ�洢 
	YYYY 
	YYYY 
	UVUV 
	*/  

	/*uint8_t * y = m_pPicIn->img.plane[0];  
	uint8_t * v = m_pPicIn->img.plane[1];  
	uint8_t * u = m_pPicIn->img.plane[2];  
	int nPicSize = m_Width * m_Height;
	memcpy(m_pPicIn->img.plane[0], data, nPicSize);  
	for (int i = 0; i < nPicSize/4; i++)  
	{  
		*(u+i) = *((uint8_t*)data + nPicSize + i*2);  
		*(v+i) = *((uint8_t*)data + nPicSize + i*2 + 1);
	} 
	*/

	//
	int iNal = -1;  
	x264_nal_t *pNals = NULL;  
	x264_picture_t *pPicOut,PicOut; 
	pPicOut=&PicOut;
	x264_picture_init(pPicOut);
	int Buflength=0;
	char *pBuf=NULL;
	int BufCount=0;
	//����õ�һ֡

	m_pPicIn->i_type = X264_TYPE_AUTO;
	//m_pPicIn->i_type = X264_TYPE_IDR;
	int frame_size = x264_encoder_encode(m_pX264Handle,&pNals,&iNal,m_pPicIn,pPicOut);
	if(frame_size >0)  
	{  
		for (int i = 0; i < iNal; ++i)  
		{
			Buflength+=pNals[i].i_payload;
			//fwrite(pNals[i].p_payload, pNals[i].i_payload, 1, fp);
		}  
		pBuf = new char[Buflength];
		for (int i = 0; i < iNal; ++i)  
		{
			memcpy(pBuf+BufCount,pNals[i].p_payload,pNals[i].i_payload);
			BufCount += pNals[i].i_payload;
		}  
		if (m_FrameCallBACK)
		{
			int FrameType=EN_FrameType::P_FRAME;
			if(X264_TYPE_IDR==pPicOut->i_type)
			{
				FrameType = EN_FrameType::I_FRAME;
			}
			m_FrameCallBACK(FrameType,pBuf,BufCount,m_UserData);
		}
		delete[] pBuf;
		pBuf=NULL;
	}
	//free (RGB);
	//free (YUV);
	//YUV=NULL;
	return true;
}

/*-----------------����ɫ�ȿռ����YUV420�����ڴ棬�������ڴ���׵�ַ��Ϊָ��---------------
���ǳ�˵��YUV420����planar��ʽ��YUV��ʹ����������ֿ����YUV����������������
һ����άƽ��һ�����ڳ���H264���Ե�YUV������,����CIFͼ���С��YUV����(352*288),��
�ļ���ʼ��û���ļ�ͷ,ֱ�Ӿ���YUV����,�ȴ��һ֡��Y��Ϣ,����Ϊ352*288��byte, Ȼ���ǵ�
һ֡U��Ϣ������352*288/4��byte, ����ǵ�һ֡��V��Ϣ,������352*288/4��byte, ��˿�����
����һ֡�����ܳ�����352*288*1.5,��152064��byte, ������������300֡�Ļ�, ��ô������
���ȼ�Ϊ152064*300=44550KB,��Ҳ����Ϊʲô������300֡CIF��������44M��ԭ��.
---------------------------------------------------------------------------------*/
void CH264Encoderc::Convert(unsigned char *RGB, unsigned char *YUV,unsigned int width,unsigned int height)
{
	//��������
	unsigned int i,x,y,j;
	unsigned char *Y = NULL;
	unsigned char *U = NULL;
	unsigned char *V = NULL;

	Y = YUV;
	U = YUV + width*height;
	V = U + ((width*height)>>2);
	for(y=0; y < height; y++)
		for(x=0; x < width; x++)
		{
			j = y*width + x;
			i = j*3;
			Y[j] = (unsigned char)(DY(RGB[i], RGB[i+1], RGB[i+2]));
			if(x%2 == 1 && y%2 == 1)
			{
				j = (width>>1) * (y>>1) + (x>>1);
				//����i����Ч
				U[j] = (unsigned char)
					((DU(RGB[i  ], RGB[i+1], RGB[i+2]) + 
					DU(RGB[i-3], RGB[i-2], RGB[i-1]) +
					DU(RGB[i  -width*3], RGB[i+1-width*3], RGB[i+2-width*3]) +
					DU(RGB[i-3-width*3], RGB[i-2-width*3], RGB[i-1-width*3]))/4);
				V[j] = (unsigned char)
					((DV(RGB[i  ], RGB[i+1], RGB[i+2]) + 
					DV(RGB[i-3], RGB[i-2], RGB[i-1]) +
					DV(RGB[i  -width*3], RGB[i+1-width*3], RGB[i+2-width*3]) +
					DV(RGB[i-3-width*3], RGB[i-2-width*3], RGB[i-1-width*3]))/4);
			}
		}
}

void CH264Encoderc:: imgPalneCopy(x264_picture_t* pPicIn, unsigned char *YUV1, int height, int width)
{
	//��YUV�������ݷֱ𿽱�������ƽ�棬����plane[i]����ֿ����YUV��������
	unsigned char *p1,*p2;
	p1=YUV1;
	p2=pPicIn->img.plane[0];
	for(int i=0;i<height;i++)
	{
		memcpy(p2,p1,width);
		p1+=width;
		p2+=width;
	}

	p2=pPicIn->img.plane[1];
	for(int i=0;i<height/2;i++)
	{
		memcpy(p2,p1,width/2);
		p1+=width/2;
		p2+=width/2;
	}

	p2=pPicIn->img.plane[2];
	for(int i=0;i<height/2;i++)
	{
		memcpy(p2,p1,width/2);
		p1+=width/2;
		p2+=width/2;
	}
}

void CH264Encoderc::BRGtoRGB(unsigned char *RGB, unsigned char *imageData, int height, int width)
{
	//pFrame��BGRתRGB
	for(int i=0;i<height;i++)
	{
		for(int j=0;j<width;j++)
		{
			//��opencv��pFrame��ȡλͼRGB����,����BGRתRGB
			RGB[(i*width+j)*3]   = imageData[i *width + j * 3 + 2];
			RGB[(i*width+j)*3+1] = imageData[i *width + j * 3 + 1];
			RGB[(i*width+j)*3+2] = imageData[i *width + j * 3  ]; 
		}
	}
}