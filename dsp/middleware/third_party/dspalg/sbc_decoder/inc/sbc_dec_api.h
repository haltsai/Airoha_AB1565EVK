#ifndef _SBC_DEC_API_H_
#define _SBC_DEC_API_H_


#include "xa_type_def.h"
#include "xa_error_standards.h"
#include "xa_error_handler.h"
#include "xa_apicmd_standards.h"
#include "xa_memory_standards.h"
#include "xa_sbc_dec_api.h"
#include "types.h"

#define C_InBuf_Len     512
#define C_OnBuf_Len     512

#if 0
#ifdef WIN32
	#define ALIGN(x) __declspec(align(x))
#else
	#define ALIGN(x) __attribute__((aligned (x)))
#endif /*WIN32*/
#endif

typedef struct
{
	WORD32  i_bitrate;
	WORD32  i_samp_freq;
	WORD32  i_num_chan;
	WORD32  i_pcm_wd_sz;
} SBC_DEC_INFO;

typedef struct
{
	pWORD8  pb_inp_buf;
	pWORD8  pb_out_buf;

	WORD32  ui_inp_size;
	WORD32  i_bytes_consumed;
    WORD32  i_bytes_read;
    WORD32  i_buff_size;
} SBC_DEC_INP_BUF;

typedef struct stru_sbc_header
{
    ALIGN(4) WORD8   API_Buf[30];
    ALIGN(4) WORD8   MEMTABS_Buf[80];
    ALIGN(8) WORD8   Persistent_Buf[2728];
    ALIGN(8) WORD8   Scratch_Buf[1728];
    ALIGN(8) WORD8   Input_Buf[512];
    ALIGN(8) WORD8   Output_Buf[512];
    SBC_DEC_INFO     info;
    SBC_DEC_INP_BUF  inp_buf;
} SBC_DEC_MEM, *SBC_DEC_MEMPtr;

extern XA_ERRORCODE SBC_Init1(SBC_DEC_MEM *param);
extern XA_ERRORCODE SBC_Init2(SBC_DEC_MEM *param, UWORD32 *ui_init_done);
extern XA_ERRORCODE SBC_Decoder_Process(SBC_DEC_MEM *param, WORD32 *o_frame_size, UWORD32 *ui_exec_done);

#endif
