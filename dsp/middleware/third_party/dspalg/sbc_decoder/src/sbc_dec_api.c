//#include <string.h>
//#include <assert.h>
#include "sbc_dec_api.h"
#ifdef MTK_BT_A2DP_SBC_USE_PIC
#include "sbc_decoder_portable.h"
#endif
#include "memory_attribute.h"

VOID xa_sbc_dec_error_handler_init();
VOID xa_testbench_error_handler_init();

extern xa_error_info_struct xa_testbench_error_info;
extern xa_error_info_struct xa_sbc_dec_error_info;

XA_ERRORCODE xa_sbc_dec_get_config_param (
                 xa_codec_handle_t p_xa_process_api_obj,
			     pWORD32           pi_bitrate,
			     pWORD32           pi_samp_freq,
			     pWORD32           pi_num_chan,
			     pWORD32           pi_pcm_wd_sz )
{
    XA_ERRORCODE err_code = XA_NO_ERROR;
    // the process API function
    //xa_codec_func_t *p_xa_process_api = xa_sbc_dec;

    xa_error_info_struct *p_proc_err_info = &xa_sbc_dec_error_info;
    UNUSED(p_proc_err_info);

    // Data rate
    {
        err_code = xa_sbc_dec(p_xa_process_api_obj,
				   XA_API_CMD_GET_CONFIG_PARAM,
				   XA_SBC_DEC_CONFIG_PARAM_BITRATE, pi_bitrate);
        _XA_HANDLE_ERROR(p_proc_err_info, "", err_code);

    }
    // Sampling frequency
    {
        err_code = xa_sbc_dec(p_xa_process_api_obj,
				   XA_API_CMD_GET_CONFIG_PARAM,
				   XA_SBC_DEC_CONFIG_PARAM_SAMP_FREQ, pi_samp_freq);
        _XA_HANDLE_ERROR(p_proc_err_info, "", err_code);
    }
    // Number of channels
    {
        err_code = xa_sbc_dec(p_xa_process_api_obj,
				   XA_API_CMD_GET_CONFIG_PARAM,
				   XA_SBC_DEC_CONFIG_PARAM_NUM_CHANNELS, pi_num_chan);
        _XA_HANDLE_ERROR(p_proc_err_info, "", err_code);
    }
    // PCM word size
    {
        err_code = xa_sbc_dec(p_xa_process_api_obj,
				   XA_API_CMD_GET_CONFIG_PARAM,
				   XA_SBC_DEC_CONFIG_PARAM_PCM_WDSZ, pi_pcm_wd_sz);
        _XA_HANDLE_ERROR(p_proc_err_info, "", err_code);
    }

    return XA_NO_ERROR;
}

void Flush_SBC_In_Buf(SBC_DEC_MEM *param)
{
	SBC_DEC_INP_BUF *pbuf = &param->inp_buf;

	pbuf->ui_inp_size = sizeof(param->Input_Buf);
	pbuf->i_bytes_consumed = pbuf->ui_inp_size;
	pbuf->i_buff_size = pbuf->ui_inp_size;
	pbuf->i_bytes_read = 0;
	pbuf->pb_inp_buf = param->Input_Buf;
	pbuf->pb_out_buf = param->Output_Buf;
};

