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


	//* 使用默认参数，在这里因为我的是实时网络传输，所以我使用了zerolatency的选项，使用这个选项之后就不会有delayed_frames，如果你使用的不是这样的话，还需要在编码完成之后得到缓存的编码帧   
	x264_param_default_preset(pX264Param, "ultrafast"/*"veryfast"*/, "zerolatency");  

	//* cpuFlags   
	pX264Param->i_threads  = X264_THREADS_AUTO/*X264_SYNC_LOOKAHEAD_AUTO*/;//* 取空缓冲区继续使用不死锁的保证.  
	pX264Param->i_log_level = X264_LOG_NONE; 
	//* 视频选项      
	pX264Param->i_width   = Width; //* 要编码的图像宽度.   
	pX264Param->i_height  = Height; //* 要编码的图像高度   
	//pX264Param->i_frame_total = 0; //* 编码总帧数.不知道用0.   
	//pX264Param->i_keyint_max = 10;   

	//* 流参数   
	pX264Param->i_bframe  = 0;  
// 	pX264Param->b_open_gop  = 0;  
// 	pX264Param->i_bframe_pyramid = 0;  
// 	pX264Param->i_bframe_adaptive = 0; 

	//* Log参数，不需要打印编码信息时直接注释掉就行   
	//pX264Param->i_log_level  = X264_LOG_DEBUG;  

	//* 速率控制参数   
	//pX264Param->rc.i_bitrate = 1024 * 10;//* 码率(比特率,单位Kbps)  

	//* muxing parameters   
	pX264Param->i_fps_den  = 1; //* 帧率分母   
	pX264Param->i_fps_num  = FRAME_RATE;//* 帧率分子   
