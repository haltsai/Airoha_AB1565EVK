

#ifndef  __DSP0_PISPLIT_DEMO_LIBRARY_PORTABLE_H__
#define  __DSP0_PISPLIT_DEMO_LIBRARY_PORTABLE_H__



#ifdef DSP0_PISPLIT_DEMO_LIBRARY

typedef char * (*p_dsp0_demo_lib_entry_1_t) (void);
typedef char * (*p_dsp0_demo_lib_entry_2_t) (void);
typedef char * (*p_dsp0_demo_lib_entry_3_t) (void);


extern void *dsp0_pisplit_demo_library_import_parameters[];


/*for export parameters*************************************************/
extern void *dsp0_pisplit_demo_library_export_parameters[];

#define dsp0_demo_lib_entry_1   ((p_dsp0_demo_lib_entry_1_t)dsp0_pisplit_demo_library_export_parameters[0])     //Have this Macro, will esay to porting your library.
#define dsp0_demo_lib_entry_2   ((p_dsp0_demo_lib_entry_2_t)dsp0_pisplit_demo_library_export_parameters[1])   //Have this Macro, will esay to porting your library.
#define dsp0_demo_lib_entry_3   ((p_dsp0_demo_lib_entry_3_t)dsp0_pisplit_demo_library_export_parameters[2])   //Have this Macro, will esay to porting your library.

/***********************************************************************/



extern void dsp0_pisplit_demo_library_load();
extern void dsp0_pisplit_demo_library_unload();

#endif


#endif