// ==================================================================================================
// ==================================================================================================
XA_ERRORCODE SBC_Init1(SBC_DEC_MEM *param)
{
	XA_ERRORCODE      	 err_code = XA_NO_ERROR;
    xa_error_info_struct *p_proc_err_info;

    // The error init function
    VOID (*p_error_init)();

    UNUSED(p_proc_err_info);
    UNUSED(p_error_init);

    // Stack process struct initing
    //p_error_init = xa_sbc_dec_error_handler_init;
    //p_proc_err_info = &xa_sbc_dec_error_info;
    // Stack process struct initing end

    //p_error_init();

    // Allocate memory for API
    //g_pv_arr_alloc_memory[g_w_malloc_count] = param->API_Buf; // 24
    // API object requires 4 bytes (WORD32) alignment;
    // malloc() provides at least 8-byte alignment.
    //assert((((unsigned int) param->API_Buf) & 3) == 0);

    // Set API object with the memory allocated
    xa_codec_handle_t xa_process_handle = (void *) param->API_Buf;
    // Set the config params to default values
    err_code = xa_sbc_dec(xa_process_handle, XA_API_CMD_INIT, XA_CMD_TYPE_INIT_API_PRE_CONFIG_PARAMS, NULL);
    _XA_HANDLE_ERROR(p_proc_err_info, "", err_code);

    // Initialize Memory info tables
    // Get memory info tables size
    //g_pv_arr_alloc_memory[g_w_malloc_count] = param->MEMTABS_Buf; // 80
    // Memory table requires 4 bytes (WORD32) alignment; malloc()
    // provides at least 8-byte alignment.
    //assert((((unsigned int) param->MEMTABS_Buf) & 3) == 0);
    // Set pointer for process memory tables
    err_code = xa_sbc_dec(xa_process_handle, XA_API_CMD_SET_MEMTABS_PTR, 0, (void *) param->MEMTABS_Buf);
    _XA_HANDLE_ERROR(p_proc_err_info, "", err_code);

    // initialize the API, post config, fill memory tables
    err_code = xa_sbc_dec(xa_process_handle, XA_API_CMD_INIT, XA_CMD_TYPE_INIT_API_POST_CONFIG_PARAMS, NULL);
    _XA_HANDLE_ERROR(p_proc_err_info, "", err_code);

    // ******************************************************************
    // Allocate Memory with info from library
    // ******************************************************************
    // Set the buffer pointer
     err_code = xa_sbc_dec(xa_process_handle, XA_API_CMD_SET_MEM_PTR, 0, param->Persistent_Buf);
     _XA_HANDLE_ERROR(p_proc_err_info, "", err_code);

     err_code = xa_sbc_dec(xa_process_handle, XA_API_CMD_SET_MEM_PTR, 1, param->Scratch_Buf);
     _XA_HANDLE_ERROR(p_proc_err_info, "", err_code);

     err_code = xa_sbc_dec(xa_process_handle, XA_API_CMD_SET_MEM_PTR, 2, param->Input_Buf);
     _XA_HANDLE_ERROR(p_proc_err_info, "", err_code);

     err_code = xa_sbc_dec(xa_process_handle, XA_API_CMD_SET_MEM_PTR, 3, param->Output_Buf);
     _XA_HANDLE_ERROR(p_proc_err_info, "", err_code);

     Flush_SBC_In_Buf(param);

     return XA_NO_ERROR;
};

