#include <string.h>
#include <xtensa/tie/xt_hifi2.h>
#include <xtensa/hal.h>
#include "assert.h"


#include "xt_library_loader.h"
#include "preloader_pisplit.h"
#include "dsp0_load_dsp1_pisplit_demo_library_portable.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "ccci.h"
#include "hal_resource_assignment.h"
#include "hal_clock_internal.h"
#include "syslog.h"
LOG_CONTROL_BLOCK_DECLARE(common);
#ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
#ifdef DSP1_PISPLIT_DEMO_LIBRARY

preloader_pisplit_handle_t p_dsp0_load_dsp1_pisplit_library_handle;
void dsp0_load_dsp1_pisplit_demo_library_callback(preloader_pisplit_handle_t handle)
{
    ccci_msg_t ccci_msg;
    LOG_MSGID_I(common,"this is dsp0_load_dsp1_pisplit_demo_library_callback",0);
    //just for error check and debug
    //if(handle != p_dsp0_load_dsp1_pisplit_library_handle)
    //{
    //     LOG_MSGID_I(common,"main error !!! p_dsp0_load_dsp1_pisplit_library_handle=0x%x handle=0x%x ---",0,(uint32_t)p_dsp0_load_dsp1_pisplit_library_handle,(uint32_t)handle);
    //}
    //LOG_MSGID_I(common,"p_dsp0_load_dsp1_pisplit_library_handle:0x%x",0,(uint32_t)p_dsp0_load_dsp1_pisplit_library_handle);
    ccci_msg.event = CCCI_EVENT_DSP0_TO_DSP1_PIC_DEMO_LIBRARY_LOAD_DONE;
    ccci_msg.data = hal_memview_dsp0_to_infrasys((uint32_t)handle);
    if(CCCI_STATUS_OK != ccci_send_msg(HAL_CORE_DSP1,ccci_msg,CCCI_SEND_MSG_NO_WAIT))
    {
        assert(0);
    }
}

void dsp0_load_dsp1_pisplit_demo_library_load()
{
    if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_get_handle(&p_dsp0_load_dsp1_pisplit_library_handle,&dsp1_pisplit_library_build_demo,dsp0_load_dsp1_pisplit_demo_library_callback) )
    {
        LOG_MSGID_I(common,"dsp0_load_dsp1_pisplit_demo_library_load preloader_pisplit_get_handle() error!!!!",0);
        assert(0);
    }
    if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_load(p_dsp0_load_dsp1_pisplit_library_handle,PRELOADER_EXT_IRAM,PRELOADER_EXT_DRAM) )
    {
        LOG_MSGID_I(common,"dsp0_load_dsp1_pisplit_demo_library_load preloader_pisplit_load() error!!!!",0);
        assert(0);
    }

}

void dsp0_load_dsp1_pisplit_demo_library_unload()
{
    ccci_msg_t ccci_msg;
    if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_unload(p_dsp0_load_dsp1_pisplit_library_handle))
    {
        LOG_MSGID_I(common,"dsp0_load_dsp1_pisplit_demo_library_unload preloader_pisplit_unload() error!!!!",0);
        assert(0);
    }
    if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_release_handle(p_dsp0_load_dsp1_pisplit_library_handle))
    {
        LOG_MSGID_I(common,"dsp0_load_dsp1_pisplit_demo_library_unload preloader_pisplit_release_handle() error!!!!",0);
        assert(0);
    }

    ccci_msg.event = CCCI_EVENT_DSP0_TO_DSP1_PIC_DEMO_LIBRARY_UNLOAD_DONE;
    ccci_msg.data = 0;
    if(CCCI_STATUS_OK != ccci_send_msg(HAL_CORE_DSP1,ccci_msg,CCCI_SEND_MSG_NO_WAIT))
    {
        assert(0);
    }
}

// dsp1 notify dsp0 dsp1 pic lib had been executed done, now to do unload
void dsp1_pic_demo_lib_execute_done()
{
    dsp0_load_dsp1_pisplit_demo_library_unload();
}
#endif
#endif
