#include <string.h>
#include <xtensa/tie/xt_hifi2.h>
#include <xtensa/hal.h>
#include "assert.h"


#include "xt_library_loader.h"
#include "preloader_pisplit.h"
#include "dsp1_pisplit_demo_library_portable.h"
#include "syslog.h"
#include "hal.h"
#include "hal_ccni_config.h"
#include "hal_resource_assignment.h"
#include "ccci.h"
#include "airo_cqueue.h"
#include "message_queue_configure.h"
LOG_CONTROL_BLOCK_DECLARE(common);
#ifdef DSP1_PISPLIT_DEMO_LIBRARY
extern void *p_message_queue;

extern void dsp1_pisplit_demo_library_default_function_parameter();


void *dsp1_pisplit_demo_library_export_parameters[3] = {dsp1_pisplit_demo_library_default_function_parameter,
                                                        dsp1_pisplit_demo_library_default_function_parameter,
                                                        dsp1_pisplit_demo_library_default_function_parameter};
void *dsp1_pisplit_demo_library_import_parameters[1] = {printf};//


void dsp1_pisplit_demo_library_default_function_parameter()
{
    LOG_MSGID_I(common,"function point is NULL!!! dsp1_pisplit_demo_library not load or had been unload!!!",0);
    assert(0);
}


preloader_pisplit_handle_t p_dsp1_pisplit_demo_library_handle = NULL;

//This is a callback function of CCCI, because DSP0 help DSP1 to load lib, when loading done
//Will send CCCI to DSP1,so this function register by CCCI callback
void dsp1_pisplit_lib_demo_load_done(ccci_msg_t ccci_msg)
{
    uint32_t *p_export_parameters;
    uint32_t data = ccci_msg.data;
    if(ccci_msg.event != CCCI_EVENT_DSP0_TO_DSP1_PIC_DEMO_LIBRARY_LOAD_DONE)
        assert(0);
    p_dsp1_pisplit_demo_library_handle = (preloader_pisplit_handle_t)hal_memview_infrasys_to_dsp1(data);

    LOG_MSGID_I(common,"p_dsp1_pisplit_demo_library_handle:0x%x",1,(uint32_t)p_dsp1_pisplit_demo_library_handle);

    p_export_parameters = preloader_pisplit_get_export_parameter(p_dsp1_pisplit_demo_library_handle,dsp1_pisplit_demo_library_import_parameters);
    if(p_export_parameters == NULL)
    {
       LOG_MSGID_I(common,"p_export_parameters is NULL, please check!!!",0);
       assert(0);
    }
    else
    {
    /*Notice:Strongly suggestion: Just do receive parameters on CCNI ISR callback.
                                  Then notice your thread that can execute the library entries!!!*/
        dsp1_pisplit_demo_library_export_parameters[0] =(void*)p_export_parameters[0];
        dsp1_pisplit_demo_library_export_parameters[1] =(void*)p_export_parameters[1];
        dsp1_pisplit_demo_library_export_parameters[2] =(void*)p_export_parameters[2];
        LOG_MSGID_I(common,"dsp1_pisplit_demo_library_export_parameters[0]:0x%x",1,(uint32_t)dsp1_pisplit_demo_library_export_parameters[0]);
        LOG_MSGID_I(common,"dsp1_pisplit_demo_library_export_parameters[1]:0x%x",1,(uint32_t)dsp1_pisplit_demo_library_export_parameters[1]);
        LOG_MSGID_I(common,"dsp1_pisplit_demo_library_export_parameters[2]:0x%x",1,(uint32_t)dsp1_pisplit_demo_library_export_parameters[2 ]);
    }

    /* Notice: Now the library had been load done, user can use the library porting layer symbols of dsp1_demo_lib_entry_1() 
                        and dsp1_demo_lib_entry_2() and dsp1_demo_lib_entry_3()
            There are three choices:
                1. [ Not suggestion]Call the lib enrties to here, becase this is CCCI callback thread(IRQ gen),
                    should not execute too much code to here except user just need to run one thread at this time.
                    Sample code:
                    {
                        dsp1_demo_lib_entry_1();     //To call the library entry of dsp1_demo_lib_entry_1()
                        dsp1_demo_lib_entry_2();    //To call the library entry of dsp1_demo_lib_entry_2()
                        dsp1_demo_lib_entry_3();    //To call the library entry of dsp1_demo_lib_entry_3()
                     }
                2. [Just for the user which need multi thread with nested] Tirgger a SW IRQ thread, and call library enrties on SW IRQ.
                    Need to pay attention:
                        Priority(SW0_IRQn) > Priority(CCCI callback) == Priority(SW1_IRQn) > Priority(SW2_IRQn) > Priority(SW3_IRQn)
                        User should better just trigger SW2_IRQn or SW3_IRQn, 
                        otherwise will pending CCCI callback lead to impact DSP1 multi thread notification.
                            Sample code:
                            {
                                hal_nvic_register_isr_handler(SW3_IRQn, sw3_isr);
                                hal_nvic_enable_irq(SW3_IRQn);
                                hal_nvic_irq_software_trigger(SW3_IRQn);
                             }
                             void sw3_isr(void)
                             {
                                dsp1_demo_lib_entry_1();     //To call the library entry of dsp1_demo_lib_entry_1()
                                dsp1_demo_lib_entry_2();    //To call the library entry of dsp1_demo_lib_entry_2()
                                dsp1_demo_lib_entry_3();    //To call the library entry of dsp1_demo_lib_entry_3()
                             }
                3. [Suggestion] send message queue on here!!! And on main while loop to execute the library entries
                    Sample code:
                    {
                         message_queue_t dsp1_lib_demo_message;
                        dsp1_lib_demo_message.event_id = MESSAGE_DSP1_PISPLIT_LIBRARY_DEMO_LOADING_DONE;
                        airo_cqueue_send(p_message_queue, (void *)&dsp1_lib_demo_message);
                    }
      */
    {
        //this is just a sample for message queue
        bool ret = false;
        message_queue_t dsp1_lib_demo_message;
        dsp1_lib_demo_message.event_id = MESSAGE_DSP1_PISPLIT_LIBRARY_DEMO_LOADING_DONE;
        ret = airo_cqueue_send(p_message_queue, (void *)&dsp1_lib_demo_message);
        assert(ret);
    }
}

//This is a callback function of CCCI, because DSP0 help DSP1 to load lib, when unloading done
//Will send CCCI to DSP1,so this function register by CCCI callback
void dsp1_pisplit_lib_demo_unload_done(ccci_msg_t ccci_msg)
{
    if(ccci_msg.event != CCCI_EVENT_DSP0_TO_DSP1_PIC_DEMO_LIBRARY_UNLOAD_DONE)
        assert(0);

    dsp1_pisplit_demo_library_export_parameters[0] =dsp1_pisplit_demo_library_default_function_parameter;
    dsp1_pisplit_demo_library_export_parameters[1] =dsp1_pisplit_demo_library_default_function_parameter;
    dsp1_pisplit_demo_library_export_parameters[2] =dsp1_pisplit_demo_library_default_function_parameter;
}
#endif

