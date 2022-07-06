/*
 * Copyright (c) 2006-2018 Cadence Design Systems, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <string.h>
#include <xtensa/tie/xt_hifi2.h>
#include <xtensa/hal.h>
#include "xa_type_def.h"
#include "xa_error_standards.h"
#include "xa_error_handler.h"
#include "xa_aac_dec_api.h"
#include "aac_dec_api_internal.h"

#ifdef MTK_BT_A2DP_AAC_USE_PIC
#include "aac_decoder_portable.h"
#endif


#define OPEN_AAC_DEBUG
#define UNUSED(p) ((void)(p))

/*****************************************************************
 * decoder memory                                                 *
 *****************************************************************/
xa_error_info_struct *p_proc_err_info;
xa_codec_handle_t p_xa_process_api_obj;
xa_codec_func_t *p_xa_process_api;
//---------------------------------------------xa_aac_error_handler ----------
#ifdef OPEN_AAC_DEBUG1
const char *ppb_xa_aacmch_dec_api_non_fatal[] = {
  "API Command type not supported",
  "Invalid API Sequence "
};

const char *ppb_xa_aacmch_dec_api_fatal[] = {
  "NULL Pointer: Memory Allocation Error",
  "Memory Allocation Error: Alignment requirement not met",
  "Invalid Command",
  "Invalid Command Type/Index",
  "Invalid API Sequence "
};


const char *ppb_xa_aacmch_dec_config_non_fatal[] = {
  "Config parameters not yet set",
  "bit-rate is not set",
  "Incomplete channel routemap"
  ,"Warning: Stream Position for the decoded-output may be wrong"
  ,"CopyrightID not present in bitstream (applicable only for ADTS and ADIF streams)"
  ,"Warning: Invalid PRL parameter setting"
  ,"Warning: Invalid DRC parameter setting"
};

/* Fatal Errors */
const char *ppb_xa_aacmch_dec_config_fatal[] = {
  "Invalid downsample flag",
  "Invalid downmix to mono flag",
  "Invalid external sample rate for raw bit stream",
  "Invalid bit stream format",
  "Invalid value of stereo flag",
  "Invalid output number of channels",
  "Invalid SBR signaling variable",
  "Invalid channel routemap",
  "Invalid PCM Sample Size",
  "Invalid Zero_unused_chans parameter value",
  "Invaild Number of input Channels",
  "Invaild Number of Decoding Layers",
  "Invaild Setting of External BitRate"
  ,"Invaild Setting of fade-in or fade-out or mute-release frames"
};

/*****************************************************************************/
/* Class 2: Execution Class Errors                                           */
/*****************************************************************************/
/* Nonfatal Errors */
const char *ppb_xa_aacmch_dec_execute_non_fatal[] = {
  "Insufficient Frame Data",
  "RampDown Active for Runtime_Init"
  ,"Non-Fatal Raw Frame Parsing Error"
  ,"Warning: Valid ADTS Header Error"
  ,"Warning: Valid ADTS Header Not Found"
  ,"Warning: Valid DABPlus Header Not Found"
  ,"Warning: Valid Loas Header Not Found"
  ,"Warning: Stream Change Detected - Calls to init process for codec is required !! "
  ,"Warning: Valid Header Not Found"
  ,"Warning: Unsupported Feature"
  ,"Warning: Transport Header Error"
  ,"Warning: Partial Last Frame"
  ,"Warning: Empty input buffer"
};
/* Fatal Errors */
const char *ppb_xa_aacmch_dec_execute_fatal[] = {
  "Fatal Transport Parsing Error",
  "Fatal Raw Frame Parsing Error",
  "Fatal Decoding Error due to Unsupported Format or BAD Input",
  "Fatal Decoding Error due to Unsupported Feature",
  "Fatal Incomplete or Errorneous channel routing",
  "Fatal Empty input buffer",
  "Fatal LOAS header change",
  "Fatal Init Error",
  "Fatal unknown bitstream format",
  "Fatal ADIF header not found"
};
#endif


#ifdef OPEN_AAC_DEBUG
const xa_error_info_struct xa_aac_dec_error_info =
{
	/* The Module Name	*/
  "",
  {
    /* The Class Names	*/
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    ""
  },
  {
    /* The Message Pointers	*/
    { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }
  }
};
#endif

void InitBuffer(inp_buffer_t *pbuf, void *ptr, int size);

