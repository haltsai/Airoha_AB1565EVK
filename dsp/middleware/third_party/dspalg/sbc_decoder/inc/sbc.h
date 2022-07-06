#ifndef _SBC_H
#define _SBC_H
//#include <stdio.h>
//#include "Zsp410.h"
#include "types.h"
#ifndef _WIN32
#include <xtensa/tie/xt_hifi2.h>
#include <xtensa/tie/xt_misc.h>
#endif

//#include "SBC_alloc.h"

//#define FILE_OPERATION
#define SBC_SCAN_SYNC_WORD

#ifndef _WIN32
#define XTENSA
#endif
#ifdef XTENSA
#define XT_ALIGN4 __attribute__((aligned(4)))
#else
#define XT_ALIGN4 __declspec(align(8))
#endif
typedef int		Q31;
typedef short	Q15;


//#define DECLARE_SBCDEC_TAB __attribute__((section(".coef_rom,\"aw\"")))
//#define sbcdec_const DECLARE_SBCDEC_TAB const
#define sbcdec_const const

#if 0
#define FS16K	0
#define FS32K	1
#define FS44_1K	2
#define FS48K	3

#define SBC_MONO_CHANNEL	0
#define SBC_DUAL_CHANNEL	1
#define SBC_STEREO_CHANNEL	2
#define SBC_JOINT_STEREO_CHANNEL	3
#else
enum SBC_CHANNEL_e
{
	SBC_MONO_CHANNEL            = 0,
	SBC_DUAL_CHANNEL            = 1,
	SBC_STEREO_CHANNEL          = 2,
	SBC_JOINT_STEREO_CHANNEL    = 3,
};
#endif

#define LOUDNESS_ALLOC_METOHD	0
#define SNR_ALLOC_METHOD		1


#define SBC_OVL_COMPENSATE_TH_DEFAULT	0x7500

#define MSBC_BAD_FRAMES_MUTE  32

/* Parameter variables (begin) */
extern S16 SBC_OUT_ENV_ATTACK_RATE;
extern S16 SBC_OUT_ENV_RELEASE_RATE;
/* Parameter variables (end) */

//Error codes;
#define SBC_NO_ERROR									0x0000
#define SBC_ERR_READ_NOT_COMPLETE						0x0001
#define SBC_ERR_READ_NOT_COMPLETE_DUE_TO_EOF			0x0002
#define SBC_ERR_SYNCWORD_ERROR							0x0003
#define SBC_ERR_SET_WRONG_SECTION_FOR_READ_BITS			0x0004
#define SBC_ERR_WRITE_ERROR								0x0005

#define SBC_WARNING_OUT_OF_SBC_SYNC						0x0010

extern U16 bit_count;
/*
extern U16 SBC_FRAME_WORD;
extern U16 SBC_FRAME_WORD_BITS_LEFT;

extern U16 mSBC_frame;
extern U16 SBC_FS;
extern U16 SBC_BLOCKS;
extern U16 SBC_CHANNEL_MODE;
extern U16 SBC_ALLOC_METHOD;
extern U16 SBC_SUBBANDS;
extern U16 SBC_CHANNELS;
extern U16 SBC_BITPOOL;
extern U16 SBC_CRC;
extern U16 SBC_JOIN;

extern U16 SBC_SAMPLES_PER_BLOCK;
extern U16 SBC_SAMPLES_PER_FRAME;

extern U16 SBC_SCALE_FACTOR[16];
//extern S16 SBC_SB_SAMPLES[256];
extern S32 SBC_SB_SAMPLES[256];
//extern S16 SBC_SB_SAMPLES_ASM[256];
extern U16 SBC_CRC_DATA[12];
extern U16 JOIN_IDX[8];
extern U16 *JOIN_IDX_PTR;
*/
//extern U16 SBC_BITS[16];

extern U16 SBC_FRAME_BUF[512];

#if 1
#include "dsp_rom_table.h"
#else
extern const S16 SBC_OFFSET4[4][4];
extern const S16 SBC_OFFSET8[4][8];

//extern U16 SBC_FRAME_LENGTH;
//extern U16 *SBC_FRAME_BUF_PTR;

extern const S16 G_LEVEL[17];
extern const S16 S_OFFSET[17];
extern const S32 S_ARRAY[17];
//extern const S16 SBC_cos_coef_4[12*4];
//extern const S16 SBC_cos_coef_8[24*8];
extern const S32 SBC_cos_coef_4[12*4];
extern const S32 SBC_cos_coef_8[24*8];
//extern const S16 SBC_proto_coef_4[4*11*5];
//extern const S16 SBC_proto_coef_8[8*11*5];
extern const S32 SBC_proto_coef_4[4*11*5];
extern const S32 SBC_proto_coef_8[8*11*5];
extern const U16 SBC_crc_table[128];
#endif