XA_ERRORCODE SBC_Init2(SBC_DEC_MEM *param, UWORD32 *ui_init_done)
{
    xa_error_info_struct  	*p_proc_err_info;
    XA_ERRORCODE 			err_code = XA_NO_ERROR;
    xa_codec_handle_t 		xa_process_handle = (void *) param->API_Buf;
    SBC_DEC_INP_BUF 		*pbuf = &param->inp_buf;

    UNUSED(p_proc_err_info);

    pbuf->i_buff_size = pbuf->i_buff_size - (pbuf->i_bytes_consumed - pbuf->i_bytes_read);
    if( pbuf->i_buff_size <= 0 ) {
    	pbuf->i_buff_size = 0;
        // Tell that the input is over in this buffer
        err_code = xa_sbc_dec(xa_process_handle, XA_API_CMD_INPUT_OVER, 0, NULL);
        _XA_HANDLE_ERROR(p_proc_err_info, "", err_code);
    }

    // Set number of bytes to be processed
    err_code = xa_sbc_dec(xa_process_handle, XA_API_CMD_SET_INPUT_BYTES, 0, &pbuf->i_buff_size);

    _XA_HANDLE_ERROR(p_proc_err_info, "", err_code);

    // Initialize or Execute the process
    err_code = xa_sbc_dec(xa_process_handle, XA_API_CMD_INIT, XA_CMD_TYPE_INIT_PROCESS, NULL);

    _XA_HANDLE_ERROR(p_proc_err_info, "", err_code);
    // Checking for end of initialization
    err_code = xa_sbc_dec(xa_process_handle, XA_API_CMD_INIT, XA_CMD_TYPE_INIT_DONE_QUERY, ui_init_done);

    _XA_HANDLE_ERROR(p_proc_err_info, "", err_code);

    // How much buffer is used in input buffers
    err_code =xa_sbc_dec(xa_process_handle, XA_API_CMD_GET_CURIDX_INPUT_BUF, 0, &pbuf->i_bytes_consumed);

    _XA_HANDLE_ERROR(p_proc_err_info, "", err_code);

    if (*ui_init_done) {
    	 err_code = xa_sbc_dec_get_config_param(xa_process_handle,
    					   	   	   	   	   	    &param->info.i_bitrate,
   					   	   	   	   	   	   	    &param->info.i_samp_freq,
  					   	   	   	   	   	   	   	&param->info.i_num_chan,
  					   	   	   	   	   	   	   	&param->info.i_pcm_wd_sz);
    	 _XA_HANDLE_ERROR(p_proc_err_info, "", err_code);
    }

    return XA_NO_ERROR;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 XA_ERRORCODE SBC_Decoder_Process(SBC_DEC_MEM *param, WORD32 *o_frame_size, UWORD32 *ui_exec_done)
{
    /* Error code */
	WORD32      			i_out_bytes;
	xa_error_info_struct  	*p_proc_err_info;
    XA_ERRORCODE 			err_code      = XA_NO_ERROR;
    XA_ERRORCODE 			err_code_exec = XA_NO_ERROR;
    xa_codec_handle_t 		xa_process_handle = (void *) param->API_Buf;
    SBC_DEC_INP_BUF 		*pbuf = &param->inp_buf;

    UNUSED(p_proc_err_info);

    pbuf->i_buff_size = pbuf->i_buff_size - (pbuf->i_bytes_consumed - pbuf->i_bytes_read);
    if( pbuf->i_buff_size <= 0 ) {
      	pbuf->i_buff_size = 0;
        // Tell that the input is over in this buffer
        err_code = xa_sbc_dec(xa_process_handle, XA_API_CMD_INPUT_OVER, 0, NULL);
        _XA_HANDLE_ERROR(p_proc_err_info, "", err_code);
    }

    // Set number of bytes to be processed
    err_code = xa_sbc_dec(xa_process_handle, XA_API_CMD_SET_INPUT_BYTES, 0, &pbuf->i_buff_size);

    _XA_HANDLE_ERROR(p_proc_err_info, "", err_code);

    // Execute process
    err_code_exec = xa_sbc_dec(xa_process_handle, XA_API_CMD_EXECUTE, XA_CMD_TYPE_DO_EXECUTE, NULL);

    _XA_HANDLE_ERROR(p_proc_err_info, "", err_code_exec);

    // Checking for end of processing
    err_code = xa_sbc_dec(xa_process_handle, XA_API_CMD_EXECUTE, XA_CMD_TYPE_DONE_QUERY, ui_exec_done);

    _XA_HANDLE_ERROR(p_proc_err_info, "", err_code);

    // Get the output bytes
    err_code = xa_sbc_dec(xa_process_handle, XA_API_CMD_GET_OUTPUT_BYTES, 0, &i_out_bytes);

    err_code = xa_sbc_dec_get_config_param(xa_process_handle,
    							 	 	   &param->info.i_bitrate,
    							 	 	   &param->info.i_samp_freq,
    							 	 	   &param->info.i_num_chan,
    							 	 	   &param->info.i_pcm_wd_sz);

    _XA_HANDLE_ERROR(p_proc_err_info, "", err_code);

    //*ui_nsamples = i_out_bytes / output_wordsize(param->info.i_pcm_wd_sz);
    *o_frame_size = i_out_bytes / (2 * ((param->info.i_pcm_wd_sz + 15) / 16));

    // How much buffer is used in input buffers
    err_code = xa_sbc_dec(xa_process_handle, XA_API_CMD_GET_CURIDX_INPUT_BUF, 0, &pbuf->i_bytes_consumed);

    _XA_HANDLE_ERROR(p_proc_err_info, "", err_code);

    return XA_NO_ERROR;

} // End xa_sbc_dec_process()