void xa_aac_dec_error_handler_init()
{
#ifdef OPEN_AAC_DEBUG1
	/* The Message Pointers	*/
	xa_aac_dec_error_info.ppppb_error_msg_pointers[0][ 0] =
		ppb_xa_aacmch_dec_api_non_fatal;
	xa_aac_dec_error_info.ppppb_error_msg_pointers[1][ 0] =
		ppb_xa_aacmch_dec_api_fatal;
	xa_aac_dec_error_info.ppppb_error_msg_pointers[0][ 1] =
		ppb_xa_aacmch_dec_config_non_fatal;
	xa_aac_dec_error_info.ppppb_error_msg_pointers[1][ 1] =
		ppb_xa_aacmch_dec_config_fatal;
	xa_aac_dec_error_info.ppppb_error_msg_pointers[0][ 2] =
		ppb_xa_aacmch_dec_execute_non_fatal;
	xa_aac_dec_error_info.ppppb_error_msg_pointers[1][ 2] =
		ppb_xa_aacmch_dec_execute_fatal;
#endif
}

#ifdef OPEN_AAC_DEBUG1
const char *ppb_xa_testbench_mem_file_man_fatal[] = {
  "Memory Allocation Error",
  "File Open Failed"
};

xa_error_info_struct xa_testbench_error_info =
{
	/* The Module Name	*/
  "airoha AAC Multi-Channel decode",
  {

    "Memory & File Manager",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    ""
  },
  {
    /* The Message Pointers	*/
    { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }
  }
};
#endif
void xa_testbench_error_handler_init()
{
#ifdef OPEN_AAC_DEBUG1
	xa_testbench_error_info.ppppb_error_msg_pointers[1][ 0] =
		ppb_xa_testbench_mem_file_man_fatal;
#endif
}

XA_ERRORCODE
xa_error_handler(xa_error_info_struct *p_mod_err_info,
		 const char *pb_context,
		 XA_ERRORCODE code) {
  if (code == XA_NO_ERROR) {
    return XA_NO_ERROR;
  }
  {
    int is_fatal     = XA_ERROR_SEVERITY(code) != 0;
    int err_class    = XA_ERROR_CLASS(code);
    /* int err_codec    = XA_ERROR_CODEC(code); */
    int err_sub_code = XA_ERROR_SUBCODE(code);

    //fprintf(stderr, "\n");
    if (!is_fatal)
    {
      //fprintf(stderr, "non ");
    }
   // fprintf(stderr, "fatal error: ");

    if ((p_mod_err_info->pb_module_name != NULL) &&
	(p_mod_err_info->pb_module_name[0] != '\0'))
    {
      //fprintf(stderr, "%s: ", p_mod_err_info->pb_module_name);
    }
    if (p_mod_err_info->ppb_class_names[err_class] != NULL)
    {
     // fprintf(stderr, "%s: ", p_mod_err_info->ppb_class_names[err_class]);
    }
    if (pb_context != NULL)
    {
     // fprintf(stderr, "%s: ", pb_context);
    }

    if ( p_mod_err_info->ppppb_error_msg_pointers[is_fatal][err_class] == NULL)
    {
    	//fprintf(stderr, "Unknown Error Code %d\n", err_sub_code);
    }
    else if  (p_mod_err_info->ppppb_error_msg_pointers[is_fatal][err_class][err_sub_code] == NULL)
    {
    	//fprintf(stderr, "Unknown Error Code %d\n", err_sub_code);
    }
    else
    {
    	//fprintf(stderr, "%s\n", p_mod_err_info->ppppb_error_msg_pointers[is_fatal][err_class][err_sub_code]);
    }
  }
  return XA_NO_ERROR;
}

//---------------------------------------------xa_aac_error_handler ------------------------------------

//---------------- bit stream ---------------------

