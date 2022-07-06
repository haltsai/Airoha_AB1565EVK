

#ifndef  __DSP1_PISPLIT_DEMO_LIBRARY_PORTABLE_H__
#define  __DSP1_PISPLIT_DEMO_LIBRARY_PORTABLE_H__



#ifdef DSP1_PISPLIT_DEMO_LIBRARY



typedef char * (*p_dsp1_demo_lib_entry_1_t) (void);
typedef char * (*p_dsp1_demo_lib_entry_2_t) (void);
typedef char * (*p_dsp1_demo_lib_entry_3_t) (void);


extern void *dsp1_pisplit_demo_library_import_parameters[];


/*for export parameters*************************************************/
extern void *dsp1_pisplit_demo_library_export_parameters[];

#define dsp1_demo_lib_entry_1   ((p_dsp1_demo_lib_entry_1_t)dsp1_pisplit_demo_library_export_parameters[0])   //Have this Macro, will esay to porting your library.
#define dsp1_demo_lib_entry_2   ((p_dsp1_demo_lib_entry_2_t)dsp1_pisplit_demo_library_export_parameters[1])   //Have this Macro, will esay to porting your library.
#define dsp1_demo_lib_entry_3   ((p_dsp1_demo_lib_entry_3_t)dsp1_pisplit_demo_library_export_parameters[2])   //Have this Macro, will esay to porting your library.

/***********************************************************************/

#endif


#endif



