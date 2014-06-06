#pragma once
extern "C"
{
#include "x264/x264.h"
};

#include "Common.h"

#pragma comment (lib, "x264/libx264-142.lib")
class CH264Encoderc
{
public:
	CH264Encoderc(void);
	~CH264Encoderc(void);
	int Initialzie(int Height, int Width, FRAME_CALLBACK func, void *UserData);
	void UnInitialize();
	bool InsertImageData(void *data, int length);
protected:
	void Convert(unsigned char *RGB, unsigned char *YUV,unsigned int width,unsigned int height);
	void imgPalneCopy(x264_picture_t* pPicIn, unsigned char *YUV1, int height, int width);
	void BRGtoRGB(unsigned char *RGB, unsigned char *imageData, int height, int width);
private:
	int m_FrameNum;
	x264_t* m_pX264Handle; 
	x264_picture_t* m_pPicIn;
	int m_Height;
	int m_Width;

	FRAME_CALLBACK m_FrameCallBACK;
	void *m_UserData;
	int m_InitFlag;
};