//extern S16 SYNTHESIS_BUF0[160];
//extern S16 SYNTHESIS_BUF1[160];
//extern S16 *SYNTHESIS_CBUF_ADDR;
//extern U16 SYNTHESIS_CBUF_PTR;
//extern U16 SYNTHESIS_CBUF_PTR0;
//extern U16 SYNTHESIS_CBUF_PTR1;
//extern S32 SYNTHESIS_BUF0[160];
//extern S32 SYNTHESIS_BUF1[160];
//extern S32 *SYNTHESIS_CBUF_ADDR;
//extern U32 SYNTHESIS_CBUF_PTR;
//extern U32 SYNTHESIS_CBUF_PTR0;
//extern U32 SYNTHESIS_CBUF_PTR1;

extern S16 SBC_AUD_SAMPLES[256];

extern S16 PLC_RC_WINDOW32_RL[32];

extern U32 square_sum;
extern U32 number_of_point;

typedef struct
{
	//U16 bit_count;	//never referenced?
	U16 SBC_FRAME_WORD;
	U16 SBC_FRAME_WORD_BITS_LEFT;
	U16 SBC_FS;
	U16 SBC_BLOCKS;
	U16 SBC_CHANNEL_MODE;
	U16 SBC_ALLOC_METHOD;
	U16 SBC_SUBBANDS;
	U16 SBC_CHANNELS;
	U16 SBC_BITPOOL;
	U16 SBC_CRC_DATA[12];
	U16 SBC_CRC_LENGTH;
	U16 SBC_CRC_CHECK;
	U16 SBC_JOIN;
	U16 OUTPUT_SAMPLES;
	U16 DECODE_FRAME_NUMBER;
	//U16 PINGPONG_BUFSZ;	//never referenced
	U16 SBC_SAMPLES_PER_BLOCK;
	U16 SBC_SAMPLES_PER_FRAME;
	U16 SBC_SCALE_FACTOR[16];
	S32 SBC_SB_SAMPLES[256];
	U16 JOIN_IDX[8];
	U16 *JOIN_IDX_PTR;
	U16 SBC_BITS[16];
	//S16 SBC_OFFSET4[4][4];	//moved to table
	//S16 SBC_OFFSET8[4][8];	//moved to table
	U16 SBC_FRAME_LENGTH;
	//U16 SBC_FRAME_BUF[512];
	U16 *SBC_FRAME_BUF_PTR;
	ALIGN(8) S32 SYNTHESIS_BUF0[160];
	ALIGN(8) S32 SYNTHESIS_BUF1[160];
	S32 *SYNTHESIS_CBUF_ADDR;
	U16 SYNTHESIS_CBUF_PTR;
	U16 SYNTHESIS_CBUF_PTR0;
	U16 SYNTHESIS_CBUF_PTR1;
	//S16 SBC_AUD_SAMPLES[128];
	//S16 SBC_AUD_SAMPLES_L[128];
	//S16 SBC_AUD_SAMPLES_R[128];
	S16 BITNEED[16];
	S16 SBC_OFFSET[16];
	S16 F_SHIFT[16];
	U8* SBC_PACKET_BUF_PTR;
	U16 SBC_FRAME_BUF[512];
} SBC_DEC_STATE;

#define sbc_frame_bits_extract_MACRO(st_ptr,ret, n_bit) {\
	U16 tmp;\
	if (((U16) (n_bit)) > st_ptr->SBC_FRAME_WORD_BITS_LEFT) {\
		if (st_ptr->SBC_FRAME_WORD_BITS_LEFT == 0) {\
			st_ptr->SBC_FRAME_WORD = *(st_ptr->SBC_FRAME_BUF_PTR++);\
			st_ptr->SBC_FRAME_WORD_BITS_LEFT = 16 - n_bit;\
			ret = st_ptr->SBC_FRAME_WORD >> st_ptr->SBC_FRAME_WORD_BITS_LEFT;\
			st_ptr->SBC_FRAME_WORD <<= n_bit;\
		}\
		else {\
			tmp = st_ptr->SBC_FRAME_WORD >> (16 - st_ptr->SBC_FRAME_WORD_BITS_LEFT);\
			st_ptr->SBC_FRAME_WORD = *(st_ptr->SBC_FRAME_BUF_PTR++);\
			tmp <<= (n_bit - st_ptr->SBC_FRAME_WORD_BITS_LEFT);\
			st_ptr->SBC_FRAME_WORD_BITS_LEFT = 16 - n_bit + st_ptr->SBC_FRAME_WORD_BITS_LEFT;\
			ret = (S16) (tmp | (st_ptr->SBC_FRAME_WORD >> st_ptr->SBC_FRAME_WORD_BITS_LEFT));\
			st_ptr->SBC_FRAME_WORD <<= (16 - st_ptr->SBC_FRAME_WORD_BITS_LEFT);\
		}\
	}\
	else {\
		ret = (S16) (st_ptr->SBC_FRAME_WORD >> (16 - n_bit));\
		st_ptr->SBC_FRAME_WORD = st_ptr->SBC_FRAME_WORD << n_bit;\
		st_ptr->SBC_FRAME_WORD_BITS_LEFT = st_ptr->SBC_FRAME_WORD_BITS_LEFT - n_bit;\
	}\
};\

