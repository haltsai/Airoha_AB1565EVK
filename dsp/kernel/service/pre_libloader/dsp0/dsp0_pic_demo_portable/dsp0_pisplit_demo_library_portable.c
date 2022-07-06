#include <string.h>
#include <xtensa/tie/xt_hifi2.h>
#include <xtensa/hal.h>
#include "assert.h"


#include "xt_library_loader.h"
#include "preloader_pisplit.h"
#include "dsp0_pisplit_demo_library_portable.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "syslog.h"
LOG_CONTROL_BLOCK_DECLARE(common);
#ifdef DSP0_PISPLIT_DEMO_LIBRARY

/***********************************************************************/
SemaphoreHandle_t dsp0_pisplit_demo_library_xSemaphore = NULL;


/***********************************************************************/

extern void dsp0_pisplit_demo_library_default_function_parameter();


void *dsp0_pisplit_demo_library_export_parameters[3] = {dsp0_pisplit_demo_library_default_function_parameter,
                                                        dsp0_pisplit_demo_library_default_function_parameter,
                                                        dsp0_pisplit_demo_library_default_function_parameter};
void *dsp0_pisplit_demo_library_import_parameters[1] = {printf};//


void dsp0_pisplit_demo_library_default_function_parameter()
{
    LOG_MSGID_I(common,"function point is NULL!!! dsp0_pisplit_demo_library not load or had been unload!!!",0);
    assert(0);
}


preloader_pisplit_handle_t p_dsp_pisplit_library_handle = NULL;

void dsp0_pisplit_demo_library_callback(preloader_pisplit_handle_t handle)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t *p_export_parameters;
    p_export_parameters = preloader_pisplit_get_export_parameter(handle,dsp0_pisplit_demo_library_import_parameters);

    LOG_MSGID_I(common,"*p_export_parameters:0x%x",1,*p_export_parameters);

    if(p_export_parameters == NULL)
    {
       LOG_MSGID_I(common,"p_export_parameters is NULL, please check!!!",0);
    }
    else
    {
    /*notice:Strongly suggestion: Just do receive parameters on CCNI ISR callback.
                                  Then notice your thread that can execute the library entries!!!*/
        dsp0_pisplit_demo_library_export_parameters[0] =(void*)p_export_parameters[0];
        dsp0_pisplit_demo_library_export_parameters[1] =(void*)p_export_parameters[1];
        dsp0_pisplit_demo_library_export_parameters[2] =(void*)p_export_parameters[2];
        LOG_MSGID_I(common,"dsp0_pisplit_demo_library_export_parameters[0]:0x%x",1,(uint32_t)dsp0_pisplit_demo_library_export_parameters[0]);
        LOG_MSGID_I(common,"dsp0_pisplit_demo_library_export_parameters[1]:0x%x",1,(uint32_t)dsp0_pisplit_demo_library_export_parameters[1]);
        LOG_MSGID_I(common,"dsp0_pisplit_demo_library_export_parameters[2]:0x%x",1,(uint32_t)dsp0_pisplit_demo_library_export_parameters[2]);
        xSemaphoreGiveFromISR( dsp0_pisplit_demo_library_xSemaphore, &xHigherPriorityTaskWoken );
        /*******Semaphore give won't resume task so need portYIELD*******/
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void dsp0_pisplit_demo_library_load()
{
    if (dsp0_pisplit_demo_library_xSemaphore == NULL)
    {
        dsp0_pisplit_demo_library_xSemaphore = xSemaphoreCreateBinary();

        if( dsp0_pisplit_demo_library_xSemaphore == NULL )
        {
            assert(0);
        }
    }

    if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_get_handle(&p_dsp_pisplit_library_handle,&dsp0_pisplit_library_build_demo,dsp0_pisplit_demo_library_callback) )
    {
        LOG_MSGID_I(common,"dsp0_pisplit_library_build_demo preloader_pisplit_get_handle() error!!!!",0);
        assert(0);
    }
    if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_load(p_dsp_pisplit_library_handle,PRELOADER_I_HIGH_PERFORMANCE,PRELOADER_D_HIGH_PERFORMANCE) )
    {
        LOG_MSGID_I(common,"dsp0_pisplit_library_build_demo preloader_pisplit_load() error!!!!",0);
        /*******Add PIC heap dump to check current memory usage before load fail*******/
        preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_I_HIGH_PERFORMANCE);
        preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_I_LOW_PERFORMANCE);
        preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_D_HIGH_PERFORMANCE);
        preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_D_LOW_PERFORMANCE);
        /*******Add PIC heap dump to check current memory usage before load fail*******/
        assert(0);
    }

    /*notice: If you want to blocking this task when call load library, you can use this method to wait semaphore,until loading done.
              If you don't want to blocking this thread, you can miss waiting semaphonre and return directly. 
                  You should confirm the library had been loading done before call the library entries by yourselves.*/
    if(xSemaphoreTake( dsp0_pisplit_demo_library_xSemaphore, portMAX_DELAY) == pdTRUE)
    {
    }
    else
    {
        assert(0);
    }
    /*******Semaphore delete to free heap usage*******/
    vSemaphoreDelete(dsp0_pisplit_demo_library_xSemaphore);
    dsp0_pisplit_demo_library_xSemaphore = NULL;
    /*******Semaphore delete to free heap usage*******/
}

void dsp0_pisplit_demo_library_unload()
{
    if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_unload(p_dsp_pisplit_library_handle))
    {
        LOG_MSGID_I(common,"dsp0_pisplit_library_build_demo preloader_pisplit_unload() error!!!!",0);
        assert(0);
    }
    if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_release_handle(p_dsp_pisplit_library_handle))
    {
        LOG_MSGID_I(common,"dsp0_pisplit_library_build_demo preloader_pisplit_release_handle() error!!!!",0);
        assert(0);
    }

    dsp0_pisplit_demo_library_export_parameters[0] = dsp0_pisplit_demo_library_default_function_parameter;
    dsp0_pisplit_demo_library_export_parameters[1] = dsp0_pisplit_demo_library_default_function_parameter;
    dsp0_pisplit_demo_library_export_parameters[2] = dsp0_pisplit_demo_library_default_function_parameter;
    p_dsp_pisplit_library_handle = NULL;
}

#endif