// 	pX264Param->i_timebase_den = pX264Param->i_fps_num;  
// 	pX264Param->i_timebase_num = pX264Param->i_fps_den;  
	/*
	//设置x264输出中最大、最小的IDR帧（亦称关键帧）间距。
	//最大间距默认值（fps的10倍）对大多数视频都很好；最小间距与fps相等
	pX264Param->i_keyint_max = 150;
	pX264Param->i_keyint_min = 50;

	//* 设置Profile.使用Baseline profile   
	x264_param_apply_profile(pX264Param, x264_profile_names[0]);  
	
	//完全关闭自适应I帧决策。
	pX264Param->i_scenecut_threshold = 0;

	//设置亚像素估计的复杂度。值越高越好。级别1-5简单控制亚像素的细化力度。级别6给模式决策开启RDO（码率失真优化模式），
	//级别8给运动矢量和帧内预测模式开启RDO。开启RDO会显著增加耗时。
	//使用小于2的值会开启一个快速的、低质量的预测模式，效果如同设置了一个很小的 –scenecut值。不推荐这样设置。
	//pX264Param->analyse.i_subpel_refine = 1;

	//为mb-tree ratecontrol（Macroblock Tree Ratecontrol）和vbv-lookahead设置可用的帧的数量。最大可设置为250。
	//对于mb-tree而言，调大这个值会得到更准确地结果，但也会更慢。
	//mb-tree能使用的最大值是–rc-lookahead和–keyint中较小的那一个。
	pX264Param->rc.i_lookahead = 0; 

	//i_luma_deadzone[0]和i_luma_deadzone[1]分别对应inter和intra，取值范围1~32
	//测试可以得知，这连个参数的调整可以对数据量有很大影响，值越大数据量相应越少，占用带宽越低.
	pX264Param->analyse.i_luma_deadzone[0] = 32;
	pX264Param->analyse.i_luma_deadzone[1] = 32;

	//码率控制模式有ABR（平均码率）、CQP（恒定质量）、CRF（恒定码率）.
	//ABR模式下调整i_bitrate，CQP下调整i_qp_constant调整QP值，范围0~51，值越大图像越模糊，默认23.
	//太细致了人眼也分辨不出来，为了增加编码速度降低数据量还是设大些好,CRF下调整f_rf_constant和f_rf_constant_max影响编码速度和图像质量（数据量）；
	pX264Param->rc.i_rc_method = X264_RC_CQP;
	pX264Param->rc.i_qp_constant = 30;

	//自适应量化器模式。不使用自适应量化的话，x264趋向于使用较少的bit在缺乏细节的场景里。自适应量化可以在整个视频的宏块里更好地分配比特。它有以下选项：
	//0-完全关闭自适应量化器;1-允许自适应量化器在所有视频帧内部分配比特;2-根据前一帧强度决策的自变量化器（实验性的）。默认值=1
	pX264Param->rc.i_aq_mode = 0;

	//为’direct’类型的运动矢量设定预测模式。有两种可选的模式：spatial（空间预测）和temporal（时间预测）。默认：’spatial’
	//可以设置为’none’关闭预测，也可以设置为’auto’让x264去选择它认为更好的模式，x264会在编码结束时告诉你它的选择。
	pX264Param->analyse.i_direct_mv_pred = X264_DIRECT_PRED_NONE;

	//开启明确的权重预测以增进P帧压缩。越高级的模式越耗时，有以下模式：
	//0 : 关闭; 1 : 静态补偿（永远为-1）; 2 : 智能统计静态帧，特别为增进淡入淡出效果的压缩率而设计
	//pX264Param->analyse.i_weighted_pred = X264_WEIGHTP_NONE;

	//设置全局的运动预测方法，有以下5种选择：dia（四边形搜索）, hex（六边形搜索）, umh（不均匀的多六边形搜索）
	//esa（全局搜索），tesa（变换全局搜索），默认：’hex’
	pX264Param->analyse.i_me_method = X264_ME_DIA;

	//merange控制运动搜索的最大像素范围。对于hex和dia，范围被控制在4-16像素，默认就是16。
	//对于umh和esa，可以超过默认的 16像素进行大范围的运行搜索，这对高分辨率视频和快速运动视频而言很有用。
	//注意，对于umh、esa、tesa，增大merange会显著地增加编码耗时。默认：16
	pX264Param->analyse.i_me_range = 4;

	//关闭或启动为了心理视觉而降低psnr或ssim的优化。此选项同时也会关闭一些不能通过x264命令行设置的内部的心理视觉优化方法。
	//pX264Param->analyse.b_psy = 0;

	//Mixed refs（混合参照）会以8×8的切块为参照取代以整个宏块为参照。会增进多帧参照的帧的质量，会有一些时间耗用.
	//pX264Param->analyse.b_mixed_references = 0;

	//通常运动估计都会同时考虑亮度和色度因素。开启此选项将会忽略色度因素换取一些速度的提升。
	pX264Param->analyse.b_chroma_me = 1;

	//使用网格编码量化以增进编码效率：0-关闭, 1-仅在宏块最终编码时启用, 2-所有模式下均启用.
	//选项1提供了速度和效率间较好的均衡，选项2大幅降低速度.
	//注意：需要开启 –cabac选项生效.
	//pX264Param->b_cabac = 1;
	//pX264Param->analyse.i_trellis = 1;

	//停用弹性内容的二进制算数编码（CABAC：Context Adaptive Binary Arithmetic Coder）资料流压缩，
	//切换回效率较低的弹性内容的可变长度编码（CAVLC：Context Adaptive Variable Length Coder）系统。
	//大幅降低压缩效率（通常10~20%）和解码的硬件需求。
	//pX264Param->b_cabac = 1;
	//pX264Param->i_cabac_init_idc = 0;

	//关闭P帧的早期跳过决策。大量的时耗换回非常小的质量提升。
	pX264Param->analyse.b_fast_pskip = 0;

	//DCT抽样会丢弃看上去“多余”的DCT块。会增加编码效率，通常质量损失可以忽略。
	pX264Param->analyse.b_dct_decimate = 1;

	//open-gop是一个提高效率的编码技术。有三种模式:none-停用open-gop;normal-启用open-gop;
	//bluray-启用open-gop。一个效率较低的open-gop版本，因为normal模式无法用于蓝光编码.
	//某些解码器不完全支援open-gop资料流，这就是为什么此选项并未默认为启用。如果想启用open-gop，应该先测试所有可能用来拨放的解码器。
	pX264Param->b_open_gop = 1;

	//完全关闭内置去块滤镜，不推荐使用。
	//调节H.264标准中的内置去块滤镜。这是个性价比很高的选则, 关闭.
	//pX264Param->b_deblocking_filter = 1;
	//pX264Param->i_deblocking_filter_alphac0 = 0;
	//pX264Param->i_deblocking_filter_beta = 0;

	//去掉信噪比的计算，因为在解码端也可用到.
	pX264Param->analyse.b_psnr = 0; //是否使用信噪比.
	*/
	//限制输出文件的profile。这个参数将覆盖其它所有值，此选项能保证输出profile兼容的视频流。
	//如果使用了这个选项，将不能进行无损压缩。可选：baseline，main，high
	x264_param_apply_profile(pX264Param, "baseline");
 

 
	//* 打开编码器
	m_pX264Handle = x264_encoder_open(pX264Param);

	//创建X264图像容器
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
			//* 关闭编码器句柄   
			x264_encoder_close(m_pX264Handle);  
			m_pX264Handle = NULL;  
		}

		//* 清除图像区域   
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
	//BGR转RGB
	BRGtoRGB(RGB,(unsigned char*)data,m_Height,m_Width);

	//RGB转YUV
	Convert((unsigned char*)data, YUV,m_Width,m_Height);//RGB to YUV
	*/
	//填充图像
	imgPalneCopy(m_pPicIn, (unsigned char*)data, m_Height,m_Width);

	
	/*
	Y数据全部存在一块，UV数据使用interleave方式存储 
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
	//编码得到一帧

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

/*-----------------按照色度空间分配YUV420类型内存，并返回内存的首地址作为指针---------------
我们常说得YUV420属于planar格式的YUV，使用三个数组分开存放YUV三个分量，就像是
一个三维平面一样。在常见H264测试的YUV序列中,例如CIF图像大小的YUV序列(352*288),在
文件开始并没有文件头,直接就是YUV数据,先存第一帧的Y信息,长度为352*288个byte, 然后是第
一帧U信息长度是352*288/4个byte, 最后是第一帧的V信息,长度是352*288/4个byte, 因此可以算
出第一帧数据总长度是352*288*1.5,即152064个byte, 如果这个序列是300帧的话, 那么序列总
长度即为152064*300=44550KB,这也就是为什么常见的300帧CIF序列总是44M的原因.
---------------------------------------------------------------------------------*/
void CH264Encoderc::Convert(unsigned char *RGB, unsigned char *YUV,unsigned int width,unsigned int height)
{
	//变量声明
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
				//上面i仍有效
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
	//把YUV分量数据分别拷贝到三个平面，三个plane[i]数组分开存放YUV三个分量
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
	//pFrame中BGR转RGB
	for(int i=0;i<height;i++)
	{
		for(int j=0;j<width;j++)
		{
			//从opencv的pFrame中取位图RGB数据,并把BGR转RGB
			RGB[(i*width+j)*3]   = imageData[i *width + j * 3 + 2];
			RGB[(i*width+j)*3+1] = imageData[i *width + j * 3 + 1];
			RGB[(i*width+j)*3+2] = imageData[i *width + j * 3  ]; 
		}
	}
}