int get_params(void *p_hdl, int * p_numChannels,  int *p_channelMode,int *p_sampleRate,int *p_sbrType,  int *p_bsfmt,int *p_data_rate, int *p_pcm_sample_size)
{
    int err_code = 0;
    int numChannels, acmod, samp_freq;
    int sbrType;
    char pFmt[20] = {""};
    int  fmt_idx;
    int  aac_sampleRate;

	UNUSED(p_data_rate);

    /* The process API function */
    xa_codec_func_t *p_xa_process_api = xa_aac_dec;

    /* The process API object */
    xa_codec_handle_t p_xa_process_api_obj = (xa_codec_handle_t)p_hdl;

    /* The process error info structure */
    GET_CONFIG( XA_AACDEC_CONFIG_PARAM_AAC_FORMAT, (void *)&fmt_idx, "Get Mpeg format Error");
    /* Get Number Of Channels in the Bit Stream */
    GET_CONFIG( XA_AACDEC_CONFIG_PARAM_NUM_CHANNELS, (void *)&numChannels, "Get num-channels Error");
    GET_CONFIG( XA_AACDEC_CONFIG_PARAM_ACMOD, (void *)&acmod, "Get ChannelMode Error");
    GET_CONFIG( XA_AACDEC_CONFIG_PARAM_SAMP_FREQ, (void *)&samp_freq, "Get Sample-rate Error");
    GET_CONFIG( XA_AACDEC_CONFIG_PARAM_SBR_TYPE, (void *)&sbrType, "Get SbrEnabled type Error");
    GET_CONFIG( XA_AACDEC_CONFIG_PARAM_AAC_SAMPLERATE, (void *)&aac_sampleRate, "Get AAC Sample-Rate Error");

    switch(fmt_idx)
    {
        case XA_AACDEC_EBITSTREAM_TYPE_AAC_ADIF:
            strcpy(pFmt,"ADIF");
            break;
        case XA_AACDEC_EBITSTREAM_TYPE_AAC_ADTS:
            strcpy(pFmt,"ADTS");
            break;
        case XA_AACDEC_EBITSTREAM_TYPE_AAC_LATM:
        case XA_AACDEC_EBITSTREAM_TYPE_AAC_LATM_OUTOFBAND_CONFIG:
            /* Not supported */
            strcpy(pFmt,"LATM");
            break;
        case XA_AACDEC_EBITSTREAM_TYPE_AAC_LOAS:
            /* Supported if library is built with LOAS_SUPPORT=1 */
            strcpy(pFmt,"LOAS");
            break;
        case XA_AACDEC_EBITSTREAM_TYPE_AAC_RAW:
            strcpy(pFmt,"RAW");
            break;
        case XA_AACDEC_EBITSTREAM_TYPE_DABPLUS_RAW_SIDEINFO:
                strcpy(pFmt,"DAB+ RAW+SIDEINFO");
            break;
        case XA_AACDEC_EBITSTREAM_TYPE_DABPLUS:
            strcpy(pFmt,"DAB+");
            break;
        default:
            strcpy(pFmt,"NO");
            err_code = -1;
            break;
    }

    //fprintf(stderr, "Bitstream format:   %s\n",pFmt);
   // fprintf(stderr, "Format:             ");

    /* Determine the format */
    /*
    if (sbrType==2) {
        fprintf(stderr, "aacPlus v2\n");
    }
    else if (sbrType==1) {
        fprintf(stderr, "aacPlus\n");
    }
    else {
        fprintf(stderr, "plain AAC\n");
    }
  */

    *p_numChannels = numChannels;
    *p_channelMode = acmod;
    *p_sampleRate = samp_freq;
    *p_bsfmt = fmt_idx;
    *p_sbrType = sbrType;

    /* Get PCM Sample Size */
    GET_CONFIG( XA_AACDEC_CONFIG_PARAM_PCM_WDSZ, p_pcm_sample_size, "PCM_WDSZ Get Error");

    return err_code;
}
//=========================


void AAC_DECODER_INIT1(inp_buffer_t *inp_buffer,signed char *AAC_STUCT ,signed char *AAC_MEMTABS,signed char *AAC_INPUT_BUF,signed char *AAC_OUTPUT_BUF,signed char *AAC_PERRAM,signed char *AAC_SCRAM)
{
	    int err_code=0;
	    p_xa_process_api = xa_aac_dec;
#ifdef OPEN_AAC_DEBUG
	    p_proc_err_info = (xa_error_info_struct *)&xa_aac_dec_error_info;
#endif
	    p_xa_process_api_obj = NULL;
	    /* Call decoder error handler init */
	    xa_aac_dec_error_handler_init();
	     /* Set API object with the memory allocated */
	     p_xa_process_api_obj = (xa_codec_handle_t)AAC_STUCT ;

	     AAC_API_CALL(XA_API_CMD_INIT, XA_CMD_TYPE_INIT_API_PRE_CONFIG_PARAMS, NULL, "");

	      void *ptemp = AAC_MEMTABS ;
	        AAC_API_CALL(XA_API_CMD_SET_MEMTABS_PTR, 0, ptemp, "");

	        /* initialize the API, post config, fill memory tables */
	        AAC_API_CALL(XA_API_CMD_INIT, XA_CMD_TYPE_INIT_API_POST_CONFIG_PARAMS, NULL, "");
	        /*******************************************************************/
	        /* Allocate Memory with info from library                           */
	        /*******************************************************************/
	        void* pv_alloc_ptr;
	        //i=0 ;
	        pv_alloc_ptr = (signed char*)AAC_PERRAM;
	       	AAC_API_CALL(XA_API_CMD_SET_MEM_PTR, 0, pv_alloc_ptr, "");
	        //i=1
	        pv_alloc_ptr = (signed char*)AAC_SCRAM;
	      	AAC_API_CALL(XA_API_CMD_SET_MEM_PTR, 1, pv_alloc_ptr, "");
	        //i=2
	        pv_alloc_ptr = (signed char*)AAC_INPUT_BUF;
	      	AAC_API_CALL(XA_API_CMD_SET_MEM_PTR, 2, pv_alloc_ptr, "");

	      	InitBuffer(inp_buffer,pv_alloc_ptr, 0);//my 8224

	      	//i=3
	        pv_alloc_ptr = ( signed char*)AAC_OUTPUT_BUF;

	        AAC_API_CALL(XA_API_CMD_SET_MEM_PTR, 3, pv_alloc_ptr, "");
}


