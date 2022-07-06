/*
 * Copyright (c) 2012-2013 by Tensilica Inc. ALL RIGHTS RESERVED.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __PRELOADER_PISPLIT__
#define __PRELOADER_PISPLIT__

#include"preloader_pisplit_configure.h"
#include "xt_library_loader.h"
#include <stdint.h>
/**
 * @addtogroup kernel_service
 * @{
 * @addtogroup preloader_pisplit
 * @{
 * This section describes the programming interfaces of the preloader_pisplit driver.
 *
 * @section Perloader_Pisplit_Terms_Chapter Terms and acronyms
 *
 * The following provides descriptions to the terms commonly used in the preloader driver and how to use its various functions.
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b PI                         | Position-Independent.|
 * |\b PIC                        | Position-Independent-Code.|
 * |\b split                      | means one PIC library code and data region can be loaded to two difference regions.|
 * |\b pisplit                    | the pisplit library must have memory blocks - one writable and executable for code part and one writable for data part.|
 * |\b preloader                  | Use DMA to do pre-load the library.|
 * |\b I/D \b RAM                 | DSP local instruction RAM and data RAM.|
 * |\b main \b program            | A dsp main project, like the project of dsp0_freertos_create_thread, one PIC library must be link with main program to constitute a full DSP project.|
 * |\b PIC \b memory \b pool      | A specified memory pool for PIC library.|
 * |\b extra \b memory            | Just a speical memory type, DSP1 free I/DRAM are extra memory pools for DSP0.|
 *
 * @section Perloader_Pisplit_Features_Chapter Supported features
 * Preloader diagram as below:
 * @image html preloader_Standard_PIC_solution_introduction.png
 * Features supported by this module are listed below:
 * - \b Position-Independent \b Libraries.\n
 *   The Position-Independent Libraries feature is Cadence standard service,please reference Cadence document of "Xtensa System Software Reference Manual.pdf", as the chapter of "4.3 Position-Independent Libraries" description:\n
 *   "A position-independent library can be loaded and run at any address that supports both code and data, like normal system RAM. Alternatively,you can use the pisplitload LSP to load code and data into separate memory blocks in the local RAM.
. It is not necessary to decide on the location of the library until run time. Because their locations are not fixed, a variety of position-independent libraries can be loaded at the same time, without deciding at build time which sets are possible. For example, if you have five codecs, and three can be loaded at the same time, then they could be loaded in ten different combinations, which is too many to feasibly link in fixed-locations. Position-independent libraries solve that problem by allowing the codecs to be loaded in arbitrary locations and in an arbitrary order."\n
 *     \b Advantages:
 *              Flexibility: Several dynamic location libraries can be loaded in arbitrary combinations at arbitrary locations,
 *                  all of which is decided at run-time.\n
 *     \b Disadvantages:
 *              Performance: A position-independent library must be compiled as position-independent code.
 *                  This is marginally slower than position-dependent code.\n
 * - \b Preloader \b Position-Independent \b Libraries .\n
 *   We provide a mechanism to load pisplit libraries by DMA. Because loading a library image from Flash to DSP local memory need takes time,
 *      you can load the libraries in advance. It invokes a callback function to notify that that load is complete.\n
 *   To use this mechanism, you can save MCU MIPS and speed up the action of loading libraries.
 * @section Perloader_Pisplit_Memory_Chapter The Required Resources of the Preloader
 * - \b Pisplit \b libraries \b build \b environment.\n
 *         The pisplit libraries build another set of things that are different to the set built by the main program.
 *              These types of libraries must be built with the compile option of "-fpic".
 *              The reference makefile of "dsp\project\ab1558_evk\templates\dsp_pisplit_library_build_demo\dsp0\Makefile";
 *              Please first clone this file when you create your own library.\n
 * - \b Main \b program \b link \b the \b pisplit \b libraries.\n
 *         The main program must be linked with the pisplit library during the link stage.
 *              The pisplit library is different to the general library because its library's symbol is invisible to the main program,
 *              it is simply a large array to the main program.\n
 * - \b The \b memory \b of \b dynamic \b allocate \b by \b main \b program.\n
 *         DSP must have a dynamically allocated mechanism because runtime is necessary for loading the piplit library to run.
 *              We use DSP local memory of I/D RAM as the relocation memory in our DSP SDK reference design. The reference projects will use all the free memory of I/D RAM.\n
 * - \b Pre-load \b image.\n
 *         Cadence provides the basic service for loading and relocating the pisplit libraries.
 *              The DMA service is necessary because the preloader modifies some of the Cadence service to implement a mechanism of using DMA to do libraries image copy.\n
 * - \b The \b parameters \b transfer \b between \b main \b program \b and \b pisplit \b libraries.\n
 *         The export parameters transfer to main program by pisplit libraries. For example, the pisplit library entry functions must be transferred to the main program,
 *              to make sure the library can be correctly called by the main program after it has loaded.\n
 *         The import parameters transfer to pisplit libraries by the main program. For example, the pisplit library maybe need to call some basic functions or data defined by main program.
 *              You can then transfer it as import parameters. Just use Type Cast to use these symbols in the library.\n
 * @section Perloader_Pisplit_Library_Build_Chapter How to build pisplit libraries
 * - \b Step 1: Create your library files.\n
 *         Create your pisplit library file like "dsp\project\ab1558_evk\templates\dsp_pisplit_library_build_demo\dsp0\src\lib_main.c".\n
 * @code
 * //this is pisplit library code
 *#include <stdio.h>
 * typedef int (*printf_ptr_t)(const char *format, ...);
 *
 * printf_ptr_t printf_ptr;
 *
 * #define printf printf_ptr
 *
 * char * dsp0_demo_lib_entry_1(void)
 * {
 *  printf("executing function dsp0_demo_lib_entry_1\n");
 *  return "this string returned from dsp0_demo_lib_entry_1";
 * }
 * char * dsp0_demo_lib_entry_2(void)
 * {
 *  printf("executing function dsp0_demo_lib_entry_2\n");
 *  return "this string returned from dsp0_demo_lib_entry_2";
 * }
 * char * dsp0_demo_lib_entry_3(void)
 * {
 *  printf("executing function dsp0_demo_lib_entry_3\n");
 *  return "this string returned from dsp0_demo_lib_entry_3";
 * }
 *
 * void * export_parameter_array[3] = {dsp0_demo_lib_entry_1,dsp0_demo_lib_entry_2,dsp0_demo_lib_entry_3};
 *
 * void * _start (void *p_import_parameters_array[])
 * {
 *  printf_ptr = p_import_parameters_array[0];
 *  printf("dsp0_demo_lib_entry_1:0x%x   dsp0_demo_lib_entry_2:0x%x   dsp0_demo_lib_entry_3:0x%x",dsp0_demo_lib_entry_1,dsp0_demo_lib_entry_2,dsp0_demo_lib_entry_3);
 *  return export_parameter_array;
 * }
 * @endcode
 *         Please pay attention to the following three points:\n
 *         1. Pisplit library is a stand-alone build system. Please do not reference any symbols defined by other libraries, including the C library.
 *         2. The export parameters to the main program must be prepared , the same as the sample code symbol of "dsp0_demo_lib_entry_1" and "dsp0_demo_lib_entry_2" and "dsp0_demo_lib_entry_3",
 *          the function of "_start" will return the address of "export_parameter_array" to main program. The main program then knows the entry of the library. You can even let the the function of "_start" return a array address which stores more functions or data point, 
then the main program can get more entries of this library.\n
 *         3. The import parameters to the library must be prepared. The sample code symbol of "printf_func", 
 *          the function of "_start" can import a parameter when called by the main program. You can import a parameter of the function of "_start" with an array address which stores more functions or data points, then the library can get more import parameters defined by the main program.\n
 * - \b Setp 2: Prepare a pisplit library build system.\n
 *         You must first clone the makefile of "dsp\project\ab1558_evk\templates\dsp_pisplit_library_build_demo\dsp0\Makefile", then you should make changes for your library:\n
 *         1. Modify the path of depth, such as the "ROOTDIR" as shown in the following sample code:
 * @code
 *  # module makefile settings
 *  ROOTDIR       :=  ../../../..
 * @endcode
 *         2. Modify your PIC library settings as shown in the following sample code::\n
 *            A. PIC_LIB is the final output file path and name. Please pay specific attention to it. You must also have the xxx.o file when the main program links.\n
 *            B. PIC is your library name. Please pay specific attention to it. You must use this name to load your library on the main program.\n
 *            C. PIC_MODULE_PATH use for identify your library files to build.
 * @code
 *  # PIC settings
 *  PIC      :=  dsp0_pisplit_library_build_demo
 *  # PIC_LIB named better is {PIC}.o
 *  PIC_LIB  :=  $(BUILD_LIBDIR)/dsp0_pisplit_library_build_demo.o
 *  # PIC_DEBUG_LIB named better is {PIC}_position_independent.splitload.lib
 *  PIC_DEBUG_LIB  :=  $(BUILD_LIBDIR)/dsp0_pisplit_library_build_demo_position_independent.splitload.lib
 *  PIC_MODULE_PATH := project/ab1558_evk/templates/dsp0_pisplit_library_build_demo
 * @endcode
 *            D. Make sure environment variables of DSP core which run the PIC library.\n
 *               Make sure to use "xt-xc++" to link.\n
 *               Make sure some compile options must be include. Like "-g" is for adding debugging information for the library build.\n
 * @code
 *  include $(ROOTDIR)/.config.mk
 *  #if you want to build dsp0 PIC library,please include dsp0 chip.mk, otherwise please use dsp1 chip.mk.
 *  #On dsp0 chip.mk will defined the dsp0 core environment variables,like:"XTENSA_CORE= dsp0_core_winabi_xtensac".
 *  include $(ROOTDIR)/config/chip/ab155x/dsp0/chip.mk
 *  #include $(ROOTDIR)/config/chip/ab155x/dsp0/chip.mk
 *
 *  # Compile option settings
 *  LD = xt-xc++
 *  CFLAGS  +=  -O2 -Wall -g  -mlongcalls -fmessage-length=0
 * @endcode
 *            E. Add your source file to build, as shown below:
 *              C_SRC for adding your xxx.c files, and ASM_SRC for adding your xxx.s files.INC for adding your header file path.\n
 * @code
 *  # Source file settings
 *  C_SRC  +=  $(strip $(PIC_MODULE_PATH))/src/lib_main.c
 *  ASM_SRC      +=
 *  # Include path settings
 *  INC  +=  $(strip $(PIC_MODULE_PATH))/inc
 * @endcode
 *              If your PIC library also need to include some static library files, for example xa_aac_loas_dec.a as shown below :
 * @code
 * # .o objects & .d dependency list
 * PICSL_LIBRARY_OBJS = $(C_SRC:%.c=$(OUTDIR)/%.picsl.o)
 * PICSL_LIBRARY_OBJS += $(ASM_SRC:%.S=$(OUTDIR)/%.picsl.o)
 * # Please add this line to here.
 * PICSL_LIBRARY_OBJS += $(ROOTDIR)/xxxxxx/xa_aac_loas_dec.a
 * @endcode
 *            F. There are three actions for this step. It is not necessary to use modify.\n
 *                  First, build your xxx.c and xxx.s file to obj file.\n
 *                  Next, link all obj files to a lib file, named by "PIC_DEBUG_LIB" in Step C, it and will also generate the library asm file:"{xxx}_position_independent.splitload.lib.asm",also for debug.\n
 *                  Finally, package the loadable library into an object file using the Xtensa tool of xt-pkg-loadlib. This generates the final output filenamed by "PIC_LIB" during Step C.\n
 * @code
 *  all :  $(PIC_LIB)
 *      @echo Build $(PIC_LIB) Done
 *
 *  $(OUTDIR)/%.picsl.o :  $(ROOTDIR)/%.S
 *      @mkdir -p $(dir $@)
 *      $(AS) $(ASFLAG) $(INC:%=-I"$(ROOTDIR)/%") -fpic -o $@ -c $^
 *
 *  $(OUTDIR)/%.picsl.o :  $(ROOTDIR)/%.c
 *      @mkdir -p $(dir $@)
 *      $(CC) $(CFLAGS) $(INC:%=-I"$(ROOTDIR)/%") -fpic -o $@ -c $^
 *
 *  $(PIC_DEBUG_LIB) : $(PICSL_LIBRARY_OBJS)
 *      @if [ -e "$@" ]; then rm -f "$@"; fi
 *      @mkdir -p $(dir $@)
 *      $(LD) $(CFLAGS) -mlsp=pisplitload -Wl,--shared-pagesize=128  $^ \ -Wl,-pie -o $@
 *      @$(OBJDUMP) -d $@ > $@.asm
 *
 *  $(PIC_LIB) : $(PIC_DEBUG_LIB)
 *      @if [ -e "$@" ]; then rm -f "$@"; fi
 *      @mkdir -p $(dir $@)
 *      xt-pkg-loadlib -p -e $(PIC) -o $@ $^
 * @endcode
 *                  Note: After library built will generate the three files under path of "dsp\project\ab1558_evk\templates\dsp_pisplit_library_build_demo\dsp0\Build\lib". these are very important:\n
 *                          - The <xxx.o> file need to link with main program.\n
 *                          - The <xxx_position_independent.splitload.lib> file is for online and offline debug.\n
 *                          - The <xxx_position_independent.splitload.lib.asm> file is the library assembly code, also for debug.\n
 *                          please keep and provide them when debug library. For AAC library as example,
 *                          Airoha had put the three files in the path of “dsp/prebuilt/middleware/dspalg/third_party/aac_decoder/pisplit”.\n
 * - \b Step 3: You can now build your library and change directory to the makefile path to use the "make" command to build. You must first "make clean".\n
 * - \b Step 4: Linking the pisplit library with main program.\n
 *          Firstly, user should makesure "PRELOADER_ENABLE = y" on by project file of feature.mk, like "dsp\project\ab1558_evk\templates\dsp0_freertos_create_thread\XT-XCC\feature.mk".
 *          Then user should add "kernel/service/pre_libloader/dsp0/module.mk" to the by project file of Makefile.
 *          Please refer to the template of "dsp\project\ab1558_evk\templates\dsp0_freertos_create_thread\XT-XCC\Makefile",
 *          add your library xxx.o as shown below:\n
 * @code
 *   include $(ROOTDIR)/kernel/service/pre_libloader/dsp0/module.mk
 *  ################################################################################
 *  # PIC lib# User can also add the dsp0_pisplit_library_build_demo.o in your owner module.mk, and include the sub module.mk on your Makefile
 *  # tempate of "dsp\kernel\service\pre_libloader\dsp0\dsp0_pic_demo_portable\module.mk".
 *  ###############################################################################
 *  PIC     += $(ROOTDIR)/project/ab1558_evk/templates/dsp_pisplit_library_build_demo/dsp0/Build/lib/dsp0_pisplit_library_build_demo.o
 *  PIC     += $(ROOTDIR)/project/ab1558_evk/templates/dsp_pisplit_library_build_demo/dsp1/Build/lib/dsp1_pisplit_library_build_demo.o
 *  ###############################################################################
 *
 *  PROJ_C_SRC   +=  $(strip $(PROJ_PATH))/src/preloader_pisplit_configure.c
 * @endcode
 * - \b Step 5: Start to run(please refer the chapter of How to use preloader of pisplit drivers).\n
 * @section Perloader_Pisplit_Library_Driver_Chapter How to use preloader of pisplit drivers
 * Diagram as below:
 * @image html preloader_How_to_run_PIC_library_single_core.png
 * - \b Step 1: You should add your pisplit library to the main program in this step. Please create preloader configure files in your project path. Please refer to "dsp\project\ab1558_evk\templates\dsp0_freertos_create_thread\inc\preloader_pisplit_configure.h","dsp\project\ab1558_evk\templates\dsp0_freertos_create_thread\src\preloader_pisplit_configure.c" and make the following changes:\n
 *          1. Define memory for pisplit libraries. On our SDK reference design, will use DSP local I/D RAM free memory for pisplit libraries. 
 *             User also can define others memory pools for pisplit libraries, for example,there is a sysram free memory pool, user can define it on preloader_pisplit_configure.c.\n
 * @code
 *  //this is the main program preloader_pisplit_configure.c
 * preloader_pisplit_pool_info_t preloader_pisplit_customer_static_pools[] =
 * {
 *     {//SYSRAM   Just a sample, sysram both can run code and data, so can define the type is PRELOADER_I_LOW_PERFORMANCE|PRELOADER_D_LOW_PERFORMANCE.
 *         sysram_for_pisplit_start,
 *         sysram_for_pisplit_end,
 *         PRELOADER_I_LOW_PERFORMANCE|PRELOADER_D_LOW_PERFORMANCE
 *     },
 *     {//PSRAM    Just a sample, psram both can run code and data, so can define the type is PRELOADER_I_LOW_PERFORMANCE|PRELOADER_D_LOW_PERFORMANCE.
 *         psram_for_pisplit_start,
 *         psram_for_pisplit_end,
 *         PRELOADER_I_LOW_PERFORMANCE|PRELOADER_D_LOW_PERFORMANCE
 *     },
 *     {//IRAM
 *         (void *)_preloader_pic_iram_start,
 *         (uint32_t)_preloader_pic_iram_end,
 *         PRELOADER_I_HIGH_PERFORMANCE
 *     },
 *     {//DRAM
 *         (void *)_preloader_pic_dram_start,
 *         (uint32_t)_preloader_pic_dram_end,
 *         PRELOADER_D_HIGH_PERFORMANCE
 *     },
 * };
 * @endcode
 *          2. preloader_pisplit_configure.h It's the most important step:\n
 *              Firstly, you should externally define your library name (it was necessary to define it during Chapter of "How to build pisplit libraries" Step 2.2.C).\n
 *              Secondly, you should add your library name to the Macro of "PIC_LIB_LIST".
 * @code
 *  //this is main program code
 *  extern xtlib_packaged_library dsp0_pisplit_library_build_demo;// user must be external to your library to here!
 *  extern xtlib_packaged_library dsp1_pisplit_library_build_demo;// user must be external to your library to here! this is DSP1 PIC library,just for DSP0 help to load DSP1 library to run.
 *  // user must add your library to this location!
 *  #define PIC_LIB_LIST    { \
 *      &dsp0_pisplit_library_build_demo, \
 *      &dsp1_pisplit_library_build_demo,\
 *  }
 * @endcode
 * - \b Step 2: Call #preloader_pisplit_init() fucntion in DSPx initialization stage.\n
 * @code
 *  //this is main program code
 *  preloader_pisplit_error_handling_t err_status;
 *  err_status = preloader_pisplit_init();
 *  if(PRELOADER_PISPLIT_XTLIB_NO_ERR != err_status)
 *  {
 *      printf("preloader_pisplit_init() error code:%d !",err_status);
 *      while(1);
 *  }
 * @endcode
 *
 * - \b Step 3: Add some memory pools for pisplit libraries.
 * @code
 *  //there is a function of pisplit_configure_static_pool() defined in preloader_pisplit_configure.c. User can call pisplit_configure_static_pool() to add your memory pools.
 * void pisplit_configure_static_pool()
 * {
 *     uint32_t count;
 *     count = sizeof(preloader_pisplit_customer_static_pools)/sizeof(preloader_pisplit_pool_info_t);
 *     preloader_pisplit_add_pools(preloader_pisplit_customer_static_pools,count);
 * }
 * @endcode
 * - \b Step 4: Define the export/import parameters clear.\n
 *              As shown in the description of "How to build pisplit libraries of Step 1.2", you can define your owner type rule of export/import parameters to implement transfer of more than one parameter.\n
 * - \b Step 5: To get handle and load your library to run when you need. Notice:Please call it after DMA init done, better in your task.\n
 *      You must register user callback by the function of #preloader_pisplit_get_handle with the second parameter.
 *      Transfer the import parameters by the function of #preloader_pisplit_load with the second parameter.
 * @code
 *  //this is main program code
 *  void * import_parameter_array[3] = {printf,NULL,NULL};
 *  if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_get_handle(&p_dsp_pisplit_library_handle,&dsp0_pisplit_library_build_demo,dsp_pisplit_library_build_demo_callback) )
 *  {
 *      printf("p_dsp_pisplit_library_handle preloader_pisplit_get_handle() error!!!!  err handle:%x",(unsigned int)p_dsp_pisplit_library_handle);
 *      while(1);
 *  }
 *  if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_load(p_dsp_pisplit_library_handle,PRELOADER_EXT_IRAM,PRELOADER_EXT_DRAM) )
 *  {
 *      printf("p_dsp_pisplit_library_handle preloader_pisplit_load() error!!!!");
 *      while(1);
 *  }
 * @endcode
 * - \b Step 6: When transfer done will notice to you by your callback function.\n
 * @code
 *  //this is main program code
 * void * import_parameter_array[3] = {printf,NULL,NULL};
 *
 * typedef char * (*p_dsp0_demo_lib_entry_1_t) (void);
 * typedef char * (*p_dsp0_demo_lib_entry_2_t) (void);
 * typedef char * (*p_dsp0_demo_lib_entry_3_t) (void);
 *
 * #define dsp0_demo_lib_entry_1   ((p_dsp0_demo_lib_entry_1_t)dsp0_pisplit_demo_library_export_parameters[0])
 * #define dsp0_demo_lib_entry_2   ((p_dsp0_demo_lib_entry_2_t)dsp0_pisplit_demo_library_export_parameters[1])
 * #define dsp0_demo_lib_entry_3   ((p_dsp0_demo_lib_entry_3_t)dsp0_pisplit_demo_library_export_parameters[2])
 * 
 *
 * preloader_pisplit_handle_t p_dsp_pisplit_library_handle;
 *
 * void dsp_pisplit_library_build_demo_callback(preloader_pisplit_handle_t handle)
 * {
 *   BaseType_t xTaskWokenByPost = pdFALSE;
 *   uint32_t *p_export_parameters;
 *   p_export_parameters = preloader_pisplit_get_export_parameter(handle,import_parameter_array);
 *   dsp0_pisplit_demo_library_export_parameters[0] =(void*)p_export_parameters[0];
 *   dsp0_pisplit_demo_library_export_parameters[1] =(void*)p_export_parameters[1];
 *   dsp0_pisplit_demo_library_export_parameters[2] =(void*)p_export_parameters[2];
 *   xSemaphoreGiveFromISR( dsp0_pisplit_demo_library_xSemaphore, &xHigherPriorityTaskWoken );
 * }
 * @endcode
 * - \b Step 7: Call your PIC library entries in your task after receive the semaphore. Strongly suggest user call your library entry functions on task level,void the IRQ handler execute too long to impact DSP real time performance.\n
 * @code
 *  //this is main program code
 *     dsp0_demo_lib_entry_1();   //call libray entry 1 function
 *     dsp0_demo_lib_entry_2(); //call libray entry 2 function
 *     dsp0_demo_lib_entry_3(); //call libray entry 3 function
 *     //if need, you can do unload for this library to release more memory.
 * @endcode
 * - \b Notice: For easier to switch from PIC library to static libary. User can create a portable layer for every library to replace Step 4~7.
 *              please reference the standard portable example of "dsp\kernel\service\pre_libloader\dsp0\dsp0_pic_demo_portable" .
 *              If defined the option of "DSP0_PISPLIT_DEMO_LIBRARY", will use run this library as PIC mode, otherwise will as static mode.
 *              Strongly suggest user should create your own xxx_lib_portable.h and xxx_lib_portable.c follow the demo.\n
 * File of dsp\kernel\service\pre_libloader\dsp0\dsp0_pic_demo_portable\module.mk :
 * @code
 * DSP0_PRE_LOAD_LIB_DEMO = kernel/service/pre_libloader/dsp0/dsp0_pic_demo_portable
 *
 * ifeq ($(PRELOADER_ENABLE),y)
 * ifeq ($(DSP0_PISPLIT_DEMO_LIBRARY),y)
 *  #if enable DSP0_PISPLIT_DEMO_LIBRARY, should extern defined "dsp0_pisplit_library_build_demo" in preloader_pisplit_configure.h, 
 *  #and add "dsp0_pisplit_library_build_demo" to "PIC_LIB_LIST"
 *  #CCFLAG += -DDSP0_PISPLIT_DEMO_LIBRARY  #user can defined to here or dsp\config\chip\ab155x\dsp0\chip.mk
 *  #ASFLAG += -DDSP0_PISPLIT_DEMO_LIBRARY  #user can defined to here or dsp\config\chip\ab155x\dsp0\chip.mk
 *  C_SRC  += $(DSP0_PRE_LOAD_LIB_DEMO)/dsp0_pisplit_demo_library_portable.c
 *  # Notice: if add your library obj file to here, then no need add the obj file again on Makefile, just include this module.mk to Makefile or your own module.mk.
 *  PIC     += $(ROOTDIR)/project/ab1558_evk/templates/dsp_pisplit_library_build_demo/dsp0/Build/lib/dsp0_pisplit_library_build_demo.o
 *  #else
 *  # user can include your static lib xxx.a to here, then the option of "DSP0_PISPLIT_DEMO_LIBRARY" if not defined, will use this static library!!!
 * endif
 * endif
 * #################################################################################
 * #include path
 * INC  += kernel/service/pre_libloader/dsp0/dsp0_pic_demo_portable
 * @endcode
 * File of dsp\kernel\service\pre_libloader\dsp0\dsp0_pic_demo_portable\dsp0_pisplit_demo_library_portable.h :
 * @code
 * #ifdef DSP0_PISPLIT_DEMO_LIBRARY
 * typedef char * (*p_dsp0_demo_lib_entry_1_t) (void);
 * typedef char * (*p_dsp0_demo_lib_entry_2_t) (void);
 * typedef char * (*p_dsp0_demo_lib_entry_3_t) (void);
 *
 * extern void *dsp0_pisplit_demo_library_import_parameters[];
 * extern void *dsp0_pisplit_demo_library_export_parameters[];
 *
 * #define dsp0_demo_lib_entry_1   ((p_dsp0_demo_lib_entry_1_t)dsp0_pisplit_demo_library_export_parameters[0])
 * #define dsp0_demo_lib_entry_2   ((p_dsp0_demo_lib_entry_2_t)dsp0_pisplit_demo_library_export_parameters[1])
 * #define dsp0_demo_lib_entry_3   ((p_dsp0_demo_lib_entry_3_t)dsp0_pisplit_demo_library_export_parameters[2])
 *
 * extern void dsp0_pisplit_demo_library_load();
 * extern void dsp0_pisplit_demo_library_unload();
 * #endif
 * @endcode
 * File of dsp\kernel\service\pre_libloader\dsp0\dsp0_pic_demo_portable\dsp0_pisplit_demo_library_portable.c :
 * @code
 * #ifdef DSP0_PISPLIT_DEMO_LIBRARY
 * SemaphoreHandle_t dsp0_pisplit_demo_library_xSemaphore = NULL;
 * extern void dsp0_pisplit_demo_library_default_function_parameter();
 * void *dsp0_pisplit_demo_library_export_parameters[3] = {dsp0_pisplit_demo_library_default_function_parameter,
 *                                                         dsp0_pisplit_demo_library_default_function_parameter,
 *                                                         dsp0_pisplit_demo_library_default_function_parameter};
 * void *dsp0_pisplit_demo_library_import_parameters[1] = {printf};
 *
 * void dsp0_pisplit_demo_library_default_function_parameter()
 * {
 *     printf("function point is NULL!!! dsp0_pisplit_demo_library not load or had been unload!!!");
 *     assert(0);
 * }
 *
 * preloader_pisplit_handle_t p_dsp_pisplit_library_handle = NULL;
 * void dsp0_pisplit_demo_library_callback(preloader_pisplit_handle_t handle)
 * {
 *     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
 *     uint32_t *p_export_parameters;
 *     p_export_parameters = preloader_pisplit_get_export_parameter(handle,dsp0_pisplit_demo_library_import_parameters);
 *
 *     printf("*p_export_parameters:0x%x",*p_export_parameters);
 *
 *     if(p_export_parameters == NULL)
 *     {
 *        printf("p_export_parameters is NULL, please check!!!");
 *     }
 *     else
 *     {
 *     //notice:Strongly suggestion: Just do receive parameters on CCNI ISR callback.
 *     //                             Then notice your thread that can execute the library entries!!!
 *         dsp0_pisplit_demo_library_export_parameters[0] =p_export_parameters[0];
 *         dsp0_pisplit_demo_library_export_parameters[1] =p_export_parameters[1];
 *         dsp0_pisplit_demo_library_export_parameters[2] =p_export_parameters[2];
 *         printf("dsp0_pisplit_demo_library_export_parameters[0]:0x%x",dsp0_pisplit_demo_library_export_parameters[0]);
 *         printf("dsp0_pisplit_demo_library_export_parameters[1]:0x%x",dsp0_pisplit_demo_library_export_parameters[1]);
 *         printf("dsp0_pisplit_demo_library_export_parameters[2]:0x%x",dsp0_pisplit_demo_library_export_parameters[2 ]);
 *          xSemaphoreGiveFromISR( dsp0_pisplit_demo_library_xSemaphore, &xHigherPriorityTaskWoken );
 *     }
 * }
 * void dsp0_pisplit_demo_library_load()
 * {
 *     if (dsp0_pisplit_demo_library_xSemaphore == NULL)
 *     {
 *         dsp0_pisplit_demo_library_xSemaphore = xSemaphoreCreateBinary();
 *
 *         if( dsp0_pisplit_demo_library_xSemaphore == NULL )
 *         {
 *             assert(0);
 *         }
 *     }
 *     if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_get_handle(&p_dsp_pisplit_library_handle,&dsp0_pisplit_library_build_demo,dsp0_pisplit_demo_library_callback) )
 *     {
 *         printf("dsp0_pisplit_library_build_demo preloader_pisplit_get_handle() error!!!!");
 *         assert(0);
 *     }
 *     if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_load(p_dsp_pisplit_library_handle,PRELOADER_I_HIGH_PERFORMANCE,PRELOADER_D_HIGH_PERFORMANCE) )
 *     {
 *         printf("dsp0_pisplit_library_build_demo preloader_pisplit_load() error!!!!");
 *         assert(0);
 *     }
 *     //notice: If you want to blocking this task when call load library, you can use this method to wait semaphore,until loading done.
 *     //        If you don't want to blocking this thread, you can miss waiting semaphonre and return directly.
 *     //            You should confirm the library had been loading done before call the library entries by yourselves.
 *     if(xSemaphoreTake( dsp0_pisplit_demo_library_xSemaphore, portMAX_DELAY) == pdTRUE)
 *     {
 *     }
 *     else
 *     {
 *         assert(0);
 *     }
 * }
 * void dsp0_pisplit_demo_library_unload()
 * {
 *     if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_unload(p_dsp_pisplit_library_handle))
 *     {
 *         printf("dsp0_pisplit_library_build_demo preloader_pisplit_unload() error!!!!");
 *         assert(0);
 *     }
 *     if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_release_handle(p_dsp_pisplit_library_handle))
 *     {
 *         printf("dsp0_pisplit_library_build_demo preloader_pisplit_release_handle() error!!!!");
 *         assert(0);
 *     }
 *
 *     dsp0_pisplit_demo_library_export_parameters[0] = dsp0_pisplit_demo_library_default_function_parameter;
 *     p_dsp_pisplit_library_handle = NULL;
 * }
 * #endif
 * @endcode
 * The most importance point is that user can use the same function symbol with static library directly, there are advantages:\n
 * 1.  Easier to switch the library from PIC mode to static mode without change any code.
 * 2.  Easy to debug. Can add trace code for library entries functions and send error log when the library not be loaded but the entry have been called.
 * @code
 * dsp0_pisplit_demo_library_load();
 * // call library entries
 *     dsp0_demo_lib_entry_1();   //call libray entry 1 function
 *     dsp0_demo_lib_entry_2(); //call libray entry 2 function
 *     dsp0_demo_lib_entry_3(); //call libray entry 3 function
 * dsp0_pisplit_demo_library_unload();
 * @endcode
 * @section Perloader_Pisplit_Library_DSP0_Load_DSP1_Chapter The feature of DSP0 help DSP1 to load PIC library
 * If DSP1 just a accelerator for DSP0, and with less memory. Can use this feature which DSP0 help DSP1 to load its PIC library.
 * Then DSP1 can cut down most services like preloader driver and SW DMA driver etc., just use less services of #preloader_pisplit_get_export_parameter to get PIC library parameters when DSP0 load library done.\n
 *  On the other hand, if DSP1 PIC memory pool was DSP1 free memory, so the information just be known until DSP1 boot up. User can use dynamic memory pool features to resolve this problem.\n
 *  Preloader Cross dual cores diagram as below:
 * @image html preloader_Dual_Core_PIC_solution_introduction.png
 *  As below just a example of DSP0 want load libraries to DSP1 I/DRAM memory pools.\n
 * \b Notice1: DSP0 and DSP1 have some communication requirement, please use the module of CCCI. User also can reference the sample file of dsp\project\ab1558_evk\templates\dsp0_freertos_create_thread\inc\ccci_configure.h
 *  and dsp\project\ab1558_evk\templates\dsp1_create_message_queue\inc\ccci_configure.h.\n
 * \b Notice2: There is a important information of when DSP0 access DSP1 I/DRAM, must be guarantee DSP1 core be in active state, because when DSP1 enter sleep, will lead DSP0 access DSP1 I/DRAM fail.
 *  So when DSP0 help DSP1 to load library to DSP1 I/DRAM, must be call hal_sleep_manager_lock_sleep(SLEEP_LOCK_DSP1_CROSS_CORE_PRELOADER) to lock DSP1 sleep before any access DSP1 I/DRAM.\n
 *  DSP1 how to run the PIC library diagram as below:
 * @image html preloader_How_to_run_PIC_library_cross_core.png
 * Please reference the sample of "dsp\kernel\service\pre_libloader\dsp0\dsp0_load_dsp1_pic_demo_portable" and "dsp\kernel\service\pre_libloader\dsp1\dsp1_pic_demo_portable".\n
 *         - \b Step 1: Define the CCCI event for cross core communication for the feature of DSP0 help DSP1 to load its PIC library.\n
 * For the dsp1 pisplit demo library, we just need several CCCI events cross DSP0 and DSP1.
 * User also can add more events, like: "data is ready" and "process is done".
 * The sample file of dsp\project\ab1558_evk\templates\dsp0_freertos_create_thread\inc\ccci_configure.h
 * @code
 *  //User should add your event which DSP1 send to DSP0
 * #define DSP1_TO_DSP0_CCCI_EVENT_LIST
 *
 *  // User should add your event which DSP0 send to DSP1
 *  //    The event of CCCI_EVENT_DSP0_TO_DSP1_PIC_MEMORY_REQUEST is mandatory for DSP0 request the information of DSP1 pools, please clone to you project!!!
 *  //    The last two events is mandatory for the library of demo library, please clone to you project!!!
 *  //    User also can add more events, like: "data is ready" and "process is done".
 * #define DSP0_TO_DSP1_CCCI_EVENT_LIST    \
 *                                          CCCI_EVENT_DSP0_TO_DSP1_PIC_MEMORY_REQUEST,\
 *                                          CCCI_EVENT_DSP0_TO_DSP1_PIC_DEMO_LIBRARY_LOAD_DONE,\
 *                                          CCCI_EVENT_DSP0_TO_DSP1_PIC_DEMO_LIBRARY_UNLOAD_DONE,
 *
 *  //User should add your event and callback to here
 * #define DSP0_RECEIVE_CCCI_FROM_DSP1_USER_ARRAY
 * @endcode
 * The sample file of dsp\project\ab1558_evk\templates\dsp1_create_message_queue\inc\ccci_configure.h
 * @code
 *     extern void prepare_pic_pool_info_to_dsp0();
 *     extern void dsp1_pisplit_lib_demo_load_done();
 *     extern void dsp1_pisplit_lib_demo_unload_done();
 *
 *     //User should add your event which DSP1 send to DSP0
 *#define DSP1_TO_DSP0_CCCI_EVENT_LIST
 *
 *     // User should add your event which DSP0 send to DSP1
 *     //   The event of CCCI_EVENT_DSP0_TO_DSP1_PIC_MEMORY_REQUEST is mandatory for DSP0 request the information of DSP1 pools, please clone to you project!!!
 *     //   The last two events is mandatory for the library of demo library, please clone to you project!!!
 *     //   User also can add more events, like: "data is ready" and "process is done".
 *#define DSP0_TO_DSP1_CCCI_EVENT_LIST    \
 *                                             CCCI_EVENT_DSP0_TO_DSP1_PIC_MEMORY_REQUEST,\
 *                                             CCCI_EVENT_DSP0_TO_DSP1_PIC_DEMO_LIBRARY_LOAD_DONE,\
 *                                             CCCI_EVENT_DSP0_TO_DSP1_PIC_DEMO_LIBRARY_UNLOAD_DONE,
 *
 *     //User should add your event and callback to here
 *     //The event of CCCI_EVENT_DSP0_TO_DSP1_PIC_MEMORY_REQUEST is mandatory for DSP0 request the information of DSP1 pools, please clone to you project!!!
 *     //The last callbacks just for the demo library control, if you want to create new library, please clone.
 *#define DSP1_RECEIVE_CCCI_FROM_DSP0_USER_ARRAY  \
 *                  {CCCI_EVENT_DSP0_TO_DSP1_PIC_MEMORY_REQUEST, prepare_pic_pool_info_to_dsp0},\
 *                  {CCCI_EVENT_DSP0_TO_DSP1_PIC_DEMO_LIBRARY_LOAD_DONE, dsp1_pisplit_lib_demo_load_done},\
 *                  {CCCI_EVENT_DSP0_TO_DSP1_PIC_DEMO_LIBRARY_UNLOAD_DONE, dsp1_pisplit_lib_demo_unload_done}, *
 *
 * @endcode
 *         - \b Step 2: DSP0 should request the DSP1 memory pools information via CCCI event of CCCI_EVENT_DSP0_TO_DSP1_PIC_MEMORY_REQUEST.
 *              And DSP0 add the pools to preloader driver. Then the DSP0 can help DSP1 to load its PIC library.\n
 * User need define a structure of dynamic pool for DSP0 preloader_pisplit driver to manage DSP1 free I/DRAM memory pool.
 * After DSP1 boot up, DSP0 should request DSP1 to prepare the memory pools information and write to this structure.
 * User must pay attention to this key point of the structure of preloader_pisplit_customer_dynamic_pools.
 * Because of it defined and read on DSP0 side, and write on DSP1 side. To void will be have data coherency issue,
 * user should guarantee the structure must be non-cacheable for DSP0 and DSP1.\n
 * DSP0 side sample code, file of dsp\project\ab1558_evk\templates\dsp0_freertos_create_thread\src\preloader_pisplit_configure.c:
 * @code
 * //User must pay attention to this key point of the structure of preloader_pisplit_customer_dynamic_pools.
 * //Because of it defined and read on DSP0 side, and write on DSP1 side. To void will be have data coherency issue,
 * //user should guarantee the structure must be non-cacheable for DSP0 and DSP1.
 * //On this example project, RW data default put on DSP0 DRAM, should it is non-cacheable for DSP0.
 * //Because DSP1 have no cache, so DSP0 DRAM also non-cacheable for DSP1.
 * preloader_pisplit_pool_info_t preloader_pisplit_customer_dynamic_pools[] =
 * {
 *     {//dsp1 iram
 *         NULL,
 *        0,
 *         PRELOADER_EXT_IRAM
 *     },
 *     {//dsp1 dram
 *         NULL,
 *         0,
 *         PRELOADER_EXT_DRAM
 *     },
 * };
 * #ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
 * //This function implement on by project level, for register the dynamic PIC memory pools to Preloader_pisplit driver.
 * //For the feature of DSP0 load DSP1 PIC library, also need power on and let DSP1 running firstly, then can call pisplit_configure_dynamic_pool() to
 * //register DSP1 I/DRAM to PIC pools, because need power on the memory.
 * //Please clone this function to your project!!!
 *
 * //example code on init flow:
 * //   preloader_pisplit_init();
 * //   pisplit_configure_static_pool();
 * //   printf("\r\n#### DSP0 Begin to Reset DSP1... ####\r\n");
 * //   hal_dsp_core_reset(HAL_CORE_DSP1, DSP1_BASE);
 * // pisplit_configure_dynamic_pool();
 *
 * void pisplit_configure_dynamic_pool()
 * {
 *     ccci_msg_t ccci_msg;
 *     uint32_t pools;
 *     ccci_msg.event = CCCI_EVENT_DSP0_TO_DSP1_PIC_MEMORY_REQUEST;
 *     ccci_msg.data = hal_memview_dsp0_to_infrasys((uint32_t)preloader_pisplit_customer_dynamic_pools);
 *     //send CCCI to DSP1 to request DSP1 I/DRAM free memory,must be wait for got this information
 *     printf("DSP0 to reuqest DSP1 free I/DRAM info...");
 *     if(CCCI_STATUS_OK != ccci_send_msg(HAL_CORE_DSP1,ccci_msg,CCCI_SEND_MSG_WAIT_FOR_SEND_RECEIVE_DONE))
 *     {
 *         assert(0);
 *     }
 *     if( (preloader_pisplit_customer_dynamic_pools[0].pool_start == NULL) || (preloader_pisplit_customer_dynamic_pools[1].pool_start == NULL) )
 *     {
 *         printf("pisplit_configure_dynamic_pool get DSP1 fre I/DRAM fail!!!");
 *         assert(0);
 *     }
 *     pools = sizeof(preloader_pisplit_customer_dynamic_pools)/sizeof(preloader_pisplit_pool_info_t);
 *     preloader_pisplit_add_pools(preloader_pisplit_customer_dynamic_pools,pools);
 * }
 * endif
 * @endcode
 *  DSP1 side sample code of receive DSP0 request and prepare DSP1 free I/DRAM information for DSP0,
 * file of dsp\project\ab1558_evk\templates\dsp1_create_message_queue\src\preloader_pisplit_configure.c:
 * @code
 * extern uint32_t _preloader_pic_iram_start[];    //this is DSP1 IRAM free start, defined in memory layout link script
 * extern uint32_t _preloader_pic_iram_end[];      //this is DSP1 IRAM free end, defined in memory layout link script
 * 
 * extern uint32_t _preloader_pic_dram_start[];    //this is DSP1 DRAM free start, defined in memory layout link script
 * extern uint32_t _preloader_pic_dram_end[];      //this is DSP1 DRAM free end, defined in memory layout link script
 * 
 * //For DSP0 help DSP1 to load its PIB library.
 * //This function is the callback of  event <CCCI_EVENT_DSP0_TO_DSP1_PIC_MEMORY_REQUEST>
 * //When DSP0 send the event of CCCI_EVENT_DSP0_TO_DSP1_PIC_MEMORY_REQUEST, the function will be called to prepare DSP1 memory pools information to DSP0.
 * //For DSP0 request DSP1 I/D RAM free memory information.
 * //User should clone this function to your DSP1 project if you want use the feature of DSP0 load DSP1 PIC library.
 * //void prepare_pic_pool_info_to_dsp0(ccci_msg_t ccci_msg)
 * {
 *     preloader_pisplit_pool_info_t *pool_info;
 *     if(ccci_msg.event != CCCI_EVENT_DSP0_TO_DSP1_PIC_MEMORY_REQUEST)
 *         assert(0);
 *
 *     pool_info = hal_memview_infrasys_to_dsp1(ccci_msg.data);
 *     printf("DSP1 prepare pic memory pool...");
 *     pool_info->pool_start = (void*)_preloader_pic_iram_start;
 *     pool_info->pool_end = (uint32_t)_preloader_pic_iram_end;
 *     (pool_info+1)->pool_start = (void*)_preloader_pic_dram_start;
 *     (pool_info+1)->pool_end = (uint32_t)_preloader_pic_dram_end;
 *
 *     printf("DSP1 prepare pic iram memory pool info (start:0x%x,end:0x%x)",(uint32_t)_preloader_pic_iram_start,(uint32_t)_preloader_pic_iram_end);
 *     printf("DSP1 prepare pic dram memory pool info (start:0x%x,end:0x%x)",(uint32_t)_preloader_pic_dram_start,(uint32_t)_preloader_pic_dram_end);
 * }

 * @endcode
 *         - \b Step 3: DSP0 do loading libraries. Should notify DSP1 after loading done.\n
 * So on DSP0 preloader callback should send CCCI event with one word information of handler to DSP1, 
 * let DSP1 know the library had been loaded done. Then use the one word information of handler to get the library entry point on DSP1 side.
 * DSP0 side sample code, file of dsp\kernel\service\pre_libloader\dsp0\dsp0_load_dsp1_pic_demo_portable\dsp0_load_dsp1_pisplit_demo_library_portable.c
 * @code
 * #ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
 * #ifdef DSP1_PISPLIT_DEMO_LIBRARY
 * preloader_pisplit_handle_t p_dsp0_load_dsp1_pisplit_library_handle;
 * 
 * void dsp0_load_dsp1_pisplit_demo_library_callback(preloader_pisplit_handle_t handle)
 * {
 *     ccci_msg_t ccci_msg;
 *     printf("this is dsp0_load_dsp1_pisplit_demo_library_callback");
 *     //just for error check and debug
 *     //if(handle != p_dsp0_load_dsp1_pisplit_library_handle)
 *     //{
 *     //     printf("main error !!! p_dsp0_load_dsp1_pisplit_library_handle=0x%x handle=0x%x ---",(uint32_t)p_dsp0_load_dsp1_pisplit_library_handle,(uint32_t)handle);
 *     //}
 *     //printf("p_dsp0_load_dsp1_pisplit_library_handle:0x%x",(uint32_t)p_dsp0_load_dsp1_pisplit_library_handle);
 *     ccci_msg.event = CCCI_EVENT_DSP0_TO_DSP1_PIC_DEMO_LIBRARY_LOAD_DONE;
 *     ccci_msg.data = hal_memview_dsp0_to_infrasys((uint32_t)handle);
 *     if(CCCI_STATUS_OK != ccci_send_msg(HAL_CORE_DSP1,ccci_msg,CCCI_SEND_MSG_NO_WAIT))
 *     {
 *         assert(0);
 *     }
 * }
 * 
 * void dsp0_load_dsp1_pisplit_demo_library_load()
 * {
 *     if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_get_handle(&p_dsp0_load_dsp1_pisplit_library_handle,&dsp1_pisplit_library_build_demo,dsp0_load_dsp1_pisplit_demo_library_callback) )
 *     {
 *         printf("dsp0_load_dsp1_pisplit_demo_library_load preloader_pisplit_get_handle() error!!!!");
 *         assert(0);
 *     }
 *     if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_load(p_dsp0_load_dsp1_pisplit_library_handle,PRELOADER_EXT_IRAM,PRELOADER_EXT_DRAM) )
 *     {
 *         printf("dsp0_load_dsp1_pisplit_demo_library_load preloader_pisplit_load() error!!!!");
 *         assert(0);
 *     }
 * 
 * }
 * 
 * void dsp0_load_dsp1_pisplit_demo_library_unload()
 * {
 *     ccci_msg_t ccci_msg;
 *     if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_unload(p_dsp0_load_dsp1_pisplit_library_handle))
 *     {
 *         printf("dsp0_load_dsp1_pisplit_demo_library_unload preloader_pisplit_unload() error!!!!");
 *         assert(0);
 *     }
 *     if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_release_handle(p_dsp0_load_dsp1_pisplit_library_handle))
 *     {
 *         printf("dsp0_load_dsp1_pisplit_demo_library_unload preloader_pisplit_release_handle() error!!!!");
 *         assert(0);
 *     }
 * 
 *     ccci_msg.event = CCCI_EVENT_DSP0_TO_DSP1_PIC_DEMO_LIBRARY_UNLOAD_DONE;
 *     ccci_msg.data = 0;
 *     if(CCCI_STATUS_OK != ccci_send_msg(HAL_CORE_DSP1,ccci_msg,CCCI_SEND_MSG_NO_WAIT))
 *     {
 *         assert(0);
 *     }
 * }
 * #endif
 * #endif
 * @endcode
 *         - \b Step 4: DSP1 can call #preloader_pisplit_get_export_parameter() to put and get parameter to use the libraries.\n
 *         - \b Step 5: DSP0 do unloading libraries. Should can notify DSP1 after unloading done.\n
 * DSP1 side sample code, file of dsp\kernel\service\pre_libloader\dsp1\dsp1_pic_demo_portable\dsp1_pisplit_demo_library_portable.h
 * @code
 * #ifdef DSP1_PISPLIT_DEMO_LIBRARY
 * typedef char * (*p_dsp1_demo_lib_entry_1_t) (void);
 * typedef char * (*p_dsp1_demo_lib_entry_2_t) (void);
 * typedef char * (*p_dsp1_demo_lib_entry_3_t) (void);
 * extern void *dsp1_pisplit_demo_library_import_parameters[];
 * ///for export parameters*************************************************
 * extern void *dsp1_pisplit_demo_library_export_parameters[];
 * #define dsp1_demo_lib_entry_1    ((p_dsp1_demo_lib_entry_1_t)dsp1_pisplit_demo_library_export_parameters[0])
 * #define dsp1_demo_lib_entry_2    ((p_dsp1_demo_lib_entry_2_t)dsp1_pisplit_demo_library_export_parameters[1])
 * #define dsp1_demo_lib_entry_3    ((p_dsp1_demo_lib_entry_3_t)dsp1_pisplit_demo_library_export_parameters[2])
 * // **********************************************************************
 * #endif
 * @endcode
 * DSP1 side sample code, file of dsp\kernel\service\pre_libloader\dsp1\dsp1_pic_demo_portable\dsp1_pisplit_demo_library_portable.c
 * @code
 * #ifdef DSP1_PISPLIT_DEMO_LIBRARY
 * extern void dsp1_pisplit_demo_library_default_function_parameter();
 * void *dsp1_pisplit_demo_library_export_parameters[3] = {dsp1_pisplit_demo_library_default_function_parameter,
 *                                                         dsp1_pisplit_demo_library_default_function_parameter,
 *                                                         dsp1_pisplit_demo_library_default_function_parameter};
 * void *dsp1_pisplit_demo_library_import_parameters[1] = {printf};
 * void dsp1_pisplit_demo_library_default_function_parameter()
 * {
 *     printf("function point is NULL!!! dsp1_pisplit_demo_library not load or had been unload!!!");
 *     assert(0);
 * }
 * preloader_pisplit_handle_t p_dsp1_pisplit_demo_library_handle = NULL;
 *
 * void dsp1_pisplit_lib_demo_load_done(ccci_msg_t ccci_msg)
 * {
 *     uint32_t *p_export_parameters;
 *     uint32_t data = ccci_msg.data;
 *     if(ccci_msg.event != CCCI_EVENT_DSP0_TO_DSP1_PIC_DEMO_LIBRARY_LOAD_DONE)
 *         assert(0);
 *     p_dsp1_pisplit_demo_library_handle = hal_memview_infrasys_to_dsp1(data);
 *     printf("p_dsp1_pisplit_demo_library_handle:0x%x",(uint32_t)p_dsp1_pisplit_demo_library_handle);
 *
 *     p_export_parameters = preloader_pisplit_get_export_parameter(p_dsp1_pisplit_demo_library_handle,dsp1_pisplit_demo_library_import_parameters);
 *     if(p_export_parameters == NULL)
 *     {
 *        printf("p_export_parameters is NULL, please check!!!");
 *        assert(0);
 *     }
 *     else
 *     {
 *     //notice:Strongly suggestion: Just do receive parameters on CCNI ISR callback. 
 *     //                              Then notice your thread that can execute the library entries!!!
 *         dsp1_pisplit_demo_library_export_parameters[0] =p_export_parameters[0];
 *         dsp1_pisplit_demo_library_export_parameters[1] =p_export_parameters[1];
 *         dsp1_pisplit_demo_library_export_parameters[2] =p_export_parameters[2];
 *         printf("dsp1_pisplit_demo_library_export_parameters[0]:0x%x",dsp1_pisplit_demo_library_export_parameters[0]);
 *         printf("dsp1_pisplit_demo_library_export_parameters[1]:0x%x",dsp1_pisplit_demo_library_export_parameters[1]);
 *         printf("dsp1_pisplit_demo_library_export_parameters[2]:0x%x",dsp1_pisplit_demo_library_export_parameters[2 ]);
 *     }
 * }
 *
 * void dsp1_pisplit_lib_demo_unload_done(ccci_msg_t ccci_msg)
 * {
 *     if(ccci_msg.event != CCCI_EVENT_DSP0_TO_DSP1_PIC_DEMO_LIBRARY_UNLOAD_DONE)
 *         assert(0);
 *
 *     dsp1_pisplit_demo_library_export_parameters[0] =dsp1_pisplit_demo_library_default_function_parameter;
 *     dsp1_pisplit_demo_library_export_parameters[1] =dsp1_pisplit_demo_library_default_function_parameter;
 *     dsp1_pisplit_demo_library_export_parameters[2] =dsp1_pisplit_demo_library_default_function_parameter;
 * }
 * #endif
 *
 * @endcode
 * DSP1 side sample code, user can call the library entries on user thread.
 * @code
 *     dsp1_demo_lib_entry_1();
 *     dsp1_demo_lib_entry_2();
 *     dsp1_demo_lib_entry_3();
 * @endcode
 *    Notice: As the description above, DSP1 side just need one API of #preloader_pisplit_get_export_parameter when DSP0 help DSP1 to load PIC library.\n
 * @section Perloader_Pisplit_Library_How_To_Debug_Chapter How to debug pisplit library
 *  Xplorer or Xtensa OCD both support pisplit library debugging and they must both have the library lib file: "dsp\project\ab1558_evk\templates\dsp_pisplit_library_build_demo\dsp0\Build\lib\dsp0_pisplit_library_build_demo_position_independent.splitload.lib".\n
 * - \b How \b to \b debug \b with \b Xplorer.\n
 *       - \b Step 1: Debug main program success.\n
 *       - \b Step 2: Must get the relocation address of the running library.\n
 *          You can get the address using one of two methods:
 *          1. Refer to the log. When library loaded, the address is sent as "###debug###important info## the (xtlib_pi_library_debug_addr) lib_info.text_addr:0x300410f0". The address of 0x300410f0 is the library relocation address.\n
 *          2. Use online debugging to see the variable of "g_preloader_pisplit_library_info[x].lib_info.text_addr" to get the address.\n
 *       - \b Step 3: Add library debug symbol.\n
 *          1. Select and right click the menu of "Debug Program".\n
 *          2. Select and right click the menu of "Load Alternate Symbols".\n
 *          3. Select and right click the menu of "Select...".\n
 *          @image html preloader_how_to_debug_with_Xplorer_1.png
 *          4. Select the  menu of "Add these new symbols to currently ...".\n
 *          5. Write the running library relocation address.
 *          6. Select the  menu of "Choose from file system" and find the file of xxx_position_independent.splitload.lib.\n
 *          7. Click the button of "OK".\n
 *          @image html preloader_how_to_debug_with_Xplorer_2.png
 *       - \b Step 4: Then your library symbol will be add success.
 *          @image html preloader_how_to_debug_with_Xplorer_3.png
 * - \b How \b to \b debug \b with \b Xtensa \b OCD.\n
 *      The operations are very similar to the Xplorer procedure. Simply replace the Step 3 to use the cmd of "add-symbol-file Z:\\dsp\\project\\ab1558_evk\\templates\\dsp_pisplit_library_build_demo\\dsp0\\Build\\lib\\dsp_pisplit_library_build_demo_position_independent.splitload.lib 0x300410f0".\n 
 */