#define sbc_frame_bits_extract_N(ret, n_bit) {\
	S16 bits_next_word;\
	bits_next_word = (S16) (n_bit - SBC_FRAME_WORD_BITS_LEFT);\
	if (bits_next_word > 0) {\
		U16 tmp;\
		tmp = SBC_FRAME_WORD >> ((U16) (16 - SBC_FRAME_WORD_BITS_LEFT));\
		SBC_FRAME_WORD = *(SBC_FRAME_BUF_PTR++);\
		N_shla_s(tmp, bits_next_word);\
		SBC_FRAME_WORD_BITS_LEFT = ((U16) (16 - bits_next_word));\
		ret = (S16) (tmp | (SBC_FRAME_WORD >> SBC_FRAME_WORD_BITS_LEFT));\
		N_shla_s(SBC_FRAME_WORD, bits_next_word);\
	}\
	else {\
		ret = (S16) (SBC_FRAME_WORD >> ((U16) (16 - n_bit)));\
		N_shla_s(SBC_FRAME_WORD, n_bit);\
		SBC_FRAME_WORD_BITS_LEFT = (U16) (-bits_next_word);\
	}\
};

#define SBC_Q_FORMAT_NORMALIZATION						32768
#define SBC_Q_FORMAT_NORMALIZATION_1					(-104858.0)
#define SBC_Q_FORMAT_NORMALIZATION_2					(-209715.0)
#define SBC_Q_FORMAT_NORMALIZATION_3					2147483648LL	//KCH from NORM
#define SBC_Q_FORMAT_NORMALIZATION_4					-13743882240LL	//KCH from NORM_2
#define SBC_Q_FORMAT_NORMALIZATION_5					-6871973888LL	//KCH from NORM_1

// SBC functions
void sbc_decoder_init(void);
void sbc_init(SBC_DEC_STATE*);
U16 sbc_get_bytes(SBC_DEC_STATE* st_ptr, U16 byte_count);
U16 sbc_frame_bits_extract(U16 n_bit);
void sbc_extract_scale_factors(void);
U16 sbc_get_frame_length (SBC_DEC_STATE* st_ptr);
U16 sbc_get_sample_freq (SBC_DEC_STATE* st_ptr);
U16 sbc_frame_header_decode(U16 *is_mSBC_frame, SBC_DEC_STATE*);
U16 sbc_frame_decode(SBC_DEC_STATE*, void* OutBufL, void* OutBufR);
U16 sbc_header_parse(U16 SBC_FRAME_HEADER_WORD0, U16 SBC_FRAME_HEADER_WORD1, SBC_DEC_STATE*);
void sbc_bit_allocation(SBC_DEC_STATE*);
void sbc_reconstruction(SBC_DEC_STATE*);
void SBC_reconstruction_ASM(U16 *SBC_BITS_ptr, U16 SBC_SCALE_FACTOR_ptr);

void sbc_synthesis_setup(void);
void SBC_synthesis4_per_channel(U16 ch, S16 *sbc_audio_sample, SBC_DEC_STATE*);
void SBC_synthesis8_per_channel(U16 ch, S16 *sbc_audio_sample, SBC_DEC_STATE*);
void sbc_extract_scale_factors_non_joint(SBC_DEC_STATE*);

void sbc_aud_out_fade_out_set(void);

void msbc_init(SBC_DEC_STATE* st_ptr);
U16 msbc_frame_decode(U16 force_as_bad_frame, U16 force_mute, U16 *crc_fail, SBC_DEC_STATE*);
void sbc_zero_sb_samples(SBC_DEC_STATE* st_ptr);

//msbc decoder
void  msbc_dec_init(SBC_DEC_STATE* st_ptr);

/* AB1520 Legacy */
typedef	union
{
	U8 sbc_byte2;
	struct
	{
		U8 SUBBANDS:1;
		U8 ALLOCATION_METHOD:1;
		U8 CHANNEL_MODE:2;
		U8 BLOCKS:2;
		U8 SAMPLING_FREQ:2;
	}bit_alloc;
} SBC_BYTE2;

typedef struct stru_audio_frame_header
{
	U8 SyncWord;
	SBC_BYTE2 Byte1;
	U8 Bitpool;
	U8 RSVD[10];
} SBCFrameHeaderStru, *SBCFrameHeaderStruPtr;


#endif