#define SET_CONFIG(idx, pvalue, errstr) AAC_API_CALL(XA_API_CMD_SET_CONFIG_PARAM,idx, pvalue, errstr)

int AAC_DECODER_INIT2(inp_buffer_t *inp_buffer,int a,int i_value ,signed int ui_value){
    int ui_init_done = 0;
    int err_code=0;
    int len ,ret;

    UNUSED(ret);
    UNUSED(len);

    SET_CONFIG( XA_AACDEC_CONFIG_PARAM_PCM_WDSZ, (void *)&i_value, "");
    SET_CONFIG(XA_AACDEC_CONFIG_PARAM_EXTERNALBSFORMAT, (void *)&ui_value, "");

    {

          // UpdateBuffer(inp_buffer,  a);///
#define magicoffset  3

    	  inp_buffer->i_buff_size = a+ magicoffset ;
  ///* Set number of bytes to be processed */
           AAC_API_CALL(XA_API_CMD_SET_INPUT_BYTES, 0, &inp_buffer->i_buff_size, "");


       /* Initialize the process */
       AAC_API_CALL(XA_API_CMD_INIT, XA_CMD_TYPE_INIT_PROCESS, NULL, "");

       /* Checking for end of initialization */
       AAC_API_CALL(XA_API_CMD_INIT, XA_CMD_TYPE_INIT_DONE_QUERY, &ui_init_done, "");

       /* How much buffer is used in input buffers */
       AAC_API_CALL(XA_API_CMD_GET_CURIDX_INPUT_BUF, 0, &inp_buffer->i_bytes_consumed, "");

       //printf("",inp_buffer->i_bytes_consumed);

       /* Break if init is done */


   }
     return ui_init_done ;
   }

void InitBuffer(inp_buffer_t *pbuf, void *ptr, int size)
{
    pbuf->pb_inp_buf = (signed char*)ptr;
    pbuf->ui_inp_size = size;
    pbuf->i_bytes_consumed = 0;
    pbuf->i_buff_size = 0;
    pbuf->i_inp_over = 0;
}

unsigned int FILL_AAC_DATA( inp_buffer_t *inp_buffer,int len_read)
{
	int ret_v ,len;
	int err_code=0;
	signed int aac_packet_len ;
	unsigned short framelen=0;

	UNUSED(ret_v);
	UNUSED(len);

#define magicoffset  3 //magic number
    aac_packet_len = (3 + len_read + magicoffset) ; // latm = 3+raw+magicword

	inp_buffer->ui_inp_size = aac_packet_len ;
	  //latm header (3byte)
	  inp_buffer->pb_inp_buf[0]            =0x56 ;
	  inp_buffer->pb_inp_buf[len_read+ 3+ 0]  = inp_buffer->pb_inp_buf[0];
	  framelen  = 0xE000|len_read ;
	  inp_buffer->pb_inp_buf[1] =(char)((framelen>>8)&0x00ff);
	  inp_buffer->pb_inp_buf[len_read+3 + 1]  = inp_buffer->pb_inp_buf[1]  ;
	  inp_buffer->pb_inp_buf[2] =(char)(framelen);
	  inp_buffer->pb_inp_buf[len_read+3+ 2]  = inp_buffer->pb_inp_buf[2]  ;


#ifdef SHOW_MY_DEBUG_MS
        //DSP_MW_LOG_I("%d", 1, aac_packet_len - magicoffset);
        //DSP_MW_LOG_I("%d", 1, inp_buffer->pb_inp_buf[0]&0x00ff);
#endif
       /* Set number of bytes to be processed */
    	     AAC_API_CALL(XA_API_CMD_SET_INPUT_BYTES, 0, &aac_packet_len, "");

     return aac_packet_len ;
}