#ifdef __cplusplus
    extern "C" {
#endif

/*****************************************************************************
 * Enums
 *****************************************************************************/

/** @defgroup preloader_enum Enum
 *  @{
 */

/** @brief This enum defines the preloader error status. */
typedef enum {
    PRELOADER_PISPLIT_XTLIB_NO_ERR = XTLIB_NO_ERR,                            /**< Defines preloader status of no error */
    PRELOADER_PISPLIT_XTLIB_NOT_ELF = XTLIB_NOT_ELF,                          /**< Defines preloader status of not elf */
    PRELOADER_PISPLIT_XTLIB_NOT_DYNAMIC = XTLIB_NOT_DYNAMIC,                  /**< Defines preloader status of not dynamic */
    PRELOADER_PISPLIT_XTLIB_NOT_STATIC = XTLIB_NOT_STATIC,                    /**< Defines preloader status of not static */
    PRELOADER_PISPLIT_XTLIB_NO_DYNAMIC_SEGMENT = XTLIB_NO_DYNAMIC_SEGMENT,    /**< Defines preloader status of not synamic segment */
    PRELOADER_PISPLIT_XTLIB_UNKNOWN_SYMBOL = XTLIB_UNKNOWN_SYMBOL,            /**< Defines preloader status of unknown symbol */
    PRELOADER_PISPLIT_XTLIB_NOT_ALIGNED = XTLIB_NOT_ALIGNED,                  /**< Defines preloader status of not aligned */
    PRELOADER_PISPLIT_XTLIB_NOT_SPLITLOAD = XTLIB_NOT_SPLITLOAD,              /**< Defines preloader status of not split load */
    PRELOADER_PISPLIT_XTLIB_RELOCATION_ERR = XTLIB_RELOCATION_ERR,            /**< Defines preloader status of relocation error */
    PRELOADER_PISPLIT_ERROR_STATIC_LIB_INFO = 0x10,                           /**< Defines preloader status of error static lib info */
    PRELOADER_PISPLIT_ERROR_GET_HANDLE = 0x11,                                /**< Defines preloader status of get handle error */
    PRELOADER_PISPLIT_ERROR_HANDLE = 0x12,                                    /**< Defines preloader status of error handle */
    PRELOADER_PISPLIT_ERROR_CUSTOMER_CALLBACK = 0x13,                         /**< Defines preloader status of error callback */
    PRELOADER_PISPLIT_ERROR_USER_CALLFLOW = 0x14,                             /**< Defines preloader status of error callflow */
    PRELOADER_PISPLIT_ERROR_ALLOC_MEM_FAIL = 0x15,                            /**< Defines preloader status of alloc memory fail */
    PRELOADER_PISPLIT_ERROR = 0x16,                                           /**< Defines preloader status of misc error */
} preloader_pisplit_error_handling_t;

/** @brief This enum defines preloader malloc memory type, just for adaptability, user should configure which type is which memory.*/
typedef enum {
    PRELOADER_I_HIGH_PERFORMANCE = 1<<0,   /**< Defines the type of high performance code memory for pisplit, like: DSP0 IRAM */
    PRELOADER_D_HIGH_PERFORMANCE = 1<<1,   /**< Defines the type of high performance data memory for pisplit, like: DSP0 DRAM */
    PRELOADER_I_LOW_PERFORMANCE = 1<<2,    /**< Defines the type of low performance code memory for pisplit, like: SYSRAM */
    PRELOADER_D_LOW_PERFORMANCE = 1<<3,    /**< Defines the type of low performance data memory for pisplit, like: SYSRAM */
#ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
    PRELOADER_EXT_IRAM = 1<<4,             /**< Defines the type of external code memory for pisplit, like: DSP1 IRAM. For the feature of DSP0 help DSP1 to load its PIC library.*/
    PRELOADER_EXT_DRAM = 1<<5,             /**< Defines the type of external data memory for pisplit, like: DSP1 DRAM. For the feature of DSP0 help DSP1 to load its PIC library.*/
#endif
} preloader_pisplit_memory_type_t;

/**
 * @}
 */

/** @defgroup preloader_struct Struct
 *  @{
 */
/** @brief This struct defines preloader user customization configure structure */
typedef struct {
    void *pool_start;       /**< Defines the start address of the memory pool */
    unsigned int pool_end;  /**< Defines the end address of the memory pool */
    preloader_pisplit_memory_type_t pool_type; /**< Defines the type of the memory pool, the type is #preloader_pisplit_memory_type_t. User can define a memory pool have two type, for example, SYSRAM can store code and data, so user can define SYSRAM memory pool with the type of PRELOADER_I_LOW_PERFORMANCE|PRELOADER_D_LOW_PERFORMANCE */
} preloader_pisplit_pool_info_t;
/**
 * @}
 */

/** @defgroup preloader_define Define
  * @{
  */
  /** This macro is used to define the max pools for pisplit libraries.
  */
#define PRELOADER_PISPLIT_SUPPORT_MAX_MULTI_POOLS 8
/**
  * @}
  */

/*****************************************************************************
 * Typedef
 *****************************************************************************/

/** @defgroup preloader_typedef Typedef
 *  @{
 */

/** @brief This typedef defines preloader handle type.
 * When user want to use preloader to load a splitpi library, should define a handle.
 */
typedef void* preloader_pisplit_handle_t;


/** @brief This typedef defines user's callback function prototype.
 *             This callback function will be called in DMA interrupt handler when DMA transfer done for loading library image.
 *             User should call preloader_pisplit_get_handle() to register callbacks to preloader driver explicitly. \n
 *             parameter "handle" : for return to user the handle which user get from preloader_pisplit_get_handle().
 */
typedef void (*preloader_pisplit_customer_callback_t) (preloader_pisplit_handle_t handle);




/**
 * @}
 */


/*****************************************************************************
 * Functions
 *****************************************************************************/

/**
 * @brief     This function initializes the preloader hardware and software with basic functionality.
 * @return
 *                #PRELOADER_PISPLIT_XTLIB_NO_ERR if OK. \n
 *                other return value please reference #preloader_pisplit_error_handling_t. \n
 * @note
 *            User must call one time for preloader basic environment setup.
 * @code
 *          preloader_pisplit_error_handling_t err_status;
 *          if(PRELOADER_PISPLIT_XTLIB_NO_ERR != err_status)
 *          {
 *              printf("preloader_pisplit_init() error code:%d !!!!",err_status);
 *              while(1);
 *          }
 * @endcode
 */

preloader_pisplit_error_handling_t preloader_pisplit_init(void);
/**
 * @brief     This function get handle for a pisplit library.
 * @param[in] *handle   defined by user and to get the pisplit library handle by this function.
 * @param[in] *library   specifies the pisplit library.
 * @param[in] complete_callback_func   specifies the callback function called by preloader DMA irq handler when pre-load library done.
 * @return
 *                #PRELOADER_PISPLIT_XTLIB_NO_ERR if OK. \n
 *                other return value please reference #preloader_pisplit_error_handling_t. \n
 *
 * @note
 *            User must call preloader_pisplit_init() before calling this function.
 *            User can call the function one time for one library just for get the handle.
 */
preloader_pisplit_error_handling_t preloader_pisplit_get_handle(preloader_pisplit_handle_t *handle,xtlib_packaged_library * library,preloader_pisplit_customer_callback_t complete_callback_func);



/**
 * @brief     This function add memory pools for pisplit libraries.
 * @param[in] preloader_pisplit_pools   user need to define which pools will used by pisplit libraries.
 * @param[in] pools_number   how many pools need added on this time.
 * @return
 *                #PRELOADER_PISPLIT_XTLIB_NO_ERR if OK. \n
 *                other return value please reference #preloader_pisplit_error_handling_t. \n
 *
 * @note
 *            User must define a customized pools for pisplit libraries,like:preloader_pisplit_configure.c
 *            User can call this function more than one time, for add difference pools on difference time, for example, use can define some static pools and call this function when system initialization phase. Also can prepare some dynmaic pools by run time to call this function.
 */
preloader_pisplit_error_handling_t preloader_pisplit_add_pools(preloader_pisplit_pool_info_t preloader_pisplit_pools[],uint32_t pools_number);


/**
 * @brief     This function do a pisplit library dynamic pre-load.
 *                  The function is non-blocking, just allocate code and data region memory and start DMA to do pre-load,so the pre-load job is ongoing when this function return. 
 *                  When DMA tansfer done, the customer callback will be called.
 * @param[in] *handle   specifies user pisplit library handle.
 * @param[in] code_memory_type  specifies which memory type need to load for code
 * @param[in] data_memory_type  specifies which memory type need to load for data
 * @return
 *                #PRELOADER_PISPLIT_XTLIB_NO_ERR if OK. \n
 *                other return value please reference #preloader_pisplit_error_handling_t. \n
 *
 * @note
 *            User must the pisplit library handle before calling this function.
 */
preloader_pisplit_error_handling_t preloader_pisplit_load(preloader_pisplit_handle_t handle,preloader_pisplit_memory_type_t code_memory_type,preloader_pisplit_memory_type_t data_memory_type);

/**
 * @brief     This function do a pisplit library unload.
 *                  If the library will never used or the memory not enough for other critical pispilt library, user can call this function to do unload.Then the memory will be free.
 *                  After unload, When user want to call this library,should be call preloader_pisplit_load() to load this library again.
 * @param[in] *handle   specifies user pisplit library handle.
 * @return
 *                #PRELOADER_PISPLIT_XTLIB_NO_ERR if OK. \n
 *                other return value please reference #preloader_pisplit_error_handling_t. \n
 *
 * @note
 *            User call this memory can free the memory allocated by this library.
 */
preloader_pisplit_error_handling_t preloader_pisplit_unload(preloader_pisplit_handle_t handle);

/**
 * @brief     This function for transfer the import and export parameters between main program and pisplit libraries.
 *            After called preloader_pisplit_load(), when the libraries loading done. User can call this function on the customer callback to do parameters transfer. 
 * @param[in] *handle   specifies user pisplit library handle.
 * @param[in] *p_import_parameters   specifies the input parameter for the library, for example, the library want to call some symbols defined by main program, the symbols can be transfered by this parameter.
 * @return
 *                #PRELOADER_PISPLIT_XTLIB_NO_ERR if OK. \n
 *                other return value please reference #preloader_pisplit_error_handling_t. \n
 *
 * @note
 *            User call this memory can free the memory allocated by this library.
 */
void* preloader_pisplit_get_export_parameter(preloader_pisplit_handle_t handle,void *p_import_parameters[]);

/**
 * @brief     This function for malloc memory from preloader pisplit heap memory.\n
 *            Because SDK reserved all free memory for preloader pisplit heap,so if use want to use extra memory, can use this API to malloc.\n
 *            Actually, the memory of preloader pisplit heap just for the memory allocation of library code region and data region. User don't care this, the preloader API of #preloader_pisplit_load will do it.\n
 *            But if a pisplit library need a extra data memory allocated by main program, can use this API to allocate memory from preloader heap. \n
 * @param[in] memory_type   defined the memory type of preloader heap.\n
 * @param[in] size          defined the mmeory of size
 *
 * @return
 *                the allocate buffer address. If the return value is NULL means allocate memory fail.\n
 * @note
 * @code
 *    // for example, a pisplit library need a extra data memory allocated by main program.
 *      // the function of lib_foo(void* memory) is the library function
 *      //the function of lib_foo_need_memory() also is the library function, and it can query the data memory requirement by the function of lib_foo(). 
 *      //pseudocode as below:
 *      preloader_pisplit_load(XXX,YYY);
 *      // load library firstly,when load done
 *      //now to call library entries on mian program
 *      uint32_t = memory_size;
 *      void* extra_memory;
 *      memory_size = lib_foo_need_memory();
 *      extra_memory = preloader_pisplit_malloc_memory(PRELOADER_DRAM,memory_size);
 *      foo(extra_memory);
 *      preloader_pisplit_free_memory(PRELOADER_DRAM,extra_memory);
 *      preloader_pisplit_unload(XXX);
 * @endcode
 */
void* preloader_pisplit_malloc_memory(preloader_pisplit_memory_type_t memory_type,uint32_t size);


/**
 * @brief     This function for free memory from preloader pisplit heap memory.\n
 * @param[in] *buffer_addr  defined buffer address.
 *
 * @return
 *                #PRELOADER_PISPLIT_XTLIB_NO_ERR if OK. \n
 */
preloader_pisplit_error_handling_t preloader_pisplit_free_memory(void *buffer_addr);


/**
 * @brief     This function for query the max size of continuous memory block in the preloader pisplit heap memory.\n
 * @param[in] memory_type   defined the memory type of preloader heap.\n
 *
 * @return
 *                the max size of continuous memory block in the preloader pisplit heap memory.\n
 */
uint32_t preloader_pisplit_get_max_free_block_size(preloader_pisplit_memory_type_t memory_type);



/**
 * @brief     This function for query the library relocation size.\n
 * @param[in] *handle   specifies user pisplit library handle.
 * @param[in] *p_code_size   get the library code size.
 * @param[in] *p_data_size   get the library data size.
 * @return
 *                #PRELOADER_PISPLIT_XTLIB_NO_ERR if OK. \n
 * @code
 *      uint32_t data_size,code_size;
 *      preloader_pisplit_query_library_size(p_dsp_pisplit_library_handle,&code_size,&data_size);
 *      printf("code size:%d, data size:%d",code_size,data_size);
 * @endcode
 */
preloader_pisplit_error_handling_t preloader_pisplit_query_library_size(preloader_pisplit_handle_t handle,uint32_t *p_code_size,uint32_t*p_data_size);


/**
 * @brief     This function do a pisplit library handle release.
 * @param[in] *handle   specifies user pisplit library handle.
 * @return
 *                #PRELOADER_PISPLIT_XTLIB_NO_ERR if OK. \n
 *                other return value please reference #preloader_pisplit_error_handling_t. \n
 *
 * @note
 *            User no need to release the hande every time unless the library will never be used.
 */
preloader_pisplit_error_handling_t preloader_pisplit_release_handle(preloader_pisplit_handle_t handle);

#ifdef MTK_SUPPORT_HEAP_DEBUG
/**
 * @brief     This function to dump the all block information of memory used by pisplit library.
 * @param[in] memory_type   defined the memory type of preloader heap.\n
 * @return
 *                #PRELOADER_PISPLIT_XTLIB_NO_ERR if OK. \n
 *                other return value please reference #preloader_pisplit_error_handling_t. \n
 *
 * @note
 *            User can dump the current block information of memory used by pisplit library if user need.
 */
preloader_pisplit_error_handling_t preloader_pisplit_dump_pic_memory_heap_info(preloader_pisplit_memory_type_t memory_type);
#endif

#ifdef __cplusplus
}
#endif
/**
 * @}
 * @}
*/
#endif /* __PRELOADER_PISPLIT__ */
