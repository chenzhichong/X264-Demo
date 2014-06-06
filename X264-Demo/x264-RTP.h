#include <stdint.h>

#define MAX_RTP_PKT_LENGTH 1360
#define H264               125
#define NALU_TYPE_FU_A     28
#define H264_PACKET_END	   9	//H264һ֡ͼ������һ����,û�а����κ�����,ֻ�ǵ����ķֽ��.
typedef enum EN_H264_NALU_TYPE
{
	UnKnow=0,				//δʹ��
	NOT_IDR,				//�����֣���IDR��Ƭ
	IDR=5,					//IDR
	SEI,					//������ǿ��Ϣ��Ԫ
	SPS,					//���в�����
	PPS,					//ͼ�������
};

enum SLICE_TYPE
{
	P=0,
	B,
	I,
	SP,
	SI,
	P_,
	B_,
	I_,
	SP_,
	SI_,
};

typedef struct 
{
	/**//* byte 0 */
	unsigned char csrc_len:4;        /**//* expect 0 */
	unsigned char extension:1;        /**//* expect 1, see RTP_OP below */
	unsigned char padding:1;        /**//* expect 0 */
	unsigned char version:2;        /**//* expect 2 */
	/**//* byte 1 */
	unsigned char payload:7;        /**//* RTP_PAYLOAD_RTSP */
	unsigned char marker:1;        /**//* expect 1 */
	/**//* bytes 2, 3 */
	unsigned short seq_no;            
	/**//* bytes 4-7 */
	unsigned  long timestamp;        
	/**//* bytes 8-11 */
	unsigned long ssrc;            /**//* stream number is used here. */
} RTP_FIXED_HEADER;

typedef struct {
	//byte 0
	unsigned char TYPE:5;
	unsigned char NRI:2;
	unsigned char F:1;    

} NALU_HEADER; /**//* 1 BYTES */

typedef struct {
	unsigned char Resv:1;
	unsigned char PicParameterSetID:1;
	unsigned char SliceType:5;
	unsigned char FirstMBInSlice:1;
}H264_P_BitStream;

typedef struct {
	unsigned char SliceType:7;
	unsigned char FirstMBInSlice:1;
}H264_I_BitStream;

typedef union H264_BitStream
{
	H264_P_BitStream PBitStream;
	H264_I_BitStream IBitStream;
	bool GetFirstMBInSlice()
	{
		return PBitStream.FirstMBInSlice==1?TRUE:FALSE;
	}
	bool isIFrame()
	{
		if (IBitStream.SliceType==SLICE_TYPE::I_+1||IBitStream.SliceType==SLICE_TYPE::I+1)
		{
			return TRUE;
		}
		return FALSE;
	}
	bool isPFrame()
	{
		if (IBitStream.SliceType==SLICE_TYPE::P||IBitStream.SliceType-1==SLICE_TYPE::P_+1)
		{
			return TRUE;
		}
		return FALSE;
	}
};

typedef struct {
	//byte 0
	unsigned char TYPE:5;
	unsigned char NRI:2; 
	unsigned char F:1;    


} FU_INDICATOR; /**//* 1 BYTES */

typedef struct {
	//byte 0
	unsigned char TYPE:5;
	unsigned char R:1;
	unsigned char E:1;
	unsigned char S:1;    
} FU_HEADER; /**//* 1 BYTES */

struct NALU_t
{
	int startcodeprefix_len;      //! 4 for parameter sets and first slice in picture, 3 for everything else (suggested)
	unsigned len;                 //! Length of the NAL unit (Excluding the start code, which does not belong to the NALU)
	unsigned max_size;            //! Nal Unit Buffer size
	int forbidden_bit;            //! should be always FALSE
	int nal_reference_idc;        //! NALU_PRIORITY_xxxx
	int nal_unit_type;            //! NALU_TYPE_xxxx    
	uint8_t *buf;                    //! contains the first byte followed by the EBSP
	unsigned short lost_packets;  //! true, if packet loss is detected
